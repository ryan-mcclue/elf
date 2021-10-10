if($ENV{ARM_CORTEX_M3_TOOLCHAIN_INCLUDED}) 
    return() 
endif() 

set(CPU_NAME cortex-m3)
set(CPU_FLAGS "-mcpu=cortex-m3 -mthumb")
set(VFP_FLAGS "-mfloat-abi=soft")

include(${CMAKE_CURRENT_LIST_DIR}/arm-none-eabi-gcc.cmake)

set(ENV{ARM_CORTEX_M3_TOOLCHAIN_INCLUDED} true)
