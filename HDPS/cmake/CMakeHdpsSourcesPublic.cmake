# source files for target HDPS_PUBLIC_LIB

set(PUBLIC_CORE_INTERFACE_HEADERS
    src/CoreInterface.h
    src/AbstractManager.h
    src/AbstractDataManager.h
    src/AbstractPluginManager.h
    src/AbstractActionsManager.h
    src/AbstractDataHierarchyManager.h
    src/AbstractEventManager.h
    src/AbstractWorkspaceManager.h
    src/AbstractProjectManager.h
    src/AbstractSettingsManager.h
)

set(PUBLIC_CORE_INTERFACE_SOURCES
    src/CoreInterface.cpp
)

set(PUBLIC_CORE_INTERFACE_FILES
    ${PUBLIC_CORE_INTERFACE_HEADERS}
    ${PUBLIC_CORE_INTERFACE_SOURCES}
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

set(PUBLIC_COLOR_MAP_ACTION_HEADERS
    src/actions/ColorMapAction.h
    src/actions/ColorMap1DAction.h
    src/actions/ColorMap2DAction.h
    src/actions/ColorMapEditor1DAction.h
    src/actions/ColorMapEditor1DEdgesGraphicsItem.h
    src/actions/ColorMapEditor1DHistogramGraphicsItem.h
    src/actions/ColorMapEditor1DNode.h
    src/actions/ColorMapEditor1DNodeGraphicsItem.h
    src/actions/ColorMapEditor1DNodeAction.h
    src/actions/ColorMapEditor1DScene.h
    src/actions/ColorMapEditor1DWidget.h
    src/actions/ColorMapViewAction.h
    src/actions/ColorMapSettingsAction.h
    src/actions/ColorMapSettings1DAction.h
    src/actions/ColorMapSettings2DAction.h
)

set(PUBLIC_COLOR_MAP_ACTION_SOURCES
    src/actions/ColorMapAction.cpp
    src/actions/ColorMap1DAction.cpp
    src/actions/ColorMap2DAction.cpp
    src/actions/ColorMapEditor1DAction.cpp
    src/actions/ColorMapEditor1DEdgesGraphicsItem.cpp
    src/actions/ColorMapEditor1DHistogramGraphicsItem.cpp
    src/actions/ColorMapEditor1DNode.cpp
    src/actions/ColorMapEditor1DNodeGraphicsItem.cpp
    src/actions/ColorMapEditor1DNodeAction.cpp
    src/actions/ColorMapEditor1DScene.cpp
    src/actions/ColorMapEditor1DWidget.cpp
    src/actions/ColorMapViewAction.cpp
    src/actions/ColorMapSettingsAction.cpp
    src/actions/ColorMapSettings1DAction.cpp
    src/actions/ColorMapSettings2DAction.cpp
)

set(PUBLIC_COLOR_MAP_ACTION_FILES
    ${PUBLIC_COLOR_MAP_ACTION_HEADERS}    
    ${PUBLIC_COLOR_MAP_ACTION_SOURCES}
)

set(PUBLIC_NUMERICAL_ACTIONS_HEADERS
    src/actions/NumericalAction.h
    src/actions/DecimalAction.h
    src/actions/IntegralAction.h
    src/actions/NumericalRangeAction.h
    src/actions/DecimalRangeAction.h
    src/actions/IntegralRangeAction.h
)

set(PUBLIC_NUMERICAL_ACTIONS_SOURCES
    src/actions/NumericalAction.cpp
    src/actions/DecimalAction.cpp
    src/actions/IntegralAction.cpp
    src/actions/NumericalRangeAction.cpp
    src/actions/DecimalRangeAction.cpp
    src/actions/IntegralRangeAction.cpp
)

set(PUBLIC_NUMERICAL_ACTIONS_FILES
    ${PUBLIC_NUMERICAL_ACTIONS_HEADERS}    
    ${PUBLIC_NUMERICAL_ACTIONS_SOURCES}
)

set(PUBLIC_RECTANGLE_ACTIONS_HEADERS
    src/actions/RectangleAction.h
    src/actions/IntegralRectangleAction.h
    src/actions/DecimalRectangleAction.h
)

set(PUBLIC_RECTANGLE_ACTIONS_SOURCES
    src/actions/RectangleAction.cpp
    src/actions/IntegralRectangleAction.cpp
    src/actions/DecimalRectangleAction.cpp
)

set(PUBLIC_RECTANGLE_ACTIONS_FILES
    ${PUBLIC_RECTANGLE_ACTIONS_HEADERS}    
    ${PUBLIC_RECTANGLE_ACTIONS_SOURCES}
)

set(PUBLIC_TEXTUAL_ACTIONS_HEADERS
    src/actions/StringAction.h
    src/actions/StringsAction.h
)

set(PUBLIC_TEXTUAL_ACTIONS_SOURCES
    src/actions/StringAction.cpp
    src/actions/StringsAction.cpp
)

set(PUBLIC_TEXTUAL_ACTIONS_FILES
    ${PUBLIC_TEXTUAL_ACTIONS_HEADERS}    
    ${PUBLIC_TEXTUAL_ACTIONS_SOURCES}
)

set(PUBLIC_GROUPING_ACTIONS_HEADERS
    src/actions/GroupAction.h
    src/actions/StretchAction.h
    src/actions/HorizontalGroupAction.h
    src/actions/VerticalGroupAction.h
    src/actions/GroupsAction.h
    src/actions/GroupSectionTreeItem.h
    src/actions/GroupWidgetTreeItem.h
    src/actions/ToolbarAction.h
    src/actions/HorizontalToolbarAction.h
    src/actions/VerticalToolbarAction.h
    src/actions/ToolbarActionItem.h
    src/actions/ToolbarActionItemWidget.h
)

set(PUBLIC_GROUPING_ACTIONS_SOURCES
    src/actions/GroupAction.cpp
    src/actions/StretchAction.cpp
    src/actions/HorizontalGroupAction.cpp
    src/actions/VerticalGroupAction.cpp
    src/actions/GroupsAction.cpp
    src/actions/GroupSectionTreeItem.cpp
    src/actions/GroupWidgetTreeItem.cpp
	src/actions/ToolbarAction.cpp
	src/actions/HorizontalToolbarAction.cpp
	src/actions/VerticalToolbarAction.cpp
    src/actions/ToolbarActionItem.cpp
    src/actions/ToolbarActionItemWidget.cpp
)

set(PUBLIC_GROUPING_ACTIONS_FILES
    ${PUBLIC_GROUPING_ACTIONS_HEADERS}    
    ${PUBLIC_GROUPING_ACTIONS_SOURCES}
)

set(PUBLIC_TRIGGER_ACTIONS_HEADERS
    src/actions/TriggerAction.h
    src/actions/TriggersAction.h
)

set(PUBLIC_TRIGGER_ACTIONS_SOURCES
    src/actions/TriggerAction.cpp
    src/actions/TriggersAction.cpp
)

set(PUBLIC_TRIGGER_ACTIONS_FILES
    ${PUBLIC_TRIGGER_ACTIONS_HEADERS}    
    ${PUBLIC_TRIGGER_ACTIONS_SOURCES}
)

set(PUBLIC_OPTION_ACTIONS_HEADERS
    src/actions/OptionAction.h
    src/actions/OptionsAction.h
)

set(PUBLIC_OPTION_ACTIONS_SOURCES
    src/actions/OptionAction.cpp
    src/actions/OptionsAction.cpp
)

set(PUBLIC_OPTION_ACTIONS_FILES
    ${PUBLIC_OPTION_ACTIONS_HEADERS}    
    ${PUBLIC_OPTION_ACTIONS_SOURCES}
)

set(PUBLIC_COLOR_ACTIONS_HEADERS
    src/actions/ColorAction.h
    src/actions/ColorPickerAction.h
)

set(PUBLIC_COLOR_ACTIONS_SOURCES
    src/actions/ColorAction.cpp
    src/actions/ColorPickerAction.cpp
)

set(PUBLIC_COLOR_ACTIONS_FILES
    ${PUBLIC_COLOR_ACTIONS_HEADERS}    
    ${PUBLIC_COLOR_ACTIONS_SOURCES}
)

set(PUBLIC_TOGGLE_ACTIONS_HEADERS
    src/actions/ToggleAction.h
)

set(PUBLIC_TOGGLE_ACTIONS_SOURCES
    src/actions/ToggleAction.cpp
)

set(PUBLIC_TOGGLE_ACTIONS_FILES
    ${PUBLIC_TOGGLE_ACTIONS_HEADERS}    
    ${PUBLIC_TOGGLE_ACTIONS_SOURCES}
)

set(PUBLIC_FILE_ACTIONS_HEADERS
    src/actions/DirectoryPickerAction.h
    src/actions/FilePickerAction.h
)

set(PUBLIC_FILE_ACTIONS_SOURCES
    src/actions/DirectoryPickerAction.cpp
    src/actions/FilePickerAction.cpp
)

set(PUBLIC_FILE_ACTIONS_FILES
    ${PUBLIC_FILE_ACTIONS_HEADERS}    
    ${PUBLIC_FILE_ACTIONS_SOURCES}
)

set(PUBLIC_TOOLBAR_ACTIONS_HEADERS
    src/actions/ToolbarAction.h
    src/actions/ToolbarActionItem.h
    src/actions/ToolbarActionItemWidget.h
)

set(PUBLIC_TOOLBAR_ACTIONS_SOURCES
    src/actions/ToolbarAction.cpp
    src/actions/ToolbarActionItem.cpp
    src/actions/ToolbarActionItemWidget.cpp
)

set(PUBLIC_TOOLBAR_ACTIONS_FILES
    ${PUBLIC_TOOLBAR_ACTIONS_HEADERS}    
    ${PUBLIC_TOOLBAR_ACTIONS_SOURCES}
)

set(PUBLIC_MISCELLANEOUS_ACTIONS_HEADERS
    src/actions/Actions.h
    src/actions/DatasetPickerAction.h
    src/actions/ImageAction.h
    src/actions/PixelSelectionAction.h
    src/actions/PluginTriggerPickerAction.h
    src/actions/PluginTriggerAction.h
    src/actions/WindowLevelAction.h
    src/actions/VariantAction.h
    src/actions/StatusAction.h
    src/actions/RecentFilesAction.h
    src/actions/PresetsAction.h
    src/actions/LockingAction.h
    src/actions/VersionAction.h
    src/actions/ProjectSplashScreenAction.h
)

set(PUBLIC_MISCELLANEOUS_ACTIONS_SOURCES
    src/actions/DatasetPickerAction.cpp
    src/actions/ImageAction.cpp
    src/actions/PixelSelectionAction.cpp
    src/actions/PluginTriggerPickerAction.cpp
    src/actions/PluginTriggerAction.cpp
    src/actions/WindowLevelAction.cpp
    src/actions/VariantAction.cpp
    src/actions/StatusAction.cpp
    src/actions/RecentFilesAction.cpp
    src/actions/PresetsAction.cpp
    src/actions/LockingAction.cpp
    src/actions/VersionAction.cpp
    src/actions/ProjectSplashScreenAction.cpp
)

set(PUBLIC_MISCELLANEOUS_ACTIONS_FILES
    ${PUBLIC_MISCELLANEOUS_ACTIONS_HEADERS}    
    ${PUBLIC_MISCELLANEOUS_ACTIONS_SOURCES}
)

set(PUBLIC_ACTIONS_INTERNAL_HEADERS
    src/actions/WidgetAction.h
    src/actions/WidgetActionWidget.h
    src/actions/WidgetActionCollapsedWidget.h
    src/actions/WidgetActionLabel.h
    src/actions/WidgetActionContextMenu.h
    src/actions/WidgetActionViewWidget.h
    src/actions/WidgetActionMimeData.h
    src/actions/WidgetActionHighlightWidget.h
)

set(PUBLIC_ACTIONS_INTERNAL_SOURCES
    src/actions/WidgetAction.cpp
    src/actions/WidgetActionWidget.cpp
    src/actions/WidgetActionCollapsedWidget.cpp
    src/actions/WidgetActionLabel.cpp
    src/actions/WidgetActionContextMenu.cpp
    src/actions/WidgetActionViewWidget.cpp
    src/actions/WidgetActionMimeData.cpp
    src/actions/WidgetActionHighlightWidget.cpp
)

set(PUBLIC_ACTIONS_INTERNAL_FILES
    ${PUBLIC_ACTIONS_INTERNAL_HEADERS}    
    ${PUBLIC_ACTIONS_INTERNAL_SOURCES}
)

set(PUBLIC_WIDGET_HEADERS
    src/widgets/WebWidget.h
    src/widgets/DropWidget.h
    src/widgets/Divider.h
    src/widgets/HierarchyWidget.h
    src/widgets/InfoOverlayWidget.h
    src/widgets/OverlayWidget.h
    src/widgets/InfoWidget.h
    src/widgets/ViewPluginEditorDialog.h
    src/widgets/ActionsWidget.h
    src/widgets/TaskProgressDialog.h
)

set(PUBLIC_WIDGET_SOURCES
    src/widgets/WebWidget.cpp
    src/widgets/DropWidget.cpp
    src/widgets/Divider.cpp
    src/widgets/HierarchyWidget.cpp
    src/widgets/InfoOverlayWidget.cpp
    src/widgets/OverlayWidget.cpp
    src/widgets/InfoWidget.cpp
    src/widgets/ViewPluginEditorDialog.cpp
    src/widgets/ActionsWidget.cpp
    src/widgets/TaskProgressDialog.cpp
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
    src/util/WidgetOverlayer.h
    src/util/Serialization.h
    src/util/Serializable.h
    src/util/DockArea.h
    src/util/Logger.h
    src/util/AbstractItemModelTester.h
    src/util/Version.h
    src/util/DockWidgetPermission.h
    src/util/NumericalRange.h
)

if(APPLE)
set(PUBLIC_UTIL_HEADERS
	${PUBLIC_UTIL_HEADERS}
    src/util/MacThemeHelper.h
)
endif()

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
    src/util/WidgetOverlayer.cpp
    src/util/Serialization.cpp
    src/util/Serializable.cpp
    src/util/DockArea.cpp
    src/util/Logger.cpp
    src/util/AbstractItemModelTester.cpp
    src/util/Version.cpp
    src/util/DockWidgetPermission.cpp
    src/util/NumericalRange.cpp
)

if(APPLE)
set(PUBLIC_UTIL_SOURCES
	${PUBLIC_UTIL_SOURCES}
    src/util/MacThemeHelper.mm
)
endif()

set(PUBLIC_UTIL_FILES
    ${PUBLIC_UTIL_HEADERS}    
    ${PUBLIC_UTIL_SOURCES}
)

set(PUBLIC_APPLICATION_HEADERS
    src/Application.h
    src/Project.h
    src/Workspace.h
)

set(PUBLIC_APPLICATION_SOURCES
    src/Application.cpp
    src/Project.cpp
    src/Workspace.cpp
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
    src/DatasetsMimeData.h
)

set(PUBLIC_DATASET_SOURCES
    src/Set.cpp
    src/LinkedData.cpp
    src/Dataset.cpp
    src/DatasetPrivate.cpp
    src/DatasetsMimeData.cpp
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
    src/PluginType.cpp
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

set(PUBLIC_ACTIONS_MODEL_HEADERS
    src/models/AbstractActionsModel.h
    src/models/ActionsListModel.h
    src/models/ActionsHierarchyModel.h
    src/models/ActionsFilterModel.h
)

set(PUBLIC_ACTIONS_MODEL_SOURCES
    src/models/AbstractActionsModel.cpp
    src/models/ActionsListModel.cpp
	src/models/ActionsHierarchyModel.cpp
    src/models/ActionsFilterModel.cpp
)

set(PUBLIC_ACTIONS_MODEL_FILES
    ${PUBLIC_ACTIONS_MODEL_HEADERS}
    ${PUBLIC_ACTIONS_MODEL_SOURCES}
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

set(PUBLIC_GLOBAL_SETTINGS_HEADERS
    src/GlobalSettingsGroupAction.h
    src/ParametersSettingsAction.h
    src/MiscellaneousSettingsAction.h
    src/ApplicationSettingsAction.h
)

set(PUBLIC_GLOBAL_SETTINGS_SOURCES
    src/GlobalSettingsGroupAction.cpp
	src/ParametersSettingsAction.cpp
    src/MiscellaneousSettingsAction.cpp
    src/ApplicationSettingsAction.cpp
)

set(PUBLIC_GLOBAL_SETTINGS_FILES
    ${PUBLIC_GLOBAL_SETTINGS_HEADERS}
    ${PUBLIC_GLOBAL_SETTINGS_SOURCES}
)

set(PUBLIC_HEADERS
    ${PUBLIC_CORE_INTERFACE_HEADERS}
    ${PUBLIC_EVENT_HEADERS}
    ${PUBLIC_COLOR_MAP_ACTION_HEADERS}
    ${PUBLIC_NUMERICAL_ACTIONS_HEADERS}
    ${PUBLIC_RECTANGLE_ACTIONS_HEADERS}
    ${PUBLIC_TEXTUAL_ACTIONS_HEADERS}
    ${PUBLIC_GROUPING_ACTIONS_HEADERS}
    ${PUBLIC_TRIGGER_ACTIONS_HEADERS}
    ${PUBLIC_OPTION_ACTIONS_HEADERS}
    ${PUBLIC_COLOR_ACTIONS_HEADERS}
    ${PUBLIC_TOGGLE_ACTIONS_HEADERS}
    ${PUBLIC_FILE_ACTIONS_HEADERS}
    ${PUBLIC_TOOLBAR_ACTIONS_HEADERS}
    ${PUBLIC_MISCELLANEOUS_ACTIONS_HEADERS}
    ${PUBLIC_ACTIONS_INTERNAL_HEADERS}
    ${PUBLIC_WIDGET_HEADERS}
    ${PUBLIC_RENDERERS_HEADERS}
    ${PUBLIC_GRAPHICS_HEADERS}
    ${PUBLIC_UTIL_HEADERS}
    ${PUBLIC_APPLICATION_HEADERS}
    ${PUBLIC_DATASET_HEADERS}
    ${PUBLIC_PLUGIN_HEADERS}
    ${PUBLIC_PROJECT_HEADERS}
    ${PUBLIC_DATA_HIERARCHY_HEADERS}
    ${PUBLIC_ACTIONS_MODEL_HEADERS}
    ${PUBLIC_DATA_HIERARCHY_MODEL_HEADERS}
    ${PUBLIC_GLOBAL_SETTINGS_HEADERS}
)

set(PUBLIC_SOURCES
    ${PUBLIC_EVENT_SOURCES}
    ${PUBLIC_COLOR_MAP_ACTION_SOURCES}
    ${PUBLIC_NUMERICAL_ACTIONS_SOURCES}
    ${PUBLIC_RECTANGLE_ACTIONS_SOURCES}
    ${PUBLIC_TEXTUAL_ACTIONS_SOURCES}
    ${PUBLIC_GROUPING_ACTIONS_SOURCES}
    ${PUBLIC_TRIGGER_ACTIONS_SOURCES}
    ${PUBLIC_OPTION_ACTIONS_SOURCES}
    ${PUBLIC_COLOR_ACTIONS_SOURCES}
    ${PUBLIC_TOGGLE_ACTIONS_SOURCES}
    ${PUBLIC_FILE_ACTIONS_SOURCES}
    ${PUBLIC_TOOLBAR_ACTIONS_SOURCES}
    ${PUBLIC_MISCELLANEOUS_ACTIONS_SOURCES}
    ${PUBLIC_ACTIONS_INTERNAL_SOURCES}
    ${PUBLIC_WIDGET_SOURCES}
    ${PUBLIC_RENDERERS_SOURCES}
    ${PUBLIC_GRAPHICS_SOURCES}
    ${PUBLIC_UTIL_SOURCES}
    ${PUBLIC_APPLICATION_SOURCES}
    ${PUBLIC_DATASET_SOURCES}
    ${PUBLIC_PLUGIN_SOURCES}
    ${PUBLIC_DATA_HIERARCHY_SOURCES}
    ${PUBLIC_PROJECT_SOURCES}
    ${PUBLIC_ACTIONS_MODEL_SOURCES}
    ${PUBLIC_DATA_HIERARCHY_MODEL_SOURCES}
    ${PUBLIC_GLOBAL_SETTINGS_SOURCES}
    ${PUBLIC_HEADERS}
)

source_group(CoreInterface FILES ${PUBLIC_CORE_INTERFACE_FILES})
source_group(Event FILES ${PUBLIC_EVENT_FILES})
source_group(Actions\\Colormap FILES ${PUBLIC_COLOR_MAP_ACTION_FILES})
source_group(Actions\\Numerical FILES ${PUBLIC_NUMERICAL_ACTIONS_FILES})
source_group(Actions\\Rectangle FILES ${PUBLIC_RECTANGLE_ACTIONS_FILES})
source_group(Actions\\Textual FILES ${PUBLIC_TEXTUAL_ACTIONS_FILES})
source_group(Actions\\Grouping FILES ${PUBLIC_GROUPING_ACTIONS_FILES})
source_group(Actions\\Trigger FILES ${PUBLIC_TRIGGER_ACTIONS_FILES})
source_group(Actions\\Option FILES ${PUBLIC_OPTION_ACTIONS_FILES})
source_group(Actions\\Color FILES ${PUBLIC_COLOR_ACTIONS_FILES})
source_group(Actions\\Toggle FILES ${PUBLIC_TOGGLE_ACTIONS_FILES})
source_group(Actions\\File FILES ${PUBLIC_FILE_ACTIONS_FILES})
source_group(Actions\\Toolbar FILES ${PUBLIC_TOOLBAR_ACTIONS_FILES})
source_group(Actions\\Miscellaneous FILES ${PUBLIC_MISCELLANEOUS_ACTIONS_FILES})
source_group(Actions\\Internal FILES ${PUBLIC_ACTIONS_INTERNAL_FILES})
source_group(Widgets FILES ${PUBLIC_WIDGET_FILES})
source_group(Renderers FILES ${PUBLIC_RENDERERS_FILES})
source_group(Graphics FILES ${PUBLIC_GRAPHICS_FILES})
source_group(Util FILES ${PUBLIC_UTIL_FILES})
source_group(Application FILES ${PUBLIC_APPLICATION_FILES})
source_group(DataHierarchy FILES ${PUBLIC_DATA_HIERARCHY_FILES})
source_group(Dataset FILES ${PUBLIC_DATASET_FILES})
source_group(Plugin FILES ${PUBLIC_PLUGIN_FILES})
source_group(Project FILES ${PUBLIC_PROJECT_FILES})
source_group(Model\\Actions FILES ${PUBLIC_ACTIONS_MODEL_FILES})
source_group(Model\\DataHierarchy FILES ${PUBLIC_DATA_HIERARCHY_MODEL_FILES})
source_group(GlobalSettings FILES ${PUBLIC_GLOBAL_SETTINGS_FILES})