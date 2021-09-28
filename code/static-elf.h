// SPDX-License-Identifier: zlib-acknowledgement
#pragma once

typedef enum {
  ELF_LOAD_ERROR_READ_FILE_FAIL = -5000,
  ELF_LOAD_ERROR_NOT_SO_FILE,
  ELF_LOAD_ERROR_MEM_ALLOC_FAIL,
  ELF_LOAD_ERROR_MPROTECT_FAIL,
} ELF_LOAD_ERROR;

typedef struct {
  unsigned char *in_memory;
  void *in_file;
  ELF_LOAD_ERROR error_code;
} ElfLoadResult;

ElfLoadResult
elf_load(char *name);

void *
elf_lookup_function(ElfLoadResult *load_result, char *function_name);
