cmake_minimum_required(VERSION 3.1.0)

set(PROJECT "HDPS")
project(${PROJECT})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

set(MACOSX_BUNDLE_BUNDLE_NAME "HDPS")
set(MACOSX_BUNDLE_BUNDLE_VERSION "0.1.0")
set(MACOSX_BUNDLE_LONG_VERSION_STRING "0.1.0")
set(MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1.0")
set(MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2018. All rights reserved.")
set(MACOSX_BUNDLE_GUI_IDENTIFIER "nl.tudelft.graphics.hdps")
set(MACOSX_BUNDLE_ICON_FILE AppIcon.icns)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /EHsc /MP")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

add_definitions(-DQT_MESSAGELOGCONTEXT)

find_package(Qt5 COMPONENTS Widgets WebEngineWidgets OpenGL REQUIRED)

set(INSTALL_DIR $ENV{HDPS_INSTALL_DIR})
#message(STATUS ${INSTALL_DIR})

set( MAIN_SOURCES
    src/Main.cpp
)

set( CORE_HEADERS
    ui/MainWindow.ui
    src/Logger.h
    src/LogItemModel.h
    src/LogDockWidget.h
    src/DataHierarchyModel.h
    src/DataHierarchy.h
    src/MainWindow.h
    src/CentralWidget.h
    src/PluginManager.h
    src/Core.h
    src/DataManager.h
    src/exceptions/SetNotFoundException.h
)

set( CORE_SOURCES
    src/Logger.cpp
    src/LogItemModel.cpp
    src/LogDockWidget.cpp
    src/DataHierarchyModel.cpp
    src/DataHierarchy.cpp
    src/MainWindow.cpp
    src/CentralWidget.cpp
    src/PluginManager.cpp
    src/Core.cpp
    src/DataManager.cpp
)

set( WIDGET_HEADERS
    src/widgets/DockableWidget.h
    src/widgets/SettingsWidget.h
    src/widgets/WebWidget.h
    src/widgets/ColormapWidget.h
    src/widgets/MaterialCheckBoxWidget.h
    src/widgets/ComboBox.h
    src/widgets/PushButton.h
    src/widgets/HorizontalDivider.h
    src/widgets/DataSlot.h
)

set( WIDGET_SOURCES
    src/widgets/DockableWidget.cpp
    src/widgets/WebWidget.cpp
    src/widgets/ColormapWidget.cpp
    src/widgets/MaterialCheckBoxWidget.cpp
    src/widgets/ComboBox.cpp
    src/widgets/PushButton.cpp
    src/widgets/HorizontalDivider.cpp
    src/widgets/DataSlot.cpp
)

set( RENDERER_HEADERS
    src/renderers/Renderer.h
    src/renderers/PointRenderer.h
    src/renderers/DensityRenderer.h
    src/renderers/SelectionRenderer.h
    src/renderers/ImageRenderer.h
)

set( RENDERER_SOURCES
    src/renderers/PointRenderer.cpp
    src/renderers/DensityRenderer.cpp
    src/renderers/SelectionRenderer.cpp
    src/renderers/ImageRenderer.cpp
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
    src/graphics/OffscreenBuffer.h
    src/graphics/Bounds.h
)

set( GRAPHICS_SOURCES
    src/graphics/BufferObject.cpp
    src/graphics/Shader.cpp
    src/graphics/Selection.cpp
    src/graphics/Matrix3f.cpp
    src/graphics/Vector2f.cpp
    src/graphics/Vector3f.cpp
    src/graphics/Bounds.cpp
)

set( UTIL_HEADERS
    src/util/FileUtil.h
    src/util/MeanShift.h
    src/util/DensityComputation.h
    src/util/Math.h
)

set( UTIL_SOURCES
    src/util/FileUtil.cpp
    src/util/MeanShift.cpp
    src/util/DensityComputation.cpp
    src/util/Math.cpp
)

set( PLUGIN_HEADERS
    src/CoreInterface.h
    src/AnalysisPlugin.h
    src/RawData.h
    src/LoaderPlugin.h
    src/WriterPlugin.h
    src/ViewPlugin.h
    src/PluginFactory.h
    src/Plugin.h
    src/PluginType.h
    src/DataConsumer.h
    src/SelectionListener.h
    src/Set.h
    src/IndexSet.h
)

set( PLUGIN_SOURCES
    src/IndexSet.cpp
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
    ${RENDERER_HEADERS}
    ${UTIL_HEADERS}
    ${PLUGIN_HEADERS}
)

list(APPEND PUBLIC_SOURCES
    ${GRAPHICS_SOURCES}
    ${WIDGET_SOURCES}
    ${RENDERER_SOURCES}
    ${UTIL_SOURCES}
    ${PLUGIN_SOURCES}
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
list(APPEND RENDERER_FILES
    ${RENDERER_HEADERS}
    ${RENDERER_SOURCES}
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
source_group( Public\\Renderers FILES ${RENDERER_FILES})
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
    src/.editorconfig
    ${PUBLIC_SOURCES}
)
target_link_libraries(${PROJECT}_Public Qt5::Widgets)
target_link_libraries(${PROJECT}_Public Qt5::WebEngineWidgets)

add_library(${PROJECT}_Private ${LINK_TYPE}
    src/.editorconfig
    ${PRIVATE_SOURCES}
)
target_link_libraries(${PROJECT}_Private Qt5::Widgets)
target_link_libraries(${PROJECT}_Private Qt5::WebEngineWidgets)
target_link_libraries(${PROJECT}_Private "$<TARGET_FILE:${PROJECT}_Public>")

add_dependencies(${PROJECT}_Private ${PROJECT}_Public)

if (APPLE)
    set( FRAMEWORKS_FILES ${FRAMEWORKS_FILES}
        "$<TARGET_FILE:${PROJECT}_Public>"
        "$<TARGET_FILE:${PROJECT}_Private>"
    )
    set_source_files_properties(${FRAMEWORKS_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Frameworks)
endif (APPLE)

add_executable(${PROJECT} MACOSX_BUNDLE
    src/.editorconfig
    ${MAIN_SOURCES}
    ${UI_HEADERS}
    ${UI_FILES}
    ${RESOURCE_FILES}
    #${FRAMEWORKS_FILES}
)

target_link_libraries(${PROJECT} Qt5::Widgets)
target_link_libraries(${PROJECT} Qt5::WebEngineWidgets)

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

#headers
foreach(PUBLIC_HEADER ${PUBLIC_HEADERS})
    STRING(REGEX REPLACE "^src/" "" STRIPPED_PUBLIC_HEADER ${PUBLIC_HEADER})
    add_custom_command(TARGET ${PROJECT}_Public POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/${PUBLIC_HEADER}"
        "${INSTALL_DIR}/$<CONFIGURATION>/include/${STRIPPED_PUBLIC_HEADER}"
    )
endforeach(PUBLIC_HEADER)

if(APPLE)
    # Retrieve the absolute path to qmake and then use that path to find qts macdeply binaries
    get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
    
    add_custom_command(TARGET ${PROJECT} POST_BUILD
        #use macdeply qt to add libraries to app bundle
        COMMAND "${MACDEPLOYQT_EXECUTABLE}"
            "$<TARGET_FILE_DIR:${PROJECT}>/../.."
            -always-overwrite
            -no-plugins
        COMMENT "Running macdeployqt..."
        #copy app bundle (this includes the libraries such as HDPS_Public in ${INSTALL_DIR}/$<CONFIGURATION>/${PROJECT}.app/Contents/Frameworks)
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>/${PROJECT}.app"
        "${INSTALL_DIR}/$<CONFIGURATION>/${PROJECT}.app")
else(APPLE)
    # library
    add_custom_command(TARGET ${PROJECT}_Public POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "$<TARGET_FILE:${PROJECT}_Public>"
        "${INSTALL_DIR}/$<CONFIGURATION>/lib/$<TARGET_FILE_NAME:${PROJECT}_Public>"
    )

    #executable
    add_custom_command(TARGET ${PROJECT} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "$<TARGET_FILE:${PROJECT}>"
        "${INSTALL_DIR}/$<CONFIGURATION>/$<TARGET_FILE_NAME:${PROJECT}>"
    )
endif(APPLE)

# Point Data
add_subdirectory(src/plugins/PointData)

# Color Data
add_subdirectory(src/plugins/ColorData)

# Text Data
add_subdirectory(src/plugins/TextData)

# Cluster Data
add_subdirectory(src/plugins/ClusterData)
