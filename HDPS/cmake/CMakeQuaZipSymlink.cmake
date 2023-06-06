# -----------------------------------------------------------------------------
# Create a symbolic link for the shared quazip library
# -----------------------------------------------------------------------------

# QuaZip sets its Version to Major.Minor instead of Major.Minor.Patch (e.g. 1.4 instead of 1.4.0)
# On some linux systems, cmake links automatically against libquazip1-qt6.so.1.4.0 instead of libquazip1-qt6.so.1.4 though
# Here, we create a symbolic link from the full version name to the shared library
macro(create_quazip_symlink)
message(STATUS "Set instruction sets for ${target}, useavx is ${useavx}")

get_target_property(quazip_name QuaZip OUTPUT_NAME)
get_target_property(quazip_ver QuaZip SOVERSION)

string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)" QUAZIP_VER_MATCH ${quazip_ver})
set(quazip_ver_major ${CMAKE_MATCH_1})
set(quazip_ver_minor ${CMAKE_MATCH_2})
set(quazip_file_base ${CMAKE_SHARED_LIBRARY_PREFIX}${quazip_name}$<IF:$<CONFIG:DEBUG>,d,>${CMAKE_SHARED_LIBRARY_SUFFIX})
set(quazip_lib_name ${INSTALL_DIR}/$<CONFIGURATION>/lib/${quazip_file_base}.${quazip_ver_major}.${quazip_ver_minor})
set(quazip_symlink_name ${INSTALL_DIR}/$<CONFIGURATION>/lib/${quazip_file_base}.${quazip_ver})

add_custom_command(TARGET ${HDPS_EXE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${quazip_lib_name} ${quazip_symlink_name}
)

endmacro()