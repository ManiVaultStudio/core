function(mv_handle_plugin_config plugin_target)
    
    # Read config file
    set(PLUGIN_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/PluginInfo.json")

    if(NOT EXISTS "${PLUGIN_CONFIG_FILE}")
        message(FATAL_ERROR "JSON file not found: ${PLUGIN_CONFIG_FILE}")
    endif()

    file(READ ${PLUGIN_CONFIG_FILE} PLUGIN_INFO_JSON)

    # Check if config file contains certain entries
    set(HAS_PLUGIN_TYPE 0)

    # Get the number of top-level keys
    string(JSON TOP_LEVEL_COUNT LENGTH "${PLUGIN_INFO_JSON}")

    math(EXPR LAST_INDEX "${TOP_LEVEL_COUNT} - 1")
    foreach(I RANGE 0 ${LAST_INDEX})
        string(JSON KEY_NAME MEMBER "${PLUGIN_INFO_JSON}" ${I})
        if("${KEY_NAME}" STREQUAL "type")
            set(HAS_PLUGIN_TYPE 1)
        endif()
    endforeach()

    string(JSON PLUGIN_NAME GET "${PLUGIN_INFO_JSON}" name)
    message(STATUS "ManiVault plugin: ${PLUGIN_NAME}")

    # Extract plugin version
    string(JSON PLUGIN_VERSION GET "${PLUGIN_INFO_JSON}" version plugin)
    message(STATUS "  version: ${PLUGIN_VERSION}")
    set_target_properties(${plugin_target} PROPERTIES
        OUTPUT_NAME  "${plugin_target}_p${PLUGIN_VERSION}_c${ManiVault_VERSION}"
    )

    # Extract plugin type
    if(HAS_PLUGIN_TYPE)
        string(JSON PLUGIN_TYPE GET "${PLUGIN_INFO_JSON}" type)
        message(STATUS "  type: ${PLUGIN_TYPE}")
        set_target_properties(${plugin_target} PROPERTIES
            FOLDER "${PLUGIN_TYPE}Plugins"
        )
    endif()

endfunction()
