# SPDX-License-Identifier: zlib-acknowledgement

cmake_minimum_required(VERSION 3.20)
project(ELF
        VERSION 0.0.1
        DESCRIPTION "Statically Linked ELF Shared Object Reader"
        LANGUAGES C)
set_property(GLOBAL PROPERTY C_STANDARD 11)

find_program(DOXYGEN doxygen)
if(DOXYGEN)
  add_custom_target(docs
    COMMAND ${DOXYGEN} docs/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
endif()

find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
  # NOTE(Ryan): We could also specify a shell script as the command
  add_custom_target(format
    COMMAND ${CLANG_FORMAT}
    # Use the .clang-format file in our root
    -style=file
    # Edit the files in place
    -i
    # Don't fall back on any defaults if the clang-format file isn't found
    # Source search rules
    arch/**/**/*.h arch/**/**/*.c src/**/*.c src/**/*.h
    test/**/*.c test/**/*.h include/*.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  )
endif()

# IMPORTANT(Ryan): This property will remove a directory, while add_custom_target BYPRODUCTS will not.
set_target_properties(docs PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_BINARY_DIR}/doc/)

include(cmake/ArgsCheck.cmake)
# include(cmake/BuildOptions.cmake)
# include(cmake/Conversions.cmake)
# include(cmake/AddExecutableWrapper.cmake)

set(default_build_type "RelWithDebInfo")
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the default build type"
      FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY
               STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(default_shared_lib OFF)
# IMPORTANT(Ryan): Correctly handle OFF value or not set (better to use option())
# option(BUILD_SHARED_LIB "Enable building shared lib" OFF)
if("${BUILD_SHARED_LIB}" STREQUAL "")
  message(STATUS "Setting build shared library type to '${default_shared_lib}' as none was specified.")
  set(BUILD_SHARED_LIB "${default_shared_lib}" CACHE
      BOOL "Choose to build a shared library"
      FORCE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE 
    BOOL "Export compile commands" FORCE)

# NOTE(Ryan): When not a dependency
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  include(CTest)
  # TODO(Ryan): Current setup will not rebuild even if warnings show
  add_compile_options(-Wall -Wextra)
endif()

include(cmake/CPM.cmake)

# TODO(Ryan): Require building from source if wanting to cross-compile
find_package(cmocka REQUIRED)
# Course provides very convoluted way of handling external projects (cmocka)
# For the moment, this relatively simple command will suffice
#find_package(cmocka QUIET)
#if(NOT cmocka_FOUND)
#  CPMAddPackage(
#    NAME cmocka
#    GITHUB_REPOSITORY https://git.cryptomilk.org/projects/cmocka.git/
#    VERSION 1.1.5
#    GIT_TAG cmocka-1.1.5
#    OPTIONS
#      "WITH_EXAMPLES OFF"
#      "CMAKE_BUILD_TYPE DEBUG"
#  )
#endif()


# NOTE(Ryan): Compiling any library to be used should be an interface library
# Can also use interface to group a list of files together for say cross-compilation inclusion
# CPMAddPackage(
#   NAME printf
#   GITHUB_REPOSITORY mpaland/printf
#   VERSION 4.0.0
#   DOWNLOAD_ONLY
# )
#add_library(printf INTERFACE)
#target_sources(printf INTERFACE ${printf_SOURCE_DIR}/printf.c)
#target_include_directories(printf SYSTEM INTERFACE ${printf_SOURCE_DIR})

# TODO(Ryan): Remove STATIC as have an option now to control this
if(BUILD_SHARED_LIB)
  add_library(static-elf SHARED)
else()
  add_library(static-elf STATIC)
endif()
target_sources(static-elf PRIVATE code/static-elf.c)
target_include_directories(static-elf PUBLIC code)
# NOTE(Ryan): Enable LTO
set_property(TARGET static-elf PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

target_link_options(static-elf PRIVATE -TSTM32_FLASH.ld -L${CMAKE_CURRENT_LIST_DIR})
set_target_properties(static-elf PROPERTIES LINK_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/STM32_FLASH.ld)  

# IMPORTANT(Ryan): When generating files outside of CMake, use this binary dir
get_target_property(dir static-elf BINARY_DIR)
target_link_options(static-elf PRIVATE -Wl,-Map,${dir}/static-elf.map)
set_target_properties(static-elf PROPERTIES ADDITIONAL_CLEAN_FILES ${dir}/static-elf.map)  

# IMPORTANT(Ryan): Global variables likes set(CMAKE_C_FLAGS_DEBUG) only used for toolchain files
target_compile_definitions(static-elf PRIVATE
  STATIC_ELF_DEV
  ARCH_64
)

# if(BUILD_TESTING AND (ELF_BUILD_TESTING OR ...))
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
   # target_link_libraries(name PRIVATE val1 PUBLIC val2 ...)

   list(APPEND desired_c_flags "-Wno-unused-parameter")
   # apply_supported_c_compiler_flags(static-elf-tests PRIVATE desired_c_flags)

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

# TODO(Ryan): Do sucessive target_add()... overwrite (in which case generator expressions become essential)

# TODO(Ryan): Options just allow booleans?
# CMakeCache.txt store persistent build information and variables.
# There purpose is to allow users to configure build settings
# They are globally accessible
# Alter with -D<var><type>:=<value> (ccmake gives us graphical way)

# -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi-gcc.cmake
# keep toolchain files simple and layered. no complex logic. use individual one for each system

# if(NOT CMAKE_CROSSCOMPILING)

# Require the boolean operator to turn into 0 or 1
# target_compile_options(c PRIVATE $<$<BOOL:${CMAKE_BUILD_TYPE}>:SOMETHING>)
# also have builtin variable queries for generator expressions $<PLATFORM_ID:Unix>

# include(CMakeDependentOption)
# CMAKE_DEPENDENT_OPTION(LIBC_BUILD_TESTING
#   "Enable libc testing even when used as an external project."
#   OFF
#   "NOT CMAKE_CROSSCOMPILING" OFF)

# IMPORTANT(Ryan): Toolchain files don't have to cross-compile (don't modify CMAKE_SYSTEM variables)
#if(NOT CMAKE_TOOLCHAIN_FILE)
#    message(WARNING "[WARN] CMAKE_TOOLCHAIN_FILE not specified: Using XXX toolchain as the default.")
#    include(cmake/toolchains/your-default-toolchain.cmake)
#endif() 

# CMAKE_C_COMPILER_ID, CMAKE_SYSTEM_NAME
