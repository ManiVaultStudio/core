cmake_minimum_required(VERSION 3.1.0)

set(PROJECT "HeatMapPlugin")

PROJECT(${PROJECT})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /EHsc /MP")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

find_package(Qt5 COMPONENTS Widgets WebEngineWidgets REQUIRED)

qt5_add_resources(RESOURCE_FILES res/heatmap_resources.qrc)

set(SOURCES
    src/HeatMapPlugin.h
    src/HeatMapPlugin.cpp
    src/HeatMapWidget.h
    src/HeatMapWidget.cpp
    src/HeatMapPlugin.json
)

set (RESOURCES
    res/heatmap_resources.qrc
)

source_group( Plugin FILES ${SOURCES})
source_group( Resources FILES ${RESOURCES})
source_group( Resources\\generated FILES ${CMAKE_CURRENT_BINARY_DIR}/qrc_heatmap_resources.cpp)

include_directories("$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/include/")

add_library(${PROJECT} SHARED ${SOURCES} ${RESOURCES} ${RESOURCE_FILES})

qt5_use_modules(${PROJECT} Widgets WebEngineWidgets)

target_link_libraries(${PROJECT} Qt5::Widgets)
target_link_libraries(${PROJECT} Qt5::WebEngineWidgets)
target_link_libraries(${PROJECT} "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/lib/HDPS_Public.lib")

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
	"$<TARGET_FILE:${PROJECT}>"
	"$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/Plugins/$<TARGET_FILE_NAME:${PROJECT}>"
)