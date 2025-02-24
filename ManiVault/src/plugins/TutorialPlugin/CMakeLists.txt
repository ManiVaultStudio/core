cmake_minimum_required(VERSION 3.22)

set(TUTORIAL_PLUGIN "TutorialPlugin")
PROJECT(${TUTORIAL_PLUGIN})

set(PLUGIN
    src/TutorialPlugin.h
    src/TutorialPlugin.cpp
)

set(WIDGETS
    src/TutorialWidget.h
    src/TutorialWidget.cpp
)

set(AUX
    src/TutorialPlugin.json
)

set(TUTORIAL_PLUGIN_SOURCES
    ${PLUGIN}
    ${WIDGETS}
    ${AUX}
)

source_group(Plugin FILES ${PLUGIN})
source_group(Widgets FILES ${WIDGETS})
source_group(Aux FILES ${AUX})

add_library(${TUTORIAL_PLUGIN} SHARED ${TUTORIAL_PLUGIN_SOURCES})

target_include_directories(${TUTORIAL_PLUGIN} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${TUTORIAL_PLUGIN} PRIVATE cxx_std_20)

target_link_libraries(${TUTORIAL_PLUGIN} PRIVATE Qt6::Core)
target_link_libraries(${TUTORIAL_PLUGIN} PRIVATE Qt6::Widgets)
target_link_libraries(${TUTORIAL_PLUGIN} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${TUTORIAL_PLUGIN} PRIVATE ${MV_PUBLIC_LIB})

# Use AVX if enabled and available
mv_check_and_set_AVX(${TUTORIAL_PLUGIN} ${MV_USE_AVX})

set_target_properties(${TUTORIAL_PLUGIN} PROPERTIES AUTOMOC ON)

if(MV_UNITY_BUILD)
    set_target_properties(${TUTORIAL_PLUGIN} PROPERTIES UNITY_BUILD ON)
endif()

install(TARGETS ${TUTORIAL_PLUGIN}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
)

add_custom_command(TARGET ${TUTORIAL_PLUGIN} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    --install ${CMAKE_CURRENT_BINARY_DIR}
    --config $<CONFIGURATION>
    --prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
)

# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
    set_property(TARGET ${TUTORIAL_PLUGIN} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/Debug,$<IF:$<CONFIG:RELWITHDEBINFO>,${MV_INSTALL_DIR}/RelWithDebInfo,${MV_INSTALL_DIR}/Release>>)
    set_property(TARGET ${TUTORIAL_PLUGIN} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,"${MV_INSTALL_DIR}/Debug/${MV_APPLICATION_NAME}.exe",$<IF:$<CONFIG:RELWITHDEBINFO>,"${MV_INSTALL_DIR}/RelWithDebInfo/${MV_APPLICATION_NAME}.exe","${MV_INSTALL_DIR}/Release/${MV_APPLICATION_NAME}.exe">>)
endif()
