cmake_minimum_required(VERSION 3.1.0)

set(PROJECT "HDPS")
project(${PROJECT})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(MACOSX_BUNDLE_BUNDLE_NAME "HDPS")
set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")
set(MACOSX_BUNDLE_LONG_VERSION_STRING "1.0")
set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
set(MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2017. All rights reserved.")
set(MACOSX_BUNDLE_GUI_IDENTIFIER "nl.tudelft.graphics.hdps")
set(MACOSX_BUNDLE_ICON_FILE AppIcon.icns)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /EHsc /MP")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG /NODEFAULTLIB:LIBCMT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebKitWidgets REQUIRED)
find_package(OpenGL REQUIRED)

set(INSTALL_DIR $ENV{HDPS_INSTALL_DIR})
#message(STATUS ${INSTALL_DIR})

set( MAIN_SOURCES
    src/Main.cpp
)

set( CORE_HEADERS
    src/MainWindow.h
    src/PluginManager.h
    src/Core.h
    src/DataManager.h
)

set( CORE_SOURCES
    src/MainWindow.cpp
    src/PluginManager.cpp
    src/Core.cpp
    src/DataManager.cpp
)


set( WIDGET_HEADERS
    src/widgets/PointRenderer.h
    src/widgets/DensityRenderer.h
    src/widgets/DockableWidget.h
    src/widgets/ScatterplotWidget.h
    src/widgets/DensityPlotWidget.h
    src/widgets/ImageWidget.h
    src/widgets/SettingsWidget.h
    src/widgets/WebWidget.h
)

set( WIDGET_SOURCES
    src/widgets/PointRenderer.cpp
    src/widgets/DensityRenderer.cpp
    src/widgets/DockableWidget.cpp
    src/widgets/ScatterplotWidget.cpp
    src/widgets/DensityPlotWidget.cpp
    src/widgets/ImageWidget.cpp
    src/widgets/WebWidget.cpp
)

set( GRAPHICS_HEADERS
    src/graphics/BufferObject.h
    src/graphics/Vector2f.h
    src/graphics/Vector3f.h
    src/graphics/Matrix3f.h
    src/graphics/Selection.h
    src/graphics/Texture.h
    src/graphics/Shader.h
    src/graphics/Framebuffer.h
)

set( GRAPHICS_SOURCES
    src/graphics/BufferObject.cpp
    src/graphics/Shader.cpp
)

set( UTIL_HEADERS
    src/util/FileUtil.h
    src/util/MeanShift.h
    src/util/DensityComputation.h
)

set( UTIL_SOURCES
    src/util/FileUtil.cpp
    src/util/MeanShift.cpp
    src/util/DensityComputation.cpp
)

set( PLUGIN_HEADERS
    src/CoreInterface.h
    src/AnalysisPlugin.h
    src/DataTypePlugin.h
    src/LoaderPlugin.h
    src/WriterPlugin.h
    src/TransformationPlugin.h
    src/ViewPlugin.h
    src/PluginFactory.h
    src/Plugin.h
    src/PluginType.h
    src/DataConsumer.h
    src/SelectionListener.h
    src/Set.h
)

list(APPEND PRIVATE_HEADERS
    ${CORE_HEADERS}
)

list(APPEND PRIVATE_SOURCES
    ${CORE_SOURCES}
    ${PRIVATE_HEADERS}
)

list(APPEND PUBLIC_HEADERS
    ${GRAPHICS_HEADERS}
    ${WIDGET_HEADERS}
    ${UTIL_HEADERS}
    ${PLUGIN_HEADERS}
)

list(APPEND PUBLIC_SOURCES
    ${GRAPHICS_SOURCES}
    ${WIDGET_SOURCES}
    ${UTIL_SOURCES}
    ${PUBLIC_HEADERS}
)
list(APPEND GRAPHICS_FILES
    ${GRAPHICS_HEADERS}
    ${GRAPHICS_SOURCES}
)
list(APPEND WIDGET_FILES
    ${WIDGET_SOURCES}
    ${WIDGET_HEADERS}
)
list(APPEND UTIL_FILES
    ${UTIL_SOURCES}
    ${UTIL_HEADERS}
)

if(WIN32)
    set(MAIN_SOURCES ${MAIN_SOURCES} win/hdps.rc)
endif(WIN32)

if(APPLE)
    set(MAIN_SOURCES ${MAIN_SOURCES} res/icons/AppIcon.icns)
    set_source_files_properties(res/icons/AppIcon.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif(APPLE)

set(UI_FILES
    ui/MainWindow.ui
)

QT5_WRAP_UI(UI_HEADERS ${UI_FILES})
QT5_ADD_RESOURCES(RESOURCE_FILES res/resources.qrc)

source_group( GUI FILES ${CORE_SOURCES})
source_group( Private FILES ${PRIVATE_SOURCES})
source_group( Public FILES ${PLUGIN_HEADERS})
source_group( Public\\Widgets FILES ${WIDGET_FILES})
source_group( Public\\Util FILES ${UTIL_FILES})
source_group( Public\\Graphics FILES ${GRAPHICS_FILES})
source_group( Resources FILES res/resources.qrc)
source_group( Resources FILES ${UI_FILES})
source_group( Resources FILES win/hdps.rc)
source_group( Resources\\generated FILES ${UI_HEADERS})

if(APPLE)
    set(LINK_TYPE SHARED)
endif(APPLE)
if(MSVC)
    set(LINK_TYPE STATIC)
endif(MSVC)


add_library(${PROJECT}_Public ${LINK_TYPE}
    ${PUBLIC_SOURCES}
)
target_link_libraries(${PROJECT}_Public Qt5::Widgets)
target_link_libraries(${PROJECT}_Public Qt5::WebKitWidgets)

add_library(${PROJECT}_Private ${LINK_TYPE}
    ${PRIVATE_SOURCES}
)
target_link_libraries(${PROJECT}_Private Qt5::Widgets)
target_link_libraries(${PROJECT}_Private Qt5::WebKitWidgets)
target_link_libraries(${PROJECT}_Private "$<TARGET_FILE:${PROJECT}_Public>")

add_dependencies(${PROJECT}_Private ${PROJECT}_Public)

add_executable(${PROJECT} MACOSX_BUNDLE
    ${MAIN_SOURCES}
    ${UI_HEADERS}
    ${UI_FILES}
    ${RESOURCE_FILES}
)

target_link_libraries(${PROJECT} Qt5::Widgets)
target_link_libraries(${PROJECT} Qt5::WebKitWidgets)

target_link_libraries(${PROJECT} "$<TARGET_FILE:${PROJECT}_Public>")
target_link_libraries(${PROJECT} "$<TARGET_FILE:${PROJECT}_Private>")

add_dependencies(${PROJECT} ${PROJECT}_Public)
add_dependencies(${PROJECT} ${PROJECT}_Private)

if(MSVC)
    set_target_properties(${PROJECT} PROPERTIES
        WIN32_EXECUTABLE YES
        LINK_FLAGS "/ENTRY:mainCRTStartup"
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif(MSVC)

#copy after build
# library
add_custom_command(TARGET ${PROJECT}_Public POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy
	"$<TARGET_FILE:${PROJECT}_Public>"
	"${INSTALL_DIR}/$<CONFIGURATION>/lib/$<TARGET_FILE_NAME:${PROJECT}_Public>"
)

#headers
foreach(PUBLIC_HEADER ${PUBLIC_HEADERS})
	STRING(REGEX REPLACE "^src/" "" STRIPPED_PUBLIC_HEADER ${PUBLIC_HEADER})
	add_custom_command(TARGET ${PROJECT}_Public POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
		"${CMAKE_CURRENT_SOURCE_DIR}/${PUBLIC_HEADER}"
		"${INSTALL_DIR}/$<CONFIGURATION>/include/${STRIPPED_PUBLIC_HEADER}"
	)
endforeach(PUBLIC_HEADER)

#executable
add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy
	"$<TARGET_FILE:${PROJECT}>"
	"${INSTALL_DIR}/$<CONFIGURATION>/$<TARGET_FILE_NAME:${PROJECT}>"
)

# Copy pointsplugin header
add_custom_command(TARGET ${PROJECT}_Public POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/PointsPlugin.h"
    "${INSTALL_DIR}/$<CONFIGURATION>/include/PointsPlugin.h"
)

# Points Plugin
set(PROJECT "PointsPlugin")
#PROJECT(${PROJECT})


set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /EHsc /MP")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG /NODEFAULTLIB:LIBCMT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebKitWidgets REQUIRED)

set(POINTS_SOURCES
    src/plugins/PointsPlugin.h
    src/plugins/PointsPlugin.cpp
    src/plugins/PointsPlugin.json
)

source_group( Plugin FILES ${POINTS_SOURCES})

include_directories("$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/include/")

add_library(${PROJECT} SHARED ${POINTS_SOURCES})

target_link_libraries(${PROJECT} Qt5::Widgets)
target_link_libraries(${PROJECT} Qt5::WebKitWidgets)
target_link_libraries(${PROJECT} "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/lib/HDPS_Public.lib")


add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy
	"$<TARGET_FILE:${PROJECT}>"
	"$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/Plugins/$<TARGET_FILE_NAME:${PROJECT}>"
)
