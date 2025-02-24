cmake_minimum_required(VERSION 3.22)

set(TASKS_PLUGIN "TasksPlugin")
PROJECT(${TASKS_PLUGIN})

set(PLUGIN
    src/TasksPlugin.h
    src/TasksPlugin.cpp
)

set(AUX
    src/TasksPlugin.json
)

set(TASKS_SOURCES
    ${PLUGIN}
    ${AUX}
)

source_group(Plugin FILES ${PLUGIN})
source_group(Aux FILES ${AUX})

add_library(${TASKS_PLUGIN} SHARED ${TASKS_SOURCES})

add_dependencies(${TASKS_PLUGIN} ${MV_PUBLIC_LIB})

target_include_directories(${TASKS_PLUGIN} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include")

target_compile_features(${TASKS_PLUGIN} PRIVATE cxx_std_20)

target_link_libraries(${TASKS_PLUGIN} PRIVATE Qt6::Widgets)
target_link_libraries(${TASKS_PLUGIN} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${TASKS_PLUGIN} PRIVATE ${MV_PUBLIC_LIB})

# Use AVX if enabled and available
mv_check_and_set_AVX(${TASKS_PLUGIN} ${MV_USE_AVX})

set_target_properties(${TASKS_PLUGIN} PROPERTIES AUTOMOC ON)

install(TARGETS ${TASKS_PLUGIN}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
)

add_custom_command(TARGET ${TASKS_PLUGIN} POST_BUILD
	COMMAND "${CMAKE_COMMAND}"
	--install ${CMAKE_CURRENT_BINARY_DIR}
	--config $<CONFIGURATION>
	--prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
)

# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
    set_property(TARGET ${TASKS_PLUGIN} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/Debug,$<IF:$<CONFIG:RELWITHDEBINFO>,${MV_INSTALL_DIR}/RelWithDebInfo,${MV_INSTALL_DIR}/Release>>)
    set_property(TARGET ${TASKS_PLUGIN} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,"${MV_INSTALL_DIR}/Debug/${MV_APPLICATION_NAME}.exe",$<IF:$<CONFIG:RELWITHDEBINFO>,"${MV_INSTALL_DIR}/RelWithDebInfo/${MV_APPLICATION_NAME}.exe","${MV_INSTALL_DIR}/Release/${MV_APPLICATION_NAME}.exe">>)
endif()
