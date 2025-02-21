cmake_minimum_required(VERSION 3.22)

set(DATAHIERARCHYPLUGIN "DataHierarchyPlugin")
PROJECT(${DATAHIERARCHYPLUGIN})

set(PLUGIN
    src/DataHierarchyPlugin.h
    src/DataHierarchyPlugin.cpp
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

target_include_directories(${DATAHIERARCHYPLUGIN} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${DATAHIERARCHYPLUGIN} PRIVATE cxx_std_20)

target_link_libraries(${DATAHIERARCHYPLUGIN} PRIVATE Qt6::Widgets)
target_link_libraries(${DATAHIERARCHYPLUGIN} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${DATAHIERARCHYPLUGIN} PRIVATE ${MV_PUBLIC_LIB})

# Use AVX if enabled and available
mv_check_and_set_AVX(${DATAHIERARCHYPLUGIN} ${MV_USE_AVX})

set_target_properties(${DATAHIERARCHYPLUGIN} PROPERTIES AUTOMOC ON)

if(MV_UNITY_BUILD)
    set_target_properties(${DATAHIERARCHYPLUGIN} PROPERTIES UNITY_BUILD ON)
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
    set_property(TARGET ${DATAHIERARCHYPLUGIN} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/Debug,$<IF:$<CONFIG:RELWITHDEBINFO>,${MV_INSTALL_DIR}/RelWithDebInfo,${MV_INSTALL_DIR}/Release>>)
    set_property(TARGET ${DATAHIERARCHYPLUGIN} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,"${MV_INSTALL_DIR}/Debug/${MV_APPLICATION_NAME}.exe",$<IF:$<CONFIG:RELWITHDEBINFO>,"${MV_INSTALL_DIR}/RelWithDebInfo/${MV_APPLICATION_NAME}.exe","${MV_INSTALL_DIR}/Release/${MV_APPLICATION_NAME}.exe">>)
endif()
