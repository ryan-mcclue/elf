# SPDX-License-Identifier: zlib-acknowledgement

# IMPORTANT(Ryan): If a variable is passed in, must double-dereference as a single
# dereference will only give the name
macro(__extract_var_value var value)
  if(NOT "${${var}}" STREQUAL "")
    set(value ${${var}})
  endif()
endmacro()

# IMPORTANT(Ryan): Assume the language passed in is actually valid and used in the project
# IMPORTANT(Ryan): String passed as "val1;val2;val3"
function(apply_supported_compiler_flags lang target scope flags)
  include(Check${lang}CompilerFlag)
  __extract_var_value(flags flags_var)
  foreach(flag ${flags_var})
    string(REPLACE - _ flag_var ${flag})
    cmake_language(CALL check_${lang}_compiler_flag ${flag} ${flag_var})
    if(${flag_var})
      target_compile_options(${target} ${scope} ${flag}) 
    endif()
  endforeach()
endfunction()

function(apply_supported_linker_flags lang target scope flags)
  include(CheckLinkerFlag)
  __extract_var_value(flags flags_var)
  foreach(flag ${flags_var})
    check_linker_flag(${lang} ${flag} ${flag_var})  
    if(${flag_var})
      target_link_options(${target} ${scope} ${flag}) 
    endif()
  endforeach()
endfunction()
