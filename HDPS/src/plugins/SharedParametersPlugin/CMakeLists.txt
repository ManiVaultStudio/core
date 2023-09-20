cmake_minimum_required(VERSION 3.17)

set(SHARED_PARAMETERS_PLUGIN "SharedParametersPlugin")
PROJECT(${SHARED_PARAMETERS_PLUGIN})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

# Get the env path and allow the user to change it
set(INSTALL_DIR $ENV{MV_INSTALL_DIR} CACHE PATH "The ManiVault Install Directoy")

# Normalize the incoming install path
file(TO_CMAKE_PATH ${INSTALL_DIR} INSTALL_DIR)

find_package(Qt6 6.3.1 COMPONENTS Widgets WebEngineWidgets REQUIRED)

set(PLUGIN
    src/SharedParametersPlugin.h
    src/SharedParametersPlugin.cpp
)

set(PLUGIN_MOC_HEADERS
	src/SharedParametersPlugin.h
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

add_dependencies(${SHARED_PARAMETERS_PLUGIN} HDPS_Public)

qt_wrap_cpp(SHARED_PARAMETERS_MOC ${PLUGIN_MOC_HEADERS} TARGET ${SHARED_PARAMETERS_PLUGIN})
target_sources(${SHARED_PARAMETERS_PLUGIN} PRIVATE ${SHARED_PARAMETERS_MOC})

target_include_directories(${SHARED_PARAMETERS_PLUGIN} PRIVATE "${INSTALL_DIR}/$<CONFIGURATION>/include")

target_compile_features(${SHARED_PARAMETERS_PLUGIN} PRIVATE cxx_std_17)

target_link_libraries(${SHARED_PARAMETERS_PLUGIN} PRIVATE Qt6::Widgets)
target_link_libraries(${SHARED_PARAMETERS_PLUGIN} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${SHARED_PARAMETERS_PLUGIN} PRIVATE HDPS_Public)

# Use avx if enabled and available
check_and_set_AVX(${SHARED_PARAMETERS_PLUGIN} ${MV_USE_AVX})

install(TARGETS ${SHARED_PARAMETERS_PLUGIN}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
)

add_custom_command(TARGET ${SHARED_PARAMETERS_PLUGIN} POST_BUILD
	COMMAND "${CMAKE_COMMAND}"
	--install ${CMAKE_CURRENT_BINARY_DIR}
	--config $<CONFIGURATION>
	--prefix ${INSTALL_DIR}/$<CONFIGURATION>
)

# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
    set_property(TARGET ${SHARED_PARAMETERS_PLUGIN} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug,${INSTALL_DIR}/release>)
    set_property(TARGET ${SHARED_PARAMETERS_PLUGIN} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug/${MV_EXE}.exe,${INSTALL_DIR}/release/${MV_EXE}.exe>)
endif()
