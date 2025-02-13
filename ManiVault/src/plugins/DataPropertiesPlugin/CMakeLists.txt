cmake_minimum_required(VERSION 3.17)

set(DATAPROPERTIESPLUGIN "DataPropertiesPlugin")
PROJECT(${DATAPROPERTIESPLUGIN})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

# Check if the directory to the ManiVault installation has been provided
if(NOT DEFINED MV_INSTALL_DIR)
    set(MV_INSTALL_DIR "" CACHE PATH "Directory where ManiVault is installed")
    message(FATAL_ERROR "Please set MV_INSTALL_DIR to the directory where ManiVault is installed")
endif()
file(TO_CMAKE_PATH ${MV_INSTALL_DIR} MV_INSTALL_DIR)

find_package(Qt6 COMPONENTS Widgets WebEngineWidgets REQUIRED)

set(PLUGIN
    src/DataPropertiesPlugin.h
    src/DataPropertiesPlugin.cpp
)

set(PLUGIN_MOC_HEADERS
	src/DataPropertiesPlugin.h
)

set(WIDGETS
    src/DataPropertiesWidget.h
    src/DataPropertiesWidget.cpp
)

set(AUX
    src/DataPropertiesPlugin.json
)

set(DATAPROPERTIES_SOURCES
    ${PLUGIN}
    ${WIDGETS}
    ${AUX}
)

source_group(Plugin FILES ${PLUGIN})
source_group(Widgets FILES ${WIDGETS})
source_group(Aux FILES ${AUX})
source_group(Resources FILES ${QRESOURCES})

add_library(${DATAPROPERTIESPLUGIN} SHARED ${DATAPROPERTIES_SOURCES})

qt_wrap_cpp(DATAPROPERTIES_MOC ${PLUGIN_MOC_HEADERS} TARGET ${DATAPROPERTIESPLUGIN})
target_sources(${DATAPROPERTIESPLUGIN} PRIVATE ${DATAPROPERTIES_MOC})

target_include_directories(${DATAPROPERTIESPLUGIN} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${DATAPROPERTIESPLUGIN} PRIVATE cxx_std_20)

target_link_libraries(${DATAPROPERTIESPLUGIN} PRIVATE Qt6::Core)
target_link_libraries(${DATAPROPERTIESPLUGIN} PRIVATE Qt6::Widgets)
target_link_libraries(${DATAPROPERTIESPLUGIN} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${DATAPROPERTIESPLUGIN} PRIVATE ${MV_PUBLIC_LIB})

# Use AVX if enabled and available
mv_check_and_set_AVX(${DATAPROPERTIESPLUGIN} ${MV_USE_AVX})

if(MV_UNITY_BUILD)
    set_target_properties(${DATAPROPERTIESPLUGIN} PROPERTIES UNITY_BUILD ON)
    set_source_files_properties(${DATAPROPERTIES_MOC} PROPERTIES
                        SKIP_UNITY_BUILD_INCLUSION TRUE)
endif()

install(TARGETS ${DATAPROPERTIESPLUGIN}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
)

add_custom_command(TARGET ${DATAPROPERTIESPLUGIN} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    --install ${CMAKE_CURRENT_BINARY_DIR}
    --config $<CONFIGURATION>
    --prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
)

# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
    set_property(TARGET ${DATAPROPERTIESPLUGIN} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/debug,${MV_INSTALL_DIR}/release>)
    set_property(TARGET ${DATAPROPERTIESPLUGIN} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/debug/${MV_APPLICATION_NAME}.exe,${MV_INSTALL_DIR}/release/${MV_APPLICATION_NAME}.exe>)
endif()
