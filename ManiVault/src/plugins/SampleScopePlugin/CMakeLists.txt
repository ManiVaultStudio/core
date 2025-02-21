cmake_minimum_required(VERSION 3.22)

set(SAMPLE_CONTEXT_PLUGIN "SampleScopePlugin")
PROJECT(${SAMPLE_CONTEXT_PLUGIN})

set(PLUGIN
    src/SampleScopePlugin.h
    src/SampleScopePlugin.cpp
)

set(PLUGIN_MOC_HEADERS
	src/SampleScopePlugin.h
)

set(WIDGETS
    src/SampleScopeWidget.h
    src/SampleScopeWidget.cpp
)

set(AUX
    src/SampleScopePlugin.json
)

set(SAMPLE_SCOPE_PLUGIN_SOURCES
    ${PLUGIN}
    ${WIDGETS}
    ${AUX}
)

source_group(Plugin FILES ${PLUGIN})
source_group(Widgets FILES ${WIDGETS})
source_group(Aux FILES ${AUX})

add_library(${SAMPLE_CONTEXT_PLUGIN} SHARED ${SAMPLE_SCOPE_PLUGIN_SOURCES})

qt_wrap_cpp(DATAPROPERTIES_MOC ${PLUGIN_MOC_HEADERS} TARGET ${SAMPLE_CONTEXT_PLUGIN})
target_sources(${SAMPLE_CONTEXT_PLUGIN} PRIVATE ${DATAPROPERTIES_MOC})

target_include_directories(${SAMPLE_CONTEXT_PLUGIN} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${SAMPLE_CONTEXT_PLUGIN} PRIVATE cxx_std_20)

target_link_libraries(${SAMPLE_CONTEXT_PLUGIN} PRIVATE Qt6::Core)
target_link_libraries(${SAMPLE_CONTEXT_PLUGIN} PRIVATE Qt6::Widgets)
target_link_libraries(${SAMPLE_CONTEXT_PLUGIN} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${SAMPLE_CONTEXT_PLUGIN} PRIVATE ${MV_PUBLIC_LIB})

# Use AVX if enabled and available
mv_check_and_set_AVX(${SAMPLE_CONTEXT_PLUGIN} ${MV_USE_AVX})

if(MV_UNITY_BUILD)
    set_target_properties(${SAMPLE_CONTEXT_PLUGIN} PROPERTIES UNITY_BUILD ON)
    set_source_files_properties(${DATAPROPERTIES_MOC} PROPERTIES
                        SKIP_UNITY_BUILD_INCLUSION TRUE)
endif()

install(TARGETS ${SAMPLE_CONTEXT_PLUGIN}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
)

add_custom_command(TARGET ${SAMPLE_CONTEXT_PLUGIN} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    --install ${CMAKE_CURRENT_BINARY_DIR}
    --config $<CONFIGURATION>
    --prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
)

# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
    set_property(TARGET ${SAMPLE_CONTEXT_PLUGIN} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/debug,${MV_INSTALL_DIR}/release>)
    set_property(TARGET ${SAMPLE_CONTEXT_PLUGIN} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/debug/${MV_APPLICATION_NAME}.exe,${MV_INSTALL_DIR}/release/${MV_APPLICATION_NAME}.exe>)
endif()
