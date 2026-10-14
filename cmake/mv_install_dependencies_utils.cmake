function(mv_install_dependencies main_target)
    
    set(MAIN_PROJECT_TARGET ${main_target})

    foreach(TARGET_STR IN LISTS ARGV)
        list(APPEND DEPENDENCIES_FOLDERS "$<TARGET_FILE_DIR:${TARGET_STR}>")
    endforeach()

    include(CMakePackageConfigHelpers)
    set(CONFIG_FILE_IN "${ManiVault_CMAKE_DIR}/mv_install_dependencies.cmake.in")
    set(CONFIG_FILE_OUT "${CMAKE_CURRENT_BINARY_DIR}/mv_install_dependencies_${main_target}.cmake")
    configure_file("${CONFIG_FILE_IN}" "${CONFIG_FILE_OUT}" @ONLY)

    include("${CONFIG_FILE_OUT}")

    add_custom_command(TARGET ${main_target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: Plugin dependencies"
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --component PLUGIN_DEPENDENCIES
        --prefix ${ManiVault_INSTALL_DIR}/$<CONFIGURATION>
        --verbose
    )

endfunction()
