cmake_minimum_required(VERSION 3.22)

set(SHARED_PARAMETERS_PLUGIN "SharedParametersPlugin")
PROJECT(${SHARED_PARAMETERS_PLUGIN})

set(PLUGIN
    src/SharedParametersPlugin.h
    src/SharedParametersPlugin.cpp
)

set(MODELS
    src/PublicActionsModel.h
    src/PublicActionsModel.cpp
)

set(AUX
    src/SharedParametersPlugin.json
)

set(SHARED_PARAMETERS_SOURCES
    ${PLUGIN}
    ${MODELS}
    ${AUX}
)

source_group(Plugin FILES ${PLUGIN})
source_group(Models FILES ${MODELS})
source_group(Aux FILES ${AUX})

add_library(${SHARED_PARAMETERS_PLUGIN} SHARED ${SHARED_PARAMETERS_SOURCES})

add_dependencies(${SHARED_PARAMETERS_PLUGIN} ${MV_PUBLIC_LIB})

target_include_directories(${SHARED_PARAMETERS_PLUGIN} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include")

target_compile_features(${SHARED_PARAMETERS_PLUGIN} PRIVATE cxx_std_20)

target_link_libraries(${SHARED_PARAMETERS_PLUGIN} PRIVATE Qt6::Widgets)
target_link_libraries(${SHARED_PARAMETERS_PLUGIN} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${SHARED_PARAMETERS_PLUGIN} PRIVATE ${MV_PUBLIC_LIB})

# Use AVX if enabled and available
mv_check_and_set_AVX(${SHARED_PARAMETERS_PLUGIN} ${MV_USE_AVX})

set_target_properties(${SHARED_PARAMETERS_PLUGIN} PROPERTIES AUTOMOC ON)

install(TARGETS ${SHARED_PARAMETERS_PLUGIN}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
)

add_custom_command(TARGET ${SHARED_PARAMETERS_PLUGIN} POST_BUILD
	COMMAND "${CMAKE_COMMAND}"
	--install ${CMAKE_CURRENT_BINARY_DIR}
	--config $<CONFIGURATION>
	--prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
)

# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
    set_property(TARGET ${SHARED_PARAMETERS_PLUGIN} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/Debug,$<IF:$<CONFIG:RELWITHDEBINFO>,${MV_INSTALL_DIR}/RelWithDebInfo,${MV_INSTALL_DIR}/Release>>)
    set_property(TARGET ${SHARED_PARAMETERS_PLUGIN} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,"${MV_INSTALL_DIR}/Debug/${MV_APPLICATION_NAME}.exe",$<IF:$<CONFIG:RELWITHDEBINFO>,"${MV_INSTALL_DIR}/RelWithDebInfo/${MV_APPLICATION_NAME}.exe","${MV_INSTALL_DIR}/Release/${MV_APPLICATION_NAME}.exe">>)
endif()
