# SPDX-License-Identifier: zlib-acknowledgement

#add_library(static-elf STATIC)
add_executable(static-elf)

target_sources(static-elf PRIVATE
  static-elf.c
)
target_compile_definitions(static-elf PRIVATE
  STATIC_ELF_DEV
  ARCH_64  
)

#if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
#  # we aren't being used as a dependency
#endif()

# CMAKE_MODULE_PATH
# opt to use CPM as scattered, e.g. just download, package, pkg-config etc.  

# ${PROJECT_SOURCE_DIR}

#add_executable(emt)
#
#find_library(CMOCKA_LIB
#             NAMES cmocka 
#             REQUIRED)
#
#target_sources(emt PRIVATE
#               emt.c)
#target_link_libraries(emt PRIVATE
#                      ${CMOCKA_LIB})


#target_compile_definitions(emt PRIVATE
#                           EMT_DEV)
#target_include_directories(emt PUBLIC ${CMAKE_PROJECT_DIR}/include)
