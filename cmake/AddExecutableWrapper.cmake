function(add_executable target)
	# Forward all arguments to the CMake add_executable
	_add_executable(${target} ${ARGN})
	if(LINKER_SCRIPT_DEPENDENCIES)
		set_target_properties(${target} PROPERTIES
			LINK_DEPENDS "${LINKER_SCRIPT_DEPENDENCIES}")
	endif()
endfunction()
