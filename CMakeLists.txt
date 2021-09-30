# SPDX-License-Identifier: zlib-acknowledgement

# TODO(Ryan): Arch detection
# build type options that pass flags onwards
# compiler and linker flags (compile sample lib)
cmake_minimum_required(VERSION 3.20)
project(ELF
        VERSION 0.0.1
        DESCRIPTION "Statically Linked ELF Shared Object Reader"
        LANGUAGES C)
set_property(GLOBAL PROPERTY C_STANDARD 11)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  include(CTest)
endif()

include(cmake/CPM.cmake)

# Course provides very convoluted way of handling external projects (cmocka)
# For the moment, this relatively simple command will suffice
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


# NOTE(Ryan): Compiling any library to be used should be an interface library
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

# ON by default
if(BUILD_TESTING)
   #add_library(sample-elf SHARED)
   #target_sources(sample-elf PRIVATE tests/sample-elf.c)
   #target_link_libraries(sample-elf PRIVATE printf)
   add_executable(static-elf-tests)
   target_sources(static-elf-tests PRIVATE tests/static-elf-tests.c)
   target_include_directories(static-elf-tests PRIVATE code)
   target_include_directories(static-elf-tests SYSTEM PRIVATE ${CMOCKA_INCLUDE_DIR})
   target_link_libraries(static-elf-tests PRIVATE ${CMOCKA_LIBRARIES} static-elf)

   # TODO(Ryan): When building sample test executables, use CMAKE_BINARY_DIR
   # ninja -t targets to list targets for debugging
   add_custom_target(test-static-elf
     COMMAND static-elf-tests
     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests
   )

   add_test(NAME sample-elf.test
     COMMAND static-elf-tests
     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests
   )
endif()
