cmake_minimum_required(VERSION 3.17)

set(DATAHIERARCHYPLUGIN "DataHierarchyPlugin")
PROJECT(${DATAHIERARCHYPLUGIN})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

# Get the env path and allow the user to change it
set(INSTALL_DIR $ENV{HDPS_INSTALL_DIR} CACHE PATH "The HDPS Install Directoy")
# Normalize the incoming install path
file(TO_CMAKE_PATH ${INSTALL_DIR} INSTALL_DIR)

find_package(Qt6 6.3.1 COMPONENTS Widgets WebEngineWidgets REQUIRED)

set(PLUGIN
	src/DataHierarchyPlugin.h
    src/DataHierarchyPlugin.cpp
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

add_dependencies(${DATAHIERARCHYPLUGIN} HDPS_Public)

target_include_directories(${DATAHIERARCHYPLUGIN} PRIVATE "${INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${DATAHIERARCHYPLUGIN} PRIVATE cxx_std_17)

set(HDPS_LINK_PATH "${INSTALL_DIR}/$<CONFIGURATION>/lib")
set(HDPS_LINK_SUFFIX $<IF:$<CXX_COMPILER_ID:MSVC>,${CMAKE_LINK_LIBRARY_SUFFIX},${CMAKE_SHARED_LIBRARY_SUFFIX}>)

set(HDPS_LINK_LIBRARY "${HDPS_LINK_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}HDPS_Public${HDPS_LINK_SUFFIX}")

target_link_libraries(${DATAHIERARCHYPLUGIN} PRIVATE Qt6::Widgets)
target_link_libraries(${DATAHIERARCHYPLUGIN} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${DATAHIERARCHYPLUGIN} PRIVATE "${HDPS_LINK_LIBRARY}")

# Use avx if enabled and available
check_and_link_AVX(${DATAHIERARCHYPLUGIN} ${USE_AVX})

install(TARGETS ${DATAHIERARCHYPLUGIN}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
)

add_custom_command(TARGET ${DATAHIERARCHYPLUGIN} POST_BUILD
	COMMAND "${CMAKE_COMMAND}"
	--install ${CMAKE_CURRENT_BINARY_DIR}
	--config $<CONFIGURATION>
	--prefix ${INSTALL_DIR}/$<CONFIGURATION>
)

# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
    set_property(TARGET ${DATAHIERARCHYPLUGIN} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug,${INSTALL_DIR}/release>)
    set_property(TARGET ${DATAHIERARCHYPLUGIN} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug/HDPS.exe,${INSTALL_DIR}/release/HDPS.exe>)
endif()
