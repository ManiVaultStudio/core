cmake_minimum_required(VERSION 3.17)

set(DATAHIERARCHYPLUGIN "DataHierarchyPlugin")
PROJECT(${DATAHIERARCHYPLUGIN})

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

find_package(Qt6 6.3.1 COMPONENTS Widgets WebEngineWidgets REQUIRED)

set(PLUGIN
    src/DataHierarchyPlugin.h
    src/DataHierarchyPlugin.cpp
)

set(PLUGIN_MOC_HEADERS
    src/DataHierarchyPlugin.h
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
    ${WIDGETS}
    ${AUX}
)

source_group(Plugin FILES ${PLUGIN})
source_group(Widgets FILES ${WIDGETS})
source_group(Aux FILES ${AUX})

add_library(${DATAHIERARCHYPLUGIN} SHARED ${DATAHIERARCHY_SOURCES})

qt_wrap_cpp(DATAHIERARCHY_MOC ${PLUGIN_MOC_HEADERS} TARGET ${DATAHIERARCHYPLUGIN})
target_sources(${DATAHIERARCHYPLUGIN} PRIVATE ${DATAHIERARCHY_MOC})

target_include_directories(${DATAHIERARCHYPLUGIN} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${DATAHIERARCHYPLUGIN} PRIVATE cxx_std_17)

target_link_libraries(${DATAHIERARCHYPLUGIN} PRIVATE Qt6::Widgets)
target_link_libraries(${DATAHIERARCHYPLUGIN} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${DATAHIERARCHYPLUGIN} PRIVATE ${MV_PUBLIC_LIB})

# Use avx if enabled and available
check_and_set_AVX(${DATAHIERARCHYPLUGIN} ${MV_USE_AVX})

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
