// SPDX-License-Identifier: zlib-acknowledgement

#include <asm/param.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include <elf.h>

#include "static-elf.h"

#if defined(STATIC_ELF_DEV)
void __bp(void) 
{ 
  return;
}
void __ebp(void)
{ 
  __attribute__((unused)) char *err = strerror(errno);
  return;
}
#define BP() __bp()
#define EBP() __ebp()
#else
#define BP()
#define EBP()
#endif

#if defined(ARCH_64)
  typedef Elf64_Ehdr ElfHeader;
  typedef Elf64_Phdr ElfProgramHeader;
  typedef Elf64_Shdr ElfSectionHeader;
  typedef Elf64_Sym ElfSymbol;
#else
  typedef Elf32_Ehdr ElfHeader;
  typedef Elf32_Phdr ElfProgramHeader;
  typedef Elf32_Shdr ElfSectionHeader;
  typedef Elf32_Sym ElfSymbol;
#endif

typedef uint8_t u8;

typedef struct {
  void *contents;
  size_t size;
  int errno_code;
} ReadFileResult;

ReadFileResult
read_entire_file(char *file_name)
{
    ReadFileResult result = {0};

    int open_res = open(file_name, O_RDONLY); 
    if (open_res < 0) 
    {
      EBP();
      result.errno_code = errno; 
      goto end;
    }
    
    int file_fd = open_res;

    struct stat file_status = {0};
    int fstat_res = fstat(file_fd, &file_status);
    if (fstat_res < 0) 
    {
      EBP();
      result.errno_code = errno; 
      goto end_open;
    }

    result.contents = malloc(file_status.st_size);
    if (result.contents == NULL)
    {
      EBP();
      result.errno_code = errno;
      goto end_open;
    }
    result.size = file_status.st_size;

    size_t bytes_to_read = file_status.st_size;
    u8 *byte_location = (u8 *)result.contents;
    while (bytes_to_read > 0) 
    {
      int read_res = read(file_fd, byte_location, bytes_to_read); 
      if (read_res < 0) 
      {
        EBP();
        result.errno_code = errno;
        free(result.contents);
        goto end_open;
      }
      else
      {
        int bytes_read = read_res;
        bytes_to_read -= bytes_read;
        byte_location += bytes_read;
      }
    }

end_open:
  close(file_fd); 
end:
  return result;
}

ElfLoadResult
elf_load(char *name)
{
  ElfLoadResult result = {0};

  ReadFileResult so_file_res = read_entire_file(name);
  if (-so_file_res.errno_code < 0)
  {
    BP();
    result.error_code = ELF_LOAD_ERROR_READ_FILE_FAIL;
    goto end;
  }
  result.in_file = so_file_res.contents;

  ElfHeader *header = (ElfHeader *)result.in_file;
  bool is_so_elf_file = (header->e_type == 3);
  if (!is_so_elf_file)
  {
    BP();
    result.error_code = ELF_LOAD_ERROR_NOT_SO_FILE;
    free(so_file_res.contents);
    goto end;
  }

  size_t largest_mem_size = 0;
  for (int prog_header_i = 0; prog_header_i < header->e_phnum; ++prog_header_i)
  {
    ElfProgramHeader *prog_header = \
        ((ElfProgramHeader *)((u8 *)header + header->e_phoff) + prog_header_i);

    bool is_header_loadable = (prog_header->p_type == 1);
    if (!is_header_loadable) continue;

    size_t cur_mem_size = prog_header->p_vaddr + prog_header->p_memsz;
    if (cur_mem_size > largest_mem_size) 
    {
      largest_mem_size = cur_mem_size; 
    }
  }

  u8 *lib_mem = mmap(NULL, largest_mem_size, PROT_READ | PROT_WRITE,
                      MAP_ANON | MAP_PRIVATE, -1, 0);
  if (lib_mem == MAP_FAILED)
  {
    BP();
    result.error_code = ELF_LOAD_ERROR_MEM_ALLOC_FAIL;
    free(so_file_res.contents);
    goto end;
  }
  result.in_memory = lib_mem;

  for (int prog_header_i = 0; prog_header_i < header->e_phnum; ++prog_header_i) {
    ElfProgramHeader *prog_header = \
        ((ElfProgramHeader *)((u8 *)header + header->e_phoff) + prog_header_i);

    bool is_header_loadable = (prog_header->p_type == 1);
    if (!is_header_loadable) continue;

    memmove(result.in_memory + prog_header->p_vaddr, 
            (u8 *)header + prog_header->p_offset,
            prog_header->p_filesz);
    
    if (!(prog_header->p_flags & PF_W)) 
    {
      int mprotect_res = \
        mprotect(result.in_memory + prog_header->p_vaddr, prog_header->p_memsz, PROT_READ);
      if (mprotect_res < 0)
      {
        BP();
        result.error_code = ELF_LOAD_ERROR_MPROTECT_FAIL;
        free(so_file_res.contents);
        munmap(lib_mem, largest_mem_size);
        goto end;
      }
    }

    if (prog_header->p_flags & PF_X) 
    {
      int mprotect_res = \
        mprotect(result.in_memory + prog_header->p_vaddr, prog_header->p_memsz, PROT_EXEC);
      if (mprotect_res < 0)
      {
        BP();
        result.error_code = ELF_LOAD_ERROR_MPROTECT_FAIL;
        free(so_file_res.contents);
        munmap(lib_mem, largest_mem_size);
        goto end;
      }
    }
  }
end:
  return result;
}

void *
elf_lookup_function(ElfLoadResult *load_result, char *function_name)
{
  void *function = NULL;

  ElfHeader *header = (ElfHeader *)load_result->in_file;
  
  ElfSectionHeader *section_header_start = \
      (ElfSectionHeader *)((u8 *)header + header->e_shoff);

  for (int section_header_i = 0; section_header_i < header->e_shnum; ++section_header_i)
  {
    ElfSectionHeader *section_header = \
        ((ElfSectionHeader *)((u8 *)header + header->e_shoff) + section_header_i);

    if (section_header->sh_type == SHT_DYNSYM)
    {
      u8 *strings = \
        (u8 *)header + section_header_start[section_header->sh_link].sh_offset;
      for (int sym_count = 0;
           sym_count < section_header->sh_size / sizeof(ElfSymbol); 
           ++sym_count) 
      {
        ElfSymbol *symbol = \
            ((ElfSymbol *)((u8 *)header + section_header->sh_offset) + sym_count);

        if (strcmp(function_name, strings + symbol->st_name) == 0) 
        {
          function = load_result->in_memory + symbol->st_value;
        }
      }
    }
  }
  
  return function;
}

//int
//main(int argc, char *argv[])
//{
//  ElfLoadResult elf_load_result = elf_load("adder.so");
//  if (elf_load_result.error_code < 0)
//  {
//    BP();
//    //printf("%s\n", elf_load_result_get_error_str(&elf_load_result));
//    //Failed to load elf file %s (%s)\n", elf_load_result.name, 
//    return EXIT_FAILURE;
//  }
//
//  int (*function)(int, int) = elf_lookup_function(&elf_load_result, "add_int");
//  if (function == NULL)
//  {
//    //printf("Failed to find %s inside elf %s\n", function_name, elf_load_result.name);
//    BP();
//    return EXIT_FAILURE;
//  }
//
//  int res = function(10, 23);
//
//  return EXIT_SUCCESS;
//}
