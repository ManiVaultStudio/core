cmake_minimum_required(VERSION 3.22)

set(LOGGINGPLUGIN "LoggingPlugin")
PROJECT(${LOGGINGPLUGIN})

set(PLUGIN
    src/LoggingPlugin.h
    src/LoggingPlugin.cpp
)

set(WIDGETS
    src/LoggingWidget.h
    src/LoggingWidget.cpp
)

set(AUX
    src/LoggingPlugin.json
)

set(LOGGING_SOURCES
    ${PLUGIN}
    ${WIDGETS}
    ${AUX}
)

source_group(Plugin FILES ${PLUGIN})
source_group(Widgets FILES ${WIDGETS})
source_group(Aux FILES ${AUX})

add_library(${LOGGINGPLUGIN} SHARED ${LOGGING_SOURCES})

target_include_directories(${LOGGINGPLUGIN} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${LOGGINGPLUGIN} PRIVATE cxx_std_20)

target_link_libraries(${LOGGINGPLUGIN} PRIVATE Qt6::Widgets)
target_link_libraries(${LOGGINGPLUGIN} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${LOGGINGPLUGIN} PRIVATE ${MV_PUBLIC_LIB})

# Use AVX if enabled and available
mv_check_and_set_AVX(${LOGGINGPLUGIN} ${MV_USE_AVX})

set_target_properties(${LOGGINGPLUGIN} PROPERTIES AUTOMOC ON)

install(TARGETS ${LOGGINGPLUGIN}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
)

add_custom_command(TARGET ${LOGGINGPLUGIN} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    --install ${CMAKE_CURRENT_BINARY_DIR}
    --config $<CONFIGURATION>
    --prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
)

# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
    set_property(TARGET ${LOGGINGPLUGIN} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/Debug,$<IF:$<CONFIG:RELWITHDEBINFO>,${MV_INSTALL_DIR}/RelWithDebInfo,${MV_INSTALL_DIR}/Release>>)
    set_property(TARGET ${LOGGINGPLUGIN} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,"${MV_INSTALL_DIR}/Debug/${MV_APPLICATION_NAME}.exe",$<IF:$<CONFIG:RELWITHDEBINFO>,"${MV_INSTALL_DIR}/RelWithDebInfo/${MV_APPLICATION_NAME}.exe","${MV_INSTALL_DIR}/Release/${MV_APPLICATION_NAME}.exe">>)
endif()
