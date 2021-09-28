# SPDX-License-Identifier: zlib-acknowledgement
cmake_minimum_required(VERSION 3.17)
project(ELF
        VERSION 0.0.1
        DESCRIPTION "Statically Linked ELF Shared Object Reader"
        LANGUAGES C)
set_property(GLOBAL PROPERTY C_STANDARD 11)

#include(cmake/CPM.cmake)
#
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

add_subdirectory(code)
# add_subdirectory(test)
