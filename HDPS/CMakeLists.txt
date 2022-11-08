cmake_minimum_required(VERSION 3.17)

set(PROJECT "hdps-common")
project(${PROJECT})

# These three targets defined in this CMakeLists.txt
set(HDPS_EXE HDPS)  # The HDPS executable
set(HDPS_PUBLIC_LIB HDPS_Public) # A public shared library used by plugin builders and the HDPS executable
set(HDPS_PRIVATE_LIB HDPS_Private) # A private shared library contain code for the HDPS executable

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

find_package(Qt6 6.3 COMPONENTS Widgets WebEngineWidgets OpenGL Core REQUIRED)

# Normalize the incoming install path
file(TO_CMAKE_PATH $ENV{HDPS_INSTALL_DIR} INSTALL_DIR)
#message(STATUS ${INSTALL_DIR})

set( MAIN_SOURCES
    src/Main.cpp
    res/styles/default.qss
)

set(PRIVATE_CORE_HEADERS
    src/Core.h
)

set(PRIVATE_CORE_SOURCES
    src/Core.cpp
)

set(PRIVATE_CORE_FILES
    ${PRIVATE_CORE_HEADERS}
    ${PRIVATE_CORE_SOURCES}
)

set(PRIVATE_PLUGIN_MANAGER_HEADERS
    src/PluginManager.h
)

set(PRIVATE_PLUGIN_MANAGER_SOURCES
    src/PluginManager.cpp
)

set(PRIVATE_PLUGIN_MANAGER_FILES
    ${PRIVATE_PLUGIN_MANAGER_HEADERS}
    ${PRIVATE_PLUGIN_MANAGER_SOURCES}
)

set(PRIVATE_DATA_MANAGER_HEADERS
    src/DataManager.h
)

set(PRIVATE_DATA_MANAGER_SOURCES
    src/DataManager.cpp
)

set(PRIVATE_DATA_MANAGER_FILES
    ${PRIVATE_DATA_MANAGER_HEADERS}
    ${PRIVATE_DATA_MANAGER_SOURCES}
)

set(PRIVATE_DATA_HIERARCHY_MANAGER_HEADERS
    src/DataHierarchyManager.h
)

set(PRIVATE_DATA_HIERARCHY_MANAGER_SOURCES
    src/DataHierarchyManager.cpp
)

set(PRIVATE_DATA_HIERARCHY_MANAGER_FILES
    ${PRIVATE_DATA_HIERARCHY_MANAGER_HEADERS}
    ${PRIVATE_DATA_HIERARCHY_MANAGER_SOURCES}
)

set(PRIVATE_ACTIONS_MANAGER_HEADERS
    src/ActionsManager.h
)

set(PRIVATE_ACTIONS_MANAGER_SOURCES
    src/ActionsManager.cpp
)

set(PRIVATE_ACTIONS_MANAGER_FILES
    ${PRIVATE_ACTIONS_MANAGER_HEADERS}
    ${PRIVATE_ACTIONS_MANAGER_SOURCES}
)

set(PRIVATE_APPLICATION_HEADERS
    src/HdpsApplication.h
    src/MainWindow.h
    ui/MainWindow.ui
)

set(PRIVATE_APPLICATION_SOURCES
    src/HdpsApplication.cpp
    src/MainWindow.cpp
)

set(PRIVATE_APPLICATION_FILES
    ${PRIVATE_APPLICATION_HEADERS}
    ${PRIVATE_APPLICATION_SOURCES}
)

set(PRIVATE_START_PAGE_HEADERS
    src/StartPageWidget.h
    src/ProjectBarWidget.h
    src/LogoWidget.h
)

set(PRIVATE_START_PAGE_SOURCES
    src/StartPageWidget.cpp
    src/ProjectBarWidget.cpp
    src/LogoWidget.cpp
)

set(PRIVATE_START_PAGE_FILES
    ${PRIVATE_START_PAGE_HEADERS}
    ${PRIVATE_START_PAGE_SOURCES}
)

set(PRIVATE_MISCELLANEOUS_HEADERS
    src/Archiver.h
    src/GroupDataDialog.h
)

set(PRIVATE_MISCELLANEOUS_SOURCES
    src/Archiver.cpp
    src/GroupDataDialog.cpp
)

set(PRIVATE_MISCELLANEOUS_FILES
    ${PRIVATE_MISCELLANEOUS_HEADERS}
    ${PRIVATE_MISCELLANEOUS_SOURCES}
)

set(PRIVATE_HEADERS
    ${PRIVATE_CORE_HEADERS}
    ${PRIVATE_PLUGIN_MANAGER_HEADERS}
    ${PRIVATE_DATA_MANAGER_HEADERS}
    ${PRIVATE_DATA_HIERARCHY_MANAGER_HEADERS}
    ${PRIVATE_ACTIONS_MANAGER_HEADERS}
    ${PRIVATE_APPLICATION_HEADERS}
    ${PRIVATE_START_PAGE_HEADERS}
    ${PRIVATE_MISCELLANEOUS_HEADERS}
)

set(PRIVATE_SOURCES
    ${PRIVATE_CORE_SOURCES}
    ${PRIVATE_PLUGIN_MANAGER_SOURCES}
    ${PRIVATE_DATA_MANAGER_SOURCES}
    ${PRIVATE_DATA_HIERARCHY_MANAGER_SOURCES}
    ${PRIVATE_ACTIONS_MANAGER_SOURCES}
    ${PRIVATE_APPLICATION_SOURCES}
    ${PRIVATE_START_PAGE_SOURCES}
    ${PRIVATE_MISCELLANEOUS_SOURCES}
    ${PRIVATE_HEADERS}
)

source_group(Private\\Core FILES ${PRIVATE_CORE_FILES})
source_group(Private\\Application FILES ${PRIVATE_APPLICATION_FILES})
source_group(Private\\Managers\\Plugin FILES ${PRIVATE_PLUGIN_MANAGER_FILES})
source_group(Private\\Managers\\Data FILES ${PRIVATE_DATA_MANAGER_FILES})
source_group(Private\\Managers\\DataHierarchy FILES ${PRIVATE_DATA_HIERARCHY_MANAGER_FILES})
source_group(Private\\Managers\\Actions FILES ${PRIVATE_ACTIONS_MANAGER_FILES})
source_group(Private\\StartPage FILES ${PRIVATE_START_PAGE_FILES})
source_group(Private\\Miscellaneous FILES ${PRIVATE_MISCELLANEOUS_FILES})

set(PUBLIC_CORE_INTERFACE_HEADERS
    src/CoreInterface.h
    src/AbstractDataManager.h
    src/AbstractPluginManager.h
    src/AbstractActionsManager.h
    src/AbstractDataHierarchyManager.h
    src/AbstractLayoutManager.h
)

set(PUBLIC_CORE_INTERFACE_FILES
    ${PUBLIC_CORE_INTERFACE_HEADERS}
)

set(PUBLIC_EVENT_HEADERS
    src/event/Event.h
    src/event/EventListener.h
)

set(PUBLIC_EVENT_SOURCES
    src/event/Event.cpp
    src/event/EventListener.cpp
)

set(PUBLIC_EVENT_FILES
    ${PUBLIC_EVENT_HEADERS}
    ${PUBLIC_EVENT_SOURCES}
)

set(PUBLIC_ACTIONS_HEADERS
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
    src/actions/PluginTriggerPickerAction.h
    src/actions/PluginTriggerAction.h
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
    src/actions/DirectoryPickerAction.h
    src/actions/FilePickerAction.h
    src/actions/StatusAction.h
    src/actions/GroupSectionTreeItem.h
    src/actions/GroupWidgetTreeItem.h
    src/actions/WidgetActionContextMenu.h
)

set(PUBLIC_ACTIONS_SOURCES
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
    src/actions/PluginTriggerPickerAction.cpp
    src/actions/PluginTriggerAction.cpp
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
    src/actions/DirectoryPickerAction.cpp
    src/actions/FilePickerAction.cpp
    src/actions/StatusAction.cpp
    src/actions/GroupSectionTreeItem.cpp
    src/actions/GroupWidgetTreeItem.cpp
    src/actions/WidgetActionContextMenu.cpp
)

set(PUBLIC_ACTIONS_FILES
    ${PUBLIC_ACTIONS_HEADERS}    
    ${PUBLIC_ACTIONS_SOURCES}
)

set(PUBLIC_WIDGET_HEADERS
    src/widgets/WebWidget.h
    src/widgets/DockableWidget.h
    src/widgets/DropWidget.h
    src/widgets/Divider.h
    src/widgets/HierarchyWidget.h
    src/widgets/OverlayWidget.h
    src/widgets/ProjectEditorDialog.h
    src/widgets/ActionHierarchyWidget.h
)

set(PUBLIC_WIDGET_SOURCES
    src/widgets/WebWidget.cpp
    src/widgets/DockableWidget.cpp
    src/widgets/DropWidget.cpp
    src/widgets/Divider.cpp
    src/widgets/HierarchyWidget.cpp
    src/widgets/OverlayWidget.cpp
    src/widgets/ProjectEditorDialog.cpp
    src/widgets/ActionHierarchyWidget.cpp
    ${ACTION_HIERARCHY_SOURCES}
)

set(PUBLIC_WIDGET_FILES
    ${PUBLIC_WIDGET_HEADERS}
    ${PUBLIC_WIDGET_SOURCES}
)

set(PUBLIC_RENDERERS_HEADERS
    src/renderers/Renderer.h
    src/renderers/PointRenderer.h
    src/renderers/DensityRenderer.h
    src/renderers/ImageRenderer.h
)

set(PUBLIC_RENDERERS_SOURCES
    src/renderers/PointRenderer.cpp
    src/renderers/DensityRenderer.cpp
    src/renderers/ImageRenderer.cpp
)

set(PUBLIC_RENDERERS_FILES
    ${PUBLIC_RENDERERS_HEADERS}
    ${PUBLIC_RENDERERS_SOURCES}
)

set(PUBLIC_GRAPHICS_HEADERS
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

set(PUBLIC_GRAPHICS_SOURCES
    src/graphics/BufferObject.cpp
    src/graphics/Shader.cpp
    src/graphics/Selection.cpp
    src/graphics/Matrix3f.cpp
    src/graphics/Vector2f.cpp
    src/graphics/Vector3f.cpp
    src/graphics/Bounds.cpp
)

set(PUBLIC_GRAPHICS_FILES
    ${PUBLIC_GRAPHICS_HEADERS}
    ${PUBLIC_GRAPHICS_SOURCES}
)

set(PUBLIC_UTIL_HEADERS
    src/util/Miscellaneous.h
    src/util/FileUtil.h
    src/util/MeanShift.h
    src/util/DensityComputation.h
    src/util/Exception.h
    src/util/Math.h
    src/util/Timer.h
    src/util/Icon.h
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
    src/util/WidgetFader.h
    src/util/Serialization.h
    src/util/Serializable.h
)

set(PUBLIC_UTIL_SOURCES
    src/util/Miscellaneous.cpp
    src/util/FileUtil.cpp
    src/util/MeanShift.cpp
    src/util/DensityComputation.cpp
    src/util/Exception.cpp
    src/util/Math.cpp
    src/util/Timer.cpp
    src/util/Icon.cpp
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
    src/util/WidgetFader.cpp
    src/util/Serialization.cpp
    src/util/Serializable.cpp
)

set(PUBLIC_UTIL_FILES
    ${PUBLIC_UTIL_HEADERS}    
    ${PUBLIC_UTIL_SOURCES}
)

set(PUBLIC_APPLICATION_HEADERS
    src/Application.h
)

set(PUBLIC_APPLICATION_SOURCES
    src/Application.cpp
)

set(PUBLIC_APPLICATION_FILES
    ${PUBLIC_APPLICATION_HEADERS}
    ${PUBLIC_APPLICATION_SOURCES}
)

set(PUBLIC_DATASET_HEADERS
    src/DataType.h
    src/RawData.h
    src/Set.h
    src/LinkedData.h
    src/Dataset.h
    src/DatasetPrivate.h
)

set(PUBLIC_DATASET_SOURCES
    src/Set.cpp
    src/LinkedData.cpp
    src/Dataset.cpp
    src/DatasetPrivate.cpp
)

set(PUBLIC_DATASET_FILES
    ${PUBLIC_DATASET_HEADERS}
    ${PUBLIC_DATASET_SOURCES}
)

set(PUBLIC_PLUGIN_HEADERS
    src/Plugin.h
    src/PluginType.h
    src/PluginFactory.h
    src/LoaderPlugin.h
    src/WriterPlugin.h
    src/AnalysisPlugin.h
    src/TransformationPlugin.h
    src/ViewPlugin.h
)

set(PUBLIC_PLUGIN_SOURCES
    src/Plugin.cpp
    src/PluginFactory.cpp
    src/LoaderPlugin.cpp
    src/WriterPlugin.cpp
    src/AnalysisPlugin.cpp
    src/TransformationPlugin.cpp
    src/ViewPlugin.cpp
)

list(APPEND PUBLIC_PLUGIN_FILES
    ${PUBLIC_PLUGIN_HEADERS}
    ${PUBLIC_PLUGIN_SOURCES}
)

set(PUBLIC_DATA_HIERARCHY_HEADERS
    src/DataHierarchyItem.h
)

set(PUBLIC_DATA_HIERARCHY_SOURCES
    src/DataHierarchyItem.cpp
)

list(APPEND PUBLIC_DATA_HIERARCHY_FILES
    ${PUBLIC_DATA_HIERARCHY_HEADERS}
    ${PUBLIC_DATA_HIERARCHY_SOURCES}
)

set(PUBLIC_PROJECT_HEADERS
    src/Project.h
)

set(PUBLIC_PROJECT_SOURCES
    src/Project.cpp
)

list(APPEND PUBLIC_PROJECT_FILES
    ${PUBLIC_PROJECT_HEADERS}
    ${PUBLIC_PROJECT_SOURCES}
)

set(PUBLIC_ACTIONS_MODEL_HEADERS
    src/models/ActionsModel.h
    src/models/ActionsFilterModel.h
)

set(PUBLIC_ACTIONS_MODEL_SOURCES
    src/models/ActionsModel.cpp
    src/models/ActionsFilterModel.cpp
)

set(PUBLIC_ACTIONS_MODEL_FILES
    ${PUBLIC_ACTIONS_MODEL_HEADERS}
    ${PUBLIC_ACTIONS_MODEL_SOURCES}
)

set(PUBLIC_ACTION_HIERARCHY_MODEL_HEADERS
    src/models/ActionHierarchyModelItem.h
    src/models/ActionHierarchyModel.h
    src/models/ActionHierarchyFilterModel.h
)

set(PUBLIC_ACTION_HIERARCHY_MODEL_SOURCES
    src/models/ActionHierarchyModelItem.cpp
    src/models/ActionHierarchyModel.cpp
    src/models/ActionHierarchyFilterModel.cpp
)

set(PUBLIC_ACTION_HIERARCHY_MODEL_FILES
    ${PUBLIC_ACTION_HIERARCHY_MODEL_HEADERS}
    ${PUBLIC_ACTION_HIERARCHY_MODEL_SOURCES}
)

set(PUBLIC_DATA_HIERARCHY_MODEL_HEADERS
    src/models/DataHierarchyModelItem.h
    src/models/DataHierarchyModel.h
    src/models/DataHierarchyFilterModel.h
)

set(PUBLIC_DATA_HIERARCHY_MODEL_SOURCES
    src/models/DataHierarchyModelItem.cpp
    src/models/DataHierarchyModel.cpp
    src/models/DataHierarchyFilterModel.cpp
)

set(PUBLIC_DATA_HIERARCHY_MODEL_FILES
    ${PUBLIC_DATA_HIERARCHY_MODEL_HEADERS}
    ${PUBLIC_DATA_HIERARCHY_MODEL_SOURCES}
)

set(PUBLIC_HEADERS
    ${PUBLIC_CORE_INTERFACE_HEADERS}
    ${PUBLIC_EVENT_HEADERS}
    ${PUBLIC_ACTIONS_HEADERS}
    ${PUBLIC_ACTION_HIERARCHY_HEADERS}
    ${PUBLIC_WIDGET_HEADERS}
    ${PUBLIC_RENDERERS_HEADERS}
    ${PUBLIC_GRAPHICS_HEADERS}
    ${PUBLIC_UTIL_HEADERS}
    ${PUBLIC_APPLICATION_HEADERS}
    ${PUBLIC_ACTIONS_MANAGER_HEADERS}
    ${PUBLIC_LAYOUT_MANAGER_HEADERS}
    ${PUBLIC_DATASET_HEADERS}
    ${PUBLIC_PLUGIN_HEADERS}
    ${PUBLIC_PROJECT_HEADERS}
    ${PUBLIC_DATA_HIERARCHY_HEADERS}
    ${PUBLIC_ACTIONS_MODEL_HEADERS}
    ${PUBLIC_ACTION_HIERARCHY_MODEL_HEADERS}
    ${PUBLIC_DATA_HIERARCHY_MODEL_HEADERS}
)

set(PUBLIC_SOURCES
    ${PUBLIC_EVENT_SOURCES}
    ${PUBLIC_ACTIONS_SOURCES}
    ${PUBLIC_ACTION_HIERARCHY_SOURCES}
    ${PUBLIC_WIDGET_SOURCES}
    ${PUBLIC_RENDERERS_SOURCES}
    ${PUBLIC_GRAPHICS_SOURCES}
    ${PUBLIC_UTIL_SOURCES}
    ${PUBLIC_APPLICATION_SOURCES}
    ${PUBLIC_ACTIONS_MANAGER_SOURCES}
    ${PUBLIC_LAYOUT_MANAGER_SOURCES}
    ${PUBLIC_DATASET_SOURCES}
    ${PUBLIC_PLUGIN_SOURCES}
    ${PUBLIC_DATA_HIERARCHY_SOURCES}
    ${PUBLIC_PROJECT_SOURCES}
    ${PUBLIC_ACTIONS_MODEL_SOURCES}
    ${PUBLIC_ACTION_HIERARCHY_MODEL_SOURCES}
    ${PUBLIC_DATA_HIERARCHY_MODEL_SOURCES}
    ${PUBLIC_HEADERS}
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

add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)

QT6_WRAP_UI(UI_HEADERS ${UI_FILES})
QT6_ADD_RESOURCES(RESOURCE_FILES res/resources.qrc)

source_group(Public\\CoreInterface FILES ${PUBLIC_CORE_INTERFACE_FILES})
source_group(Public\\Event FILES ${PUBLIC_EVENT_FILES})
source_group(Public\\Actions FILES ${PUBLIC_ACTIONS_FILES})
source_group(Public\\Widgets FILES ${PUBLIC_WIDGET_FILES})
source_group(Public\\Renderers FILES ${PUBLIC_RENDERERS_FILES})
source_group(Public\\Graphics FILES ${PUBLIC_GRAPHICS_FILES})
source_group(Public\\Util FILES ${PUBLIC_UTIL_FILES})
source_group(Public\\Application FILES ${PUBLIC_APPLICATION_FILES})
source_group(Public\\DataHierarchy FILES ${PUBLIC_DATA_HIERARCHY_FILES})
source_group(Public\\Dataset FILES ${PUBLIC_DATASET_FILES})
source_group(Public\\Plugin FILES ${PUBLIC_PLUGIN_FILES})
source_group(Public\\Project FILES ${PUBLIC_PROJECT_FILES})
source_group(Public\\Model\\Actions FILES ${PUBLIC_ACTIONS_MODEL_FILES})
source_group(Public\\Model\\ActionHierarchy FILES ${PUBLIC_ACTION_HIERARCHY_MODEL_FILES})
source_group(Public\\Model\\DataHierarchy FILES ${PUBLIC_DATA_HIERARCHY_MODEL_FILES})

source_group( Resources FILES res/resources.qrc)
source_group( Resources FILES ${UI_FILES})
source_group( Resources FILES win/hdps.rc)
source_group( Resources\\generated FILES ${UI_HEADERS})

set(LINK_TYPE SHARED)
if(APPLE)
    set(LINK_TYPE SHARED)
endif(APPLE)
if(MSVC)
    set(LINK_TYPE STATIC)
endif(MSVC)

add_library(${HDPS_PUBLIC_LIB} ${LINK_TYPE}
    src/.editorconfig
    ${PUBLIC_SOURCES}
)

set_property(TARGET ${HDPS_PUBLIC_LIB} PROPERTY CXX_STANDARD 14)

target_include_directories(${HDPS_PUBLIC_LIB} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${HDPS_PUBLIC_LIB} Qt6::Widgets)
target_link_libraries(${HDPS_PUBLIC_LIB} Qt6::WebEngineWidgets)

add_library(${HDPS_PRIVATE_LIB} ${LINK_TYPE}
    src/.editorconfig
    ${PRIVATE_SOURCES}
	${RESOURCE_FILES}
)

set_property(TARGET ${HDPS_PRIVATE_LIB} PROPERTY CXX_STANDARD 14)

target_link_libraries(${HDPS_PRIVATE_LIB} Qt6::Widgets)
target_link_libraries(${HDPS_PRIVATE_LIB} Qt6::WebEngineWidgets)
target_link_libraries(${HDPS_PRIVATE_LIB} qtadvanceddocking)
target_link_libraries(${HDPS_PRIVATE_LIB} QuaZip)
target_link_libraries(${HDPS_PRIVATE_LIB} "$<TARGET_FILE:${HDPS_PUBLIC_LIB}>")

target_include_directories(${HDPS_PRIVATE_LIB} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external/advanced_docking/src)
target_include_directories(${HDPS_PRIVATE_LIB} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${HDPS_PRIVATE_LIB} PRIVATE ${INSTALL_DIR}/$<CONFIGURATION>/include/QuaZip-Qt6-1.2)

add_dependencies(${HDPS_PRIVATE_LIB} ${HDPS_PUBLIC_LIB})
add_dependencies(${HDPS_PRIVATE_LIB} QuaZip)
#add_dependencies(${HDPS_PRIVATE_LIB} QArchive)

add_executable(${HDPS_EXE} MACOSX_BUNDLE WIN32
    src/.editorconfig
    ${MAIN_SOURCES}
    ${UI_HEADERS}
    ${UI_FILES}
    ${RESOURCE_FILES}
)

set_property(TARGET ${HDPS_EXE} PROPERTY CXX_STANDARD 14)
# find libraries relative to the executable in the macOS .app tree
if(APPLE)
    set_target_properties( ${HDPS_EXE} PROPERTIES BUILD_WITH_INSTALL_RPATH True)
endif()

target_include_directories(${HDPS_EXE} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${HDPS_EXE} Qt6::Widgets)
target_link_libraries(${HDPS_EXE} Qt6::WebEngineWidgets)
target_link_libraries(${HDPS_EXE} qtadvanceddocking)
target_link_libraries(${HDPS_EXE} QuaZip)

target_link_libraries(${HDPS_EXE} "$<TARGET_FILE:${HDPS_PUBLIC_LIB}>")
target_link_libraries(${HDPS_EXE} "$<TARGET_FILE:${HDPS_PRIVATE_LIB}>")

add_dependencies(${HDPS_EXE} ${HDPS_PUBLIC_LIB})
add_dependencies(${HDPS_EXE} ${HDPS_PRIVATE_LIB})
add_dependencies(${HDPS_EXE} QuaZip)

if(MSVC)
    set_target_properties(${HDPS_EXE} PROPERTIES
        WIN32_EXECUTABLE YES
        LINK_FLAGS "/ENTRY:mainCRTStartup"
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif(MSVC)

#executable & linklib

target_include_directories(${HDPS_PUBLIC_LIB} PRIVATE "${INSTALL_DIR}/$<CONFIGURATION>/include/QArchive")

if(APPLE)
    set(BUNDLE_DIR "${INSTALL_DIR}/$<CONFIGURATION>/${MACOSX_BUNDLE_BUNDLE_NAME}.app")
    set(WEBENGINEINSTALL_CMD "${CMAKE_CURRENT_SOURCE_DIR}/macos_webengineinstall.sh")
    install(TARGETS ${HDPS_EXE}
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
                    \"$<TARGET_FILE:${HDPS_PUBLIC_LIB}\" 
                    \"${BUNDLE_DIR}/Contents/Frameworks/\"
                    
            COMMAND ${CMAKE_COMMAND} -E copy 
                    \"$<TARGET_FILE:${HDPS_PRIVATE_LIB}>\" 
                    \"${BUNDLE_DIR}/Contents/Frameworks/\"
        )
        
        #if(NOT retval EQUAL 0)
        #	message(FATAL_ERROR \"macdeployqt failed\")
        #endif()
        "
        COMPONENT MACOS_BUNDLE
    )
    install(CODE "execute_process(
            COMMAND bash ${WEBENGINEINSTALL_CMD} ${Qt6_DIR} ${BUNDLE_DIR}
        )"
        COMPONENT MACOS_BUNDLE
    )
else()
    install(TARGETS ${HDPS_EXE}
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
	MESSAGE(STATUS \"Installing: \${PUBLIC_HEADER}\")
    endforeach(PUBLIC_HEADER)"
    COMPONENT HDPSPUBLIC_HEADERS
)

install(TARGETS ${HDPS_PUBLIC_LIB}
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

target_include_directories(${HDPS_PUBLIC_LIB} PUBLIC
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
    TARGET ${HDPS_PUBLIC_LIB} POST_BUILD
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
    TARGET ${HDPS_EXE} POST_BUILD
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

add_subdirectory(src/plugins/DataHierarchyPlugin)
add_subdirectory(src/plugins/DataPropertiesPlugin)
add_subdirectory(src/plugins/PublicActionsPlugin)

# Qt Advanced docking system
set(VERSION_SHORT 3.7.0)
set(BUILD_EXAMPLES OFF CACHE BOOL "Qt-ads examples")
add_subdirectory(external)
add_subdirectory(external/advanced_docking)

# File compression LIBRARY
if (WIN32)
    #message(status, "********Using QTZLIB for QUAZIP *********")
    #set(QUAZIP_USE_QT_ZLIB ON)  # Use the Qt supplied zlib
elseif (APPLE)
    message(status, "********Using brew zlib for QUAZIP *********")
    #set(QUAZIP_USE_QT_ZLIB OFF)
    find_program(BREW_BIN brew)
    execute_process(COMMAND ${BREW_BIN} install pkg-config) 
else ()
    message(FATAL_ERROR,
    "*****************************
    zlib needed for linux build.
    ******************************" )
endif()

set(QUAZIP_QT_MAJOR_VERSION 6)
add_subdirectory(external/quazip)

add_custom_command(TARGET ${HDPS_EXE} POST_BUILD
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
    add_custom_command(TARGET ${HDPS_EXE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${INSTALL_DIR}/$<CONFIGURATION>/bin/
            ${INSTALL_DIR}/$<CONFIGURATION>/
    )
endif()

if(APPLE)
    add_custom_command(TARGET ${HDPS_EXE} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" 
            --install ${CMAKE_BINARY_DIR}
            --config $<CONFIGURATION> 
            --component MACOS_BUNDLE
            --prefix ${INSTALL_DIR}/$<CONFIGURATION>/
    )
endif()

# Automatically set the debug environment (command + working directory) for MSVC in debug mode
set_property(TARGET ${HDPS_EXE} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY  $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug,${INSTALL_DIR}/release>)
set_property(TARGET ${HDPS_EXE} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug/${HDPS_EXE}.exe,${INSTALL_DIR}/release/${HDPS_EXE}.exe>)
