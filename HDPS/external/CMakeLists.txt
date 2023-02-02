# Perform submodule patches.
#
# 1. reset any changes in the repo and then 
# 2. git apply the provided patch to the submodule dir
#
# Patches are located in external/patches directory.
# Patched are created by applying the change and then
# git diff --ignore-cr-at-eol > patchfilename.patch
#
macro(patch DIR patch_NAME)
	message(STATUS "Patching submodule in ${DIR}")
	execute_process(
		COMMAND git checkout .
		WORKING_DIRECTORY ${DIR}
		RESULT_VARIABLE ERR_CODE
	)
	if (NOT "${ERR_CODE}" STREQUAL "0")
	  message(FATAL_ERROR "Error when resetting ${DIR}.")
	endif ()
	execute_process(
		COMMAND git apply --ignore-whitespace ../patches/${patch_NAME}
		WORKING_DIRECTORY ${DIR}
		RESULT_VARIABLE ERR_CODE
	)
	if (NOT "${ERR_CODE}" STREQUAL "0")
	  message(FATAL_ERROR "Error when patching ${DIR}.")
	endif ()
endmacro()


patch(${CMAKE_CURRENT_SOURCE_DIR}/advanced_docking ad_install.patch)