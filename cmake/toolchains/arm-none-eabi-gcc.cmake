# IMPORTANT(Ryan): Changes to toolchain files must be followed by deletion of build/

set(CMAKE_SYSTEM_NAME Generic)
# TODO(Ryan): Perhaps specify arm32 here as from armv8 can be 64bit. aarch64 is different ISA
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CPU_NAME cortex-m4)

set(CMAKE_C_COMPILER    arm-none-eabi-gcc) set(CMAKE_CXX_COMPILER  arm-none-eabi-g++)
set(AS                  arm-none-eabi-as)
set(CMAKE_AR            arm-none-eabi-gcc-ar)
set(OBJCOPY             arm-none-eabi-objcopy)
set(OBJDUMP             arm-none-eabi-objdump)
set(SIZE                arm-none-eabi-size)

# NOTE(Ryan): Look in build machine
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# NOTE(Ryan): Look in host machine (CPM will download and compile dependency for us)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# NOTE(Ryan): Allow not having to define linker flags
# set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# IMPORTANT(Ryan): These flags are overridden in derived toolchain files
set(CMAKE_C_FLAGS_INIT
	"-mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb -fdata-sections -ffunction-sections"
	CACHE
	INTERNAL "Default C compiler flags.")
set(CMAKE_ASM_FLAGS_INIT
	"${CPU_FLAGS} -x assembler-with-cpp"
	CACHE
	INTERNAL "Default ASM compiler flags.")
set(CMAKE_CXX_FLAGS_INIT
	"-mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb -fdata-sections -ffunction-sections"
	CACHE
	INTERNAL "Default C++ compiler flags.")
set(CMAKE_EXE_LINKER_FLAGS_INIT
	"-Wl,--gc-sections"
	CACHE
	INTERNAL "Default linker flags.")

# TODO(Ryan): Understand arm naming and floating point arguments
# https://embeddedartistry.com/blog/2017/10/11/demystifying-arm-floating-point-compiler-options/
set(CMAKE_C_FLAGS_INIT
	"${CPU_FLAGS} ${VFP_FLAGS} -fdata-sections -ffunction-sections"
	CACHE
	INTERNAL "Default C compiler flags.")
set(CMAKE_CXX_FLAGS_INIT
	"${CPU_FLAGS} ${VFP_FLAGS} -fdata-sections -ffunction-sections"
	CACHE
	INTERNAL "Default C++ compiler flags.")
set(CMAKE_ASM_FLAGS_INIT
	"${CPU_FLAGS} -x assembler-with-cpp"
	CACHE
	INTERNAL "Default ASM compiler flags.")
set(CMAKE_EXE_LINKER_FLAGS_INIT
	"${LD_FLAGS} -Wl,--gc-sections"
	CACHE
	INTERNAL "Default linker flags.")

# NOTE(Ryan): Include from current directory of cmake file
# include(${CMAKE_CURRENT_LIST_DIR}/arm-none-eabi-gcc.cmake) 
