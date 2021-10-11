set(LINKER_SCRIPT_DIR ${CMAKE_SOURCE_DIR}/misc/linker-scripts/stm)
set(LINKER_SCRIPT_PRIMARY_FILE STM32F103VBIx_FLASH.ld)
set(LD_FLAGS "-T${LINKER_SCRIPT_PRIMARY_FILE} -L${LINKER_SCRIPT_DIR}")

# IMPORTANT(Ryan): Cannot define functions in toolchains, so define variable that will be used
# in function
list(APPEND LINKER_SCRIPT_DEPENDENCIES 
	"${LINKER_SCRIPT_DIR}/${LINKER_SCRIPT_PRIMARY_FILE}")
