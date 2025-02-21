cmake_minimum_required(VERSION 3.22)

set(DATAHIERARCHYPLUGIN "DataHierarchyPlugin")
PROJECT(${DATAHIERARCHYPLUGIN})

set(PLUGIN
    src/DataHierarchyPlugin.h
    src/DataHierarchyPlugin.cpp
)

set(PLUGIN_MOC_HEADERS
    src/DataHierarchyPlugin.h
    src/UnhideAction.h
    src/StatisticsAction.h
)

set(ACTIONS
    src/UnhideAction.h
    src/UnhideAction.cpp
    src/StatisticsAction.h
    src/StatisticsAction.cpp
)

set(WIDGETS
    src/DataHierarchyWidget.h
    src/DataHierarchyWidget.cpp
    src/DataHierarchyWidgetContextMenu.h
    src/DataHierarchyWidgetContextMenu.cpp
)

set(AUX
    src/DataHierarchyPlugin.json
)

set(DATAHIERARCHY_SOURCES
    ${PLUGIN}
    ${ACTIONS}
    ${WIDGETS}
    ${AUX}
)

source_group(Plugin FILES ${PLUGIN})
source_group(Actions FILES ${ACTIONS})
source_group(Widgets FILES ${WIDGETS})
source_group(Aux FILES ${AUX})

add_library(${DATAHIERARCHYPLUGIN} SHARED ${DATAHIERARCHY_SOURCES})

qt_wrap_cpp(DATAHIERARCHY_MOC ${PLUGIN_MOC_HEADERS} TARGET ${DATAHIERARCHYPLUGIN})
target_sources(${DATAHIERARCHYPLUGIN} PRIVATE ${DATAHIERARCHY_MOC})

target_include_directories(${DATAHIERARCHYPLUGIN} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${DATAHIERARCHYPLUGIN} PRIVATE cxx_std_20)

target_link_libraries(${DATAHIERARCHYPLUGIN} PRIVATE Qt6::Widgets)
target_link_libraries(${DATAHIERARCHYPLUGIN} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${DATAHIERARCHYPLUGIN} PRIVATE ${MV_PUBLIC_LIB})

# Use AVX if enabled and available
mv_check_and_set_AVX(${DATAHIERARCHYPLUGIN} ${MV_USE_AVX})

if(MV_UNITY_BUILD)
    set_target_properties(${DATAHIERARCHYPLUGIN} PROPERTIES UNITY_BUILD ON)
    set_source_files_properties(${DATAHIERARCHY_MOC} PROPERTIES
    SKIP_UNITY_BUILD_INCLUSION TRUE)
endif()

install(TARGETS ${DATAHIERARCHYPLUGIN}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
)

add_custom_command(TARGET ${DATAHIERARCHYPLUGIN} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    --install ${CMAKE_CURRENT_BINARY_DIR}
    --config $<CONFIGURATION>
    --prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
)

# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
    set_property(TARGET ${DATAHIERARCHYPLUGIN} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/debug,${MV_INSTALL_DIR}/release>)
    set_property(TARGET ${DATAHIERARCHYPLUGIN} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/debug/${MV_APPLICATION_NAME}.exe,${MV_INSTALL_DIR}/release/${MV_APPLICATION_NAME}.exe>)
endif()
