# SPDX-License-Identifier: zlib-acknowledgement

function(apply_supported_compiler_flags lang target scope flag_list)
  include(Check${lang}CompilerFlag)
  # TODO(Ryan): Replace with a macro/function and move into .cmake file
  # String passed in as "-Wval;-Wother
  if(NOT "${${flag_list}}" STREQUAL "")
    set(flag_list ${${flag_list}})
  endif()
  # IMPORTANT(Ryan): If a variable is passed in, must double-dereference as a single
  # dereference will only give the name
  foreach(flag ${flag_list})
    string(REPLACE - _ flag_var ${flag})
    cmake_language(CALL check_${lang}_compiler_flag ${flag} ${flag_var})
    if(${flag_var})
      target_compile_options(${target} ${scope} ${flag}) 
    endif()
  endforeach()

  # TODO(Ryan): Check that the language is actually enabled?
endfunction()

# TODO(Ryan): Replace cmake_language with
# include(CheckLinkerFlag)
# check_linker_flag(${lang} ${flag} ${flag_var})  
