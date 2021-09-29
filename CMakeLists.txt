# SPDX-License-Identifier: zlib-acknowledgement

# TODO(Ryan): Arch detection
# build type options that pass flags onwards
# compiler and linker flags (compile sample lib)
cmake_minimum_required(VERSION 3.17)
project(ELF
        VERSION 0.0.1
        DESCRIPTION "Statically Linked ELF Shared Object Reader"
        LANGUAGES C)
set_property(GLOBAL PROPERTY C_STANDARD 11)

include(cmake/CPM.cmake)

find_package(cmocka QUIET)
if(NOT cmocka_FOUND)
  CPMAddPackage(
    NAME cmocka
    GITHUB_REPOSITORY https://git.cryptomilk.org/projects/cmocka.git/
    VERSION 1.1.5
    GIT_TAG cmocka-1.1.5
    OPTIONS
      "WITH_EXAMPLES OFF"
      "CMAKE_BUILD_TYPE DEBUG"
  )
endif()

#CPMAddPackage(
#  NAME printf
#  GITHUB_REPOSITORY mpaland/printf
#  VERSION 4.0.0
#  DOWNLOAD_ONLY
#)
#add_library(printf INTERFACE)
#target_sources(printf INTERFACE ${printf_SOURCE_DIR}/printf.c)
#target_include_directories(printf SYSTEM INTERFACE ${printf_SOURCE_DIR})

add_library(static-elf STATIC)
target_sources(static-elf PRIVATE code/static-elf.c)
target_include_directories(static-elf PUBLIC code)
target_compile_definitions(static-elf PRIVATE
  STATIC_ELF_DEV
  ARCH_64
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
   #add_library(sample-elf SHARED)
   #target_sources(sample-elf PRIVATE tests/sample-elf.c)
   #target_link_libraries(sample-elf PRIVATE printf)

   add_executable(test-static-elf)
   target_sources(test-static-elf PRIVATE tests/test-static-elf.c)
   target_include_directories(test-static-elf PRIVATE code)
   target_include_directories(test-static-elf SYSTEM PRIVATE ${CMOCKA_INCLUDE_DIR})
   target_link_libraries(test-static-elf PRIVATE ${CMOCKA_LIBRARIES} static-elf)
endif()
