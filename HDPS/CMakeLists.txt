cmake_minimum_required(VERSION 3.17)

set(PROJECT "HDPS")
project(${PROJECT})

if(NOT DEFINED ENV{HDPS_INSTALL_DIR})
    message(FATAL_ERROR, "$ENV{HDPS_INSTALL_DIR}
    ***************************************************
    Supply an environment variable HDPS_INSTALL_DIR 
    to indicate where the HDPS output will be installed,
    make sure the directory exists.
    ***************************************************" )
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC_PATH_PREFIX ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

if(APPLE)
    # These are the properties for the Info.plist in the mac bindle
    set(MACOSX_BUNDLE_BUNDLE_NAME "HDPS")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "1.0")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2021. All rights reserved.")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.biovault.hdps")
    set(MACOSX_BUNDLE_ICON_FILE AppIcon.icns)
    
    # use, i.e. don't skip the full RPATH for the build tree
    set(CMAKE_SKIP_BUILD_RPATH  FALSE)

    # when building, don't use the install RPATH already
    # (but later on when installing)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

    set(CMAKE_INSTALL_RPATH "@loader_path")

    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /DWIN32 /EHsc /MP")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

add_definitions(-DQT_MESSAGELOGCONTEXT)

if (WIN32)
    find_package(Qt5 5.15 COMPONENTS Widgets WebEngineWidgets OpenGL ZLib REQUIRED)
elseif (APPLE)
    find_package(Qt5 5.15 COMPONENTS Widgets WebEngineWidgets OpenGL REQUIRED)
endif()

# Normalize the incoming install path
file(TO_CMAKE_PATH $ENV{HDPS_INSTALL_DIR} INSTALL_DIR)
#message(STATUS ${INSTALL_DIR})

set( MAIN_SOURCES
    src/Main.cpp
    res/styles/default.qss
)

set( CORE_HEADERS
    ui/MainWindow.ui
    src/Logger.h
    src/LogItemModel.h
    src/LogDockWidget.h
    src/MainWindow.h
    src/PluginManager.h
    src/DataHierarchyManager.h
    src/Core.h
    src/DataManager.h
    src/Archiver.h
    src/HdpsApplication.h
    src/StartPageWidget.h
    src/ProjectBarWidget.h
    src/LogoWidget.h
)

set( CORE_SOURCES
    src/Logger.cpp
    src/LogItemModel.cpp
    src/LogDockWidget.cpp
    src/MainWindow.cpp
    src/PluginManager.cpp
    src/DataHierarchyManager.cpp
    src/Core.cpp
    src/DataManager.cpp
    src/Archiver.cpp
    src/HdpsApplication.cpp
    src/StartPageWidget.cpp
    src/ProjectBarWidget.cpp
    src/LogoWidget.cpp
)

set(EVENT_HEADERS
    src/event/Event.h
    src/event/EventListener.h
)

set(EVENT_SOURCES
    src/event/Event.cpp
    src/event/EventListener.cpp
)

set( WIDGET_HEADERS
    src/widgets/CollapsedWidget.h
    src/widgets/WebWidget.h
    src/widgets/DockableWidget.h
    src/widgets/DropWidget.h
    src/widgets/ResponsiveToolbarWidget.h
    src/widgets/ToolbarItemWidget.h
    src/widgets/Divider.h
)

set( WIDGET_SOURCES
    src/widgets/CollapsedWidget.cpp
    src/widgets/WebWidget.cpp
    src/widgets/DockableWidget.cpp
    src/widgets/DropWidget.cpp
    src/widgets/ResponsiveToolbarWidget.cpp
    src/widgets/ToolbarItemWidget.cpp
    src/widgets/Divider.cpp
)

set(ACTIONS_HEADERS
    src/actions/Actions.h
    src/actions/ColorAction.h
    src/actions/ColorPickerAction.h
    src/actions/ColorMapAction.h
    src/actions/ColorMapEditor1DAction.h
    src/actions/ColorMapEditor1DEdgesGraphicsItem.h
    src/actions/ColorMapEditor1DHistogramGraphicsItem.h
    src/actions/ColorMapEditor1DNode.h
    src/actions/ColorMapEditor1DNodeGraphicsItem.h
    src/actions/ColorMapEditor1DNodeAction.h
    src/actions/ColorMapEditor1DScene.h
    src/actions/ColorMapEditor1DWidget.h
    src/actions/ColorMapAxisAction.h
    src/actions/ColorMapDiscreteAction.h
    src/actions/ColorMapViewAction.h
    src/actions/ColorMapSettingsAction.h
    src/actions/ColorMapSettings1DAction.h
    src/actions/ColorMapSettings2DAction.h
    src/actions/DatasetPickerAction.h
    src/actions/DecimalAction.h
    src/actions/DecimalRangeAction.h
    src/actions/GroupAction.h
    src/actions/GroupsAction.h
    src/actions/ImageAction.h
    src/actions/IntegralAction.h
    src/actions/RectangleAction.h
    src/actions/IntegralRectangleAction.h
    src/actions/DecimalRectangleAction.h
    src/actions/NumericalAction.h
    src/actions/OptionAction.h
    src/actions/OptionsAction.h
    src/actions/PixelSelectionAction.h
    src/actions/TriggerAction.h
    src/actions/TriggersAction.h
    src/actions/ToggleAction.h
    src/actions/StringAction.h
    src/actions/WidgetAction.h
    src/actions/WidgetActionWidget.h
    src/actions/WidgetActionCollapsedWidget.h
    src/actions/WidgetActionLabel.h
    src/actions/WidgetActionStateWidget.h
    src/actions/WindowLevelAction.h
    src/actions/DataAction.h
    src/actions/DataImportAction.h
    src/actions/DataExportAction.h
    src/actions/DataCopyAction.h
    src/actions/DataRemoveAction.h
    src/actions/DataViewAction.h
    src/actions/DataAnalysisAction.h
    src/actions/DataGroupingAction.h
    src/actions/DataTransformationAction.h
    src/actions/DirectoryPickerAction.h
    src/actions/FilePickerAction.h
    src/actions/StatusAction.h
    src/actions/GroupSectionTreeItem.h
    src/actions/GroupWidgetTreeItem.h
)

set(ACTIONS_SOURCES
    src/actions/ColorAction.cpp
    src/actions/ColorPickerAction.cpp
    src/actions/ColorMapAction.cpp
    src/actions/ColorMapEditor1DAction.cpp
	src/actions/ColorMapEditor1DEdgesGraphicsItem.cpp
	src/actions/ColorMapEditor1DHistogramGraphicsItem.cpp
    src/actions/ColorMapEditor1DNode.cpp
    src/actions/ColorMapEditor1DNodeGraphicsItem.cpp
    src/actions/ColorMapEditor1DNodeAction.cpp
    src/actions/ColorMapEditor1DScene.cpp
    src/actions/ColorMapEditor1DWidget.cpp
    src/actions/ColorMapAxisAction.cpp
    src/actions/ColorMapDiscreteAction.cpp
    src/actions/ColorMapViewAction.cpp
    src/actions/ColorMapSettingsAction.cpp
	src/actions/ColorMapSettings1DAction.cpp
    src/actions/ColorMapSettings2DAction.cpp
	src/actions/DatasetPickerAction.cpp
    src/actions/DecimalAction.cpp
    src/actions/DecimalRangeAction.cpp
    src/actions/GroupAction.cpp
    src/actions/GroupsAction.cpp
    src/actions/ImageAction.cpp
    src/actions/IntegralAction.cpp
    src/actions/RectangleAction.cpp
    src/actions/IntegralRectangleAction.cpp
    src/actions/DecimalRectangleAction.cpp
    src/actions/NumericalAction.cpp
    src/actions/OptionAction.cpp
    src/actions/OptionsAction.cpp
    src/actions/PixelSelectionAction.cpp
    src/actions/TriggerAction.cpp
    src/actions/TriggersAction.cpp
    src/actions/ToggleAction.cpp
    src/actions/StringAction.cpp
    src/actions/WidgetAction.cpp
    src/actions/WidgetActionWidget.cpp
    src/actions/WidgetActionCollapsedWidget.cpp
    src/actions/WidgetActionLabel.cpp
    src/actions/WidgetActionStateWidget.cpp
    src/actions/WindowLevelAction.cpp
    src/actions/DataAction.cpp
    src/actions/DataImportAction.cpp
    src/actions/DataExportAction.cpp
    src/actions/DataCopyAction.cpp
    src/actions/DataRemoveAction.cpp
    src/actions/DataViewAction.cpp
    src/actions/DataAnalysisAction.cpp
    src/actions/DataGroupingAction.cpp
    src/actions/DataTransformationAction.cpp
    src/actions/DirectoryPickerAction.cpp
    src/actions/FilePickerAction.cpp
    src/actions/StatusAction.cpp
    src/actions/GroupSectionTreeItem.cpp
    src/actions/GroupWidgetTreeItem.cpp
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
    src/util/Miscellaneous.h
    src/util/FileUtil.h
    src/util/MeanShift.h
    src/util/DensityComputation.h
    src/util/Exception.h
    src/util/Math.h
    src/util/Timer.h
    src/util/IconFont.h
    src/util/IconFonts.h
    src/util/Interpolation.h
    src/util/FontAwesome.h
    src/util/ColorMap.h
    src/util/ColorMapFilterModel.h
    src/util/ColorMapModel.h
    src/util/ColorSpace.h
    src/util/PixelSelectionTool.h
    src/util/PixelSelection.h
    src/util/Preset.h
    src/util/PresetsModel.h
    src/util/PresetsFilterModel.h
    src/util/Serialization.h
)

set( UTIL_SOURCES
    src/util/Miscellaneous.cpp
    src/util/FileUtil.cpp
    src/util/MeanShift.cpp
    src/util/DensityComputation.cpp
    src/util/Exception.cpp
    src/util/Math.cpp
    src/util/Timer.cpp
    src/util/IconFont.cpp
    src/util/IconFonts.cpp
    src/util/Interpolation.cpp
    src/util/FontAwesome.cpp
    src/util/ColorMap.cpp
    src/util/ColorMapFilterModel.cpp
    src/util/ColorMapModel.cpp
    src/util/ColorSpace.cpp
    src/util/PixelSelectionTool.cpp
    src/util/PixelSelection.cpp
    src/util/Preset.cpp
    src/util/PresetsModel.cpp
    src/util/PresetsFilterModel.cpp
    src/util/Serialization.cpp
)

set( PLUGIN_HEADERS
    src/CoreInterface.h
    src/DataHierarchyItem.h
    src/AnalysisPlugin.h
    src/TransformationPlugin.h
    src/DataType.h
    src/RawData.h
    src/LoaderPlugin.h
    src/WriterPlugin.h
    src/ViewPlugin.h
    src/PluginFactory.h
    src/Plugin.h
    src/PluginType.h
    src/SelectionListener.h
    src/Set.h
    src/Application.h
    src/LinkedSelection.h
    src/Dataset.h
    src/DatasetPrivate.h
    src/ActionsModel.h
    src/ActionsFilterModel.h
    src/ActionsManager.h
)

set( PLUGIN_SOURCES
    src/DataHierarchyItem.cpp
    src/Plugin.cpp
    src/LoaderPlugin.cpp
    src/Application.cpp
    src/Plugin.cpp
    src/Set.cpp
    src/LinkedSelection.cpp
    src/Dataset.cpp
    src/DatasetPrivate.cpp
    src/ActionsModel.cpp
    src/ActionsFilterModel.cpp
    src/ActionsManager.cpp
)

set(DATA_HIERARCHY_HEADERS
    src/DataHierarchyModelItem.h
    src/DataHierarchyFilterModel.h
    src/DataHierarchyModel.h
    src/DataHierarchyWidget.h
    src/DataPropertiesWidget.h
    src/ActionsViewerWidget.h
)

set(DATA_HIERARCHY_SOURCES
    src/DataHierarchyModelItem.cpp
    src/DataHierarchyFilterModel.cpp
    src/DataHierarchyModel.cpp
    src/DataHierarchyWidget.cpp
    src/DataPropertiesWidget.cpp
    src/ActionsViewerWidget.cpp
)

list(APPEND PRIVATE_HEADERS
    ${CORE_HEADERS}
    ${DATA_HIERARCHY_HEADERS}
)

list(APPEND PRIVATE_SOURCES
    ${CORE_SOURCES}
    ${PRIVATE_HEADERS}
    ${DATA_HIERARCHY_SOURCES}
)

list(APPEND PUBLIC_HEADERS
    ${GRAPHICS_HEADERS}
    ${WIDGET_HEADERS}
    ${ACTIONS_HEADERS}
    ${RENDERER_HEADERS}
    ${UTIL_HEADERS}
    ${PLUGIN_HEADERS}
    ${EVENT_HEADERS}
)

list(APPEND PUBLIC_SOURCES
    ${GRAPHICS_SOURCES}
    ${WIDGET_SOURCES}
    ${ACTIONS_SOURCES}
    ${RENDERER_SOURCES}
    ${UTIL_SOURCES}
    ${PLUGIN_SOURCES}
    ${EVENT_SOURCES}
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

list(APPEND ACTIONS_FILES
    ${ACTIONS_SOURCES}
    ${ACTIONS_HEADERS}
)

list(APPEND RENDERER_FILES
    ${RENDERER_HEADERS}

    ${RENDERER_SOURCES}
)

list(APPEND UTIL_FILES
    ${UTIL_SOURCES}
    ${UTIL_HEADERS}
)

list(APPEND EVENT_FILES
    ${EVENT_HEADERS}
    ${EVENT_SOURCES}
)

list(APPEND HIERARCHY_FILES
    ${DATA_HIERARCHY_SOURCES}
    ${DATA_HIERARCHY_HEADERS}
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
source_group(Public\\Actions FILES ${ACTIONS_FILES})
source_group( Public\\Renderers FILES ${RENDERER_FILES})
source_group( Public\\Util FILES ${UTIL_FILES})
source_group( Public\\Graphics FILES ${GRAPHICS_FILES})
source_group( Public\\Event FILES ${EVENT_FILES})
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

set_property(TARGET ${PROJECT}_Public PROPERTY CXX_STANDARD 14)

target_include_directories(${PROJECT}_Public PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT}_Public PRIVATE ${INSTALL_DIR}/$<CONFIGURATION>/include/zlib)

target_link_libraries(${PROJECT}_Public Qt5::Widgets)
target_link_libraries(${PROJECT}_Public Qt5::WebEngineWidgets)

add_library(${PROJECT}_Private ${LINK_TYPE}
    src/.editorconfig
    ${PRIVATE_SOURCES}
	${RESOURCE_FILES}
)

set_property(TARGET ${PROJECT}_Private PROPERTY CXX_STANDARD 14)

target_link_libraries(${PROJECT}_Private Qt5::Widgets)
target_link_libraries(${PROJECT}_Private Qt5::WebEngineWidgets)
target_link_libraries(${PROJECT}_Private qtadvanceddocking)
target_link_libraries(${PROJECT}_Private QuaZip)
target_link_libraries(${PROJECT}_Private "$<TARGET_FILE:${PROJECT}_Public>")

target_include_directories(${PROJECT}_Private PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external/advanced_docking/src)
target_include_directories(${PROJECT}_Private PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT}_Private PRIVATE ${INSTALL_DIR}/$<CONFIGURATION>/include/QuaZip-Qt5-1.2)
target_include_directories(${PROJECT}_Private PRIVATE ${INSTALL_DIR}/$<CONFIGURATION>/include/zlib)

add_dependencies(${PROJECT}_Private ${PROJECT}_Public)
add_dependencies(${PROJECT}_Private QuaZip)
#add_dependencies(${PROJECT}_Private QArchive)

add_executable(${PROJECT} MACOSX_BUNDLE WIN32
    src/.editorconfig
    ${MAIN_SOURCES}
    ${UI_HEADERS}
    ${UI_FILES}
    ${RESOURCE_FILES}
)

set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD 14)
# find libraries relative to the executable in the macOS .app tree
if(APPLE)
    set_target_properties( ${PROJECT} PROPERTIES BUILD_WITH_INSTALL_RPATH True)
endif()

target_include_directories(${PROJECT} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT} Qt5::Widgets)
target_link_libraries(${PROJECT} Qt5::WebEngineWidgets)
target_link_libraries(${PROJECT} qtadvanceddocking)
target_link_libraries(${PROJECT} QuaZip)

target_link_libraries(${PROJECT} "$<TARGET_FILE:${PROJECT}_Public>")
target_link_libraries(${PROJECT} "$<TARGET_FILE:${PROJECT}_Private>")

add_dependencies(${PROJECT} ${PROJECT}_Public)
add_dependencies(${PROJECT} ${PROJECT}_Private)
add_dependencies(${PROJECT} QuaZip)

if(MSVC)
    set_target_properties(${PROJECT} PROPERTIES
        WIN32_EXECUTABLE YES
        LINK_FLAGS "/ENTRY:mainCRTStartup"
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif(MSVC)

#executable & linklib

target_include_directories(${PROJECT}_Public PRIVATE "${INSTALL_DIR}/$<CONFIGURATION>/include/QArchive")

if(APPLE)
    set(BUNDLE_DIR "${INSTALL_DIR}/$<CONFIGURATION>/${MACOSX_BUNDLE_BUNDLE_NAME}.app")
    set(WEBENGINEINSTALL_CMD "${CMAKE_CURRENT_SOURCE_DIR}/macos_webengineinstall.sh")
    install(TARGETS ${PROJECT}
        RUNTIME DESTINATION bin COMPONENT MACOS_BUNDLE
        BUNDLE DESTINATION . COMPONENT MACOS_BUNDLE
        # ARCHIVE DESTINATION lib COMPONENT HDPS_LINKLIB
    )
    # -always-overwrite
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
    install(CODE "execute_process(
            COMMAND ${MACDEPLOYQT_EXECUTABLE} \"${BUNDLE_DIR}\" -always-overwrite
        #	RESULT_VARIABLE retval
            COMMAND ${CMAKE_COMMAND} -E make_directory \"${BUNDLE_DIR}/Contents/Frameworks\"
            COMMAND ${CMAKE_COMMAND} -E copy 
                    \"${INSTALL_DIR}/$<CONFIGURATION>/lib/libqtadvanceddocking.3.6.3.dylib\" 
                    \"${BUNDLE_DIR}/Contents/Frameworks/\"
            COMMAND ${CMAKE_COMMAND} -E copy 
                    \"$<TARGET_FILE:${PROJECT}_Public>\" 
                    \"${BUNDLE_DIR}/Contents/Frameworks/\"
                    
            COMMAND ${CMAKE_COMMAND} -E copy 
                    \"$<TARGET_FILE:${PROJECT}_Private>\" 
                    \"${BUNDLE_DIR}/Contents/Frameworks/\"
        )
        
        #if(NOT retval EQUAL 0)
        #	message(FATAL_ERROR \"macdeployqt failed\")
        #endif()
        "
        COMPONENT MACOS_BUNDLE
    )
    install(CODE "execute_process(
            COMMAND bash ${WEBENGINEINSTALL_CMD} ${Qt5_DIR} ${BUNDLE_DIR}
        )"
        COMPONENT MACOS_BUNDLE
    )
else()
    install(TARGETS ${PROJECT}
        RUNTIME DESTINATION . COMPONENT HDPS_EXECUTABLE
        # ARCHIVE DESTINATION lib COMPONENT HDPS_LINKLIB
    )
endif()

#headers - Would it be better to refactor the headers to allow "install(DIRECTORY)"?
install(CODE "foreach(PUBLIC_HEADER ${PUBLIC_HEADERS})
        string(REGEX REPLACE \"^src/\" \"\" STRIPPED_PUBLIC_HEADER \${PUBLIC_HEADER})
        execute_process(
            COMMAND \"${CMAKE_COMMAND}\" -E copy_if_different 
                \"${CMAKE_CURRENT_SOURCE_DIR}/\${PUBLIC_HEADER}\" 
                \"${INSTALL_DIR}/$<CONFIGURATION>/include/\${STRIPPED_PUBLIC_HEADER}\"
        )

    endforeach(PUBLIC_HEADER)"
    COMPONENT HDPSPUBLIC_HEADERS
)

install(TARGETS ${PROJECT}_Public
    EXPORT HdpsPublicExport
    ARCHIVE DESTINATION lib COMPONENT HDPSPUBLIC_LINKLIB
    LIBRARY DESTINATION lib COMPONENT HDPSPUBLIC_LINKLIB
    PUBLIC_HEADER DESTINATION include COMPONENT HDPSPUBLIC_HEADERS
    INCLUDES DESTINATION include
)

# Generate a .cmake file that can be imported from the install
# to enable plugin building
# Plugin building requires HDPS_Public.lib and the include headers
# By convention this is placed in the lib directory
install(EXPORT HdpsPublicExport
        FILE HdpsCoreTargets.cmake
        NAMESPACE HdpsCore::
        DESTINATION lib/cmake/HdpsCore
        COMPONENT HDPSPUBLIC_TARGET
)

target_include_directories(${PROJECT}_Public PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# include CMakePackageConfigHelpers macro
include(CMakePackageConfigHelpers)

# set version
set(version 0.2)

# ***Create a HdpsCore package to support the find_package command**

# Generate the version file for use with find_package
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/HdpsCoreConfigVersion.cmake"
  VERSION "${version}"
  # When the hdps core is stable - version 1.0 or greater -
  # compatibility can change to
  # AnyNewerVersion or SameMajorVersion
  # until then employ ExactVersion
  COMPATIBILITY ExactVersion
)

# create config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/HdpsCoreConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/HdpsCoreConfig.cmake"
  INSTALL_DESTINATION lib/cmake/HdpsCore
)

# install the package files files
install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/HdpsCoreConfig.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/HdpsCoreConfigVersion.cmake"
        DESTINATION lib/cmake/HdpsCore
        COMPONENT HDPSPUBLIC_PACKAGE
)

# could be used for the plugins subdirectories 
# see doc https://cmake.org/cmake/help/v3.18/command/export.html
#
# export(EXPORT HdpsCoreTargets
#        FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/HdpsCoreTargets.cmake"
#        NAMESPACE HdpsCore::
# )

# Initially install everything in the same place
# this may need to be split up 

# Install the HDPSPUBLIC components needed for the Plugin builds
add_custom_command(
    TARGET ${PROJECT}_Public
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_BINARY_DIR}
        --config $<CONFIGURATION>
        --component HDPSPUBLIC_HEADERS
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
        --verbose
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_BINARY_DIR}
        --config $<CONFIGURATION>
        --component HDPSPUBLIC_LINKLIB
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
        --verbose
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_BINARY_DIR}
        --config $<CONFIGURATION>
        --component HDPSPUBLIC_PACKAGE
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
        --verbose
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_BINARY_DIR}
        --config $<CONFIGURATION>
        --component HDPSPUBLIC_TARGET
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
        --verbose
)

# Install the HDPS executable and lib
add_custom_command(
    TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_BINARY_DIR}
        --config $<CONFIGURATION>
        --component HDPS_EXECUTABLE
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
        --verbose
    # COMMAND "${CMAKE_COMMAND}"
    #     --install ${CMAKE_BINARY_DIR}
    #     --config $<CONFIGURATION>
    #     --component HDPS_LINKLIB
    #     --prefix ${INSTALL_DIR}/$<CONFIGURATION>
    #     --verbose
)

# Point Data
add_subdirectory(src/plugins/PointData)

# Color Data
add_subdirectory(src/plugins/ColorData)

# Text Data
add_subdirectory(src/plugins/TextData)

# Cluster Data
add_subdirectory(src/plugins/ClusterData)

# Image Data
add_subdirectory(src/plugins/ImageData)

# Qt Advanced docking system
set(VERSION_SHORT 3.7.0)
set(BUILD_EXAMPLES OFF CACHE BOOL "Qt-ads examples")
add_subdirectory(external)
add_subdirectory(external/advanced_docking)

# File compression LIBRARY
if (WIN32)
    message(status, "********Using QTZLIB for QUAZIP *********")
    set(QUAZIP_USE_QT_ZLIB ON)  # Use the Qt supplied zlib
elseif (APPLE)
    message(status, "********Using brew zlib for QUAZIP *********")
    set(QUAZIP_USE_QT_ZLIB OFF)
    find_program(BREW_BIN brew)
    execute_process(COMMAND ${BREW_BIN} install pkg-config) 
else ()
    message(FATAL_ERROR,
    "*****************************
    zlib needed for linux build.
    ******************************" )
endif()
add_subdirectory(external/quazip)

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" 
        --install ${CMAKE_CURRENT_BINARY_DIR}/external/advanced_docking
        --config $<CONFIGURATION> 
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>/
    COMMAND "${CMAKE_COMMAND}" 
        --install ${CMAKE_CURRENT_BINARY_DIR}/external/quazip
        --config $<CONFIGURATION> 
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>/
)

if(WIN32)
    # Note! the quazip lib is named differently in Debug & Release
    # simply copy the bin directory contents to let HDPS.exe run.
    add_custom_command(TARGET ${PROJECT} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${INSTALL_DIR}/$<CONFIGURATION>/bin/
            ${INSTALL_DIR}/$<CONFIGURATION>/
    )
endif()

if(APPLE)
    add_custom_command(TARGET ${PROJECT} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" 
            --install ${CMAKE_BINARY_DIR}
            --config $<CONFIGURATION> 
            --component MACOS_BUNDLE
            --prefix ${INSTALL_DIR}/$<CONFIGURATION>/
    )
endif()

# Automatically set the debug environment (command + working directory) for MSVC in debug mode
set_property(TARGET ${PROJECT} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY  $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug,${INSTALL_DIR}/release>)
set_property(TARGET ${PROJECT} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug/HDPS.exe,${INSTALL_DIR}/release/HDPS.exe>)
