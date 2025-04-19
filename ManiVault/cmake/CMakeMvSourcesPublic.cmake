# -----------------------------------------------------------------------------
# Source files for target MV_PUBLIC_LIB
# -----------------------------------------------------------------------------
# defines PUBLIC_SOURCES and PUBLIC_HEADERS

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
    src/AbstractTaskManager.h
    src/AbstractHelpManager.h
    src/AbstractErrorManager.h
    src/AbstractErrorLogger.h
    src/AbstractThemeManager.h
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
    src/actions/NumericalPointAction.h
    src/actions/DecimalPointAction.h
    src/actions/IntegralPointAction.h
	src/actions/RectangleAction.h
    src/actions/IntegralRectangleAction.h
    src/actions/DecimalRectangleAction.h
)

set(PUBLIC_NUMERICAL_ACTIONS_SOURCES
    src/actions/NumericalAction.cpp
    src/actions/DecimalAction.cpp
    src/actions/IntegralAction.cpp
    src/actions/NumericalRangeAction.cpp
    src/actions/DecimalRangeAction.cpp
    src/actions/IntegralRangeAction.cpp
	src/actions/NumericalPointAction.cpp
    src/actions/DecimalPointAction.cpp
    src/actions/IntegralPointAction.cpp
	src/actions/RectangleAction.cpp
    src/actions/IntegralRectangleAction.cpp
    src/actions/DecimalRectangleAction.cpp
)

set(PUBLIC_NUMERICAL_ACTIONS_FILES
    ${PUBLIC_NUMERICAL_ACTIONS_HEADERS}    
    ${PUBLIC_NUMERICAL_ACTIONS_SOURCES}
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
    src/actions/PluginPickerAction.h
    src/actions/ImageAction.h
    src/actions/PixelSelectionAction.h
    src/actions/WindowLevelAction.h
    src/actions/VariantAction.h
    src/actions/StatusAction.h
    src/actions/VersionAction.h
    src/actions/TreeAction.h
    src/actions/TableAction.h
    src/actions/StatusBarAction.h
    src/actions/PluginStatusBarAction.h
    src/actions/EventAction.h
)

set(PUBLIC_MISCELLANEOUS_ACTIONS_SOURCES
    src/actions/DatasetPickerAction.cpp
    src/actions/PluginPickerAction.cpp
    src/actions/ImageAction.cpp
    src/actions/PixelSelectionAction.cpp
    src/actions/WindowLevelAction.cpp
    src/actions/VariantAction.cpp
    src/actions/StatusAction.cpp
    src/actions/VersionAction.cpp
    src/actions/TreeAction.cpp
    src/actions/TableAction.cpp
    src/actions/StatusBarAction.cpp
    src/actions/PluginStatusBarAction.cpp
    src/actions/EventAction.cpp
)

set(PUBLIC_MISCELLANEOUS_ACTIONS_FILES
    ${PUBLIC_MISCELLANEOUS_ACTIONS_HEADERS}    
    ${PUBLIC_MISCELLANEOUS_ACTIONS_SOURCES}
)

set(PUBLIC_TASK_ACTIONS_HEADERS
    src/actions/ProgressAction.h
    src/actions/TaskAction.h
)

set(PUBLIC_TASK_ACTIONS_SOURCES
    src/actions/ProgressAction.cpp
    src/actions/TaskAction.cpp   
)

set(PUBLIC_TASK_ACTIONS_FILES
    ${PUBLIC_TASK_ACTIONS_HEADERS}    
    ${PUBLIC_TASK_ACTIONS_SOURCES}
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
    src/actions/WidgetActionDrag.h
    src/actions/TasksTreeAction.h
    src/actions/TasksListAction.h
    src/actions/SplashScreenAction.h
    src/actions/RecentFilesAction.h
    src/actions/PresetsAction.h
    src/actions/LockingAction.h
    src/actions/PluginTriggerPickerAction.h
    src/actions/PluginTriggerAction.h
    src/actions/LabelProxyAction.h
    src/actions/ModelFilterAction.h
    src/actions/ModelSelectionAction.h
    src/actions/HorizontalHeaderAction.h
    src/actions/WidgetActionBadgeOverlayWidget.h
    src/actions/WidgetActionToolButton.h
    src/actions/WidgetActionToolButtonMenu.h
    src/actions/IconAction.h
    src/actions/IconPickerAction.h
    src/actions/ApplicationIconAction.h
    src/actions/ViewPluginSamplerAction.h
    src/actions/PluginLearningCenterAction.h
    src/actions/WatchVideoAction.h
    src/actions/PaletteAction.h
    src/actions/PaletteColorAction.h
    src/actions/PaletteColorRoleAction.h
    src/actions/ColorSchemeAction.h
    src/actions/EditColorSchemeAction.h
    src/actions/NavigationAction.h
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
    src/actions/WidgetActionDrag.cpp
    src/actions/TasksTreeAction.cpp
    src/actions/TasksListAction.cpp
    src/actions/SplashScreenAction.cpp
    src/actions/RecentFilesAction.cpp
    src/actions/PresetsAction.cpp
    src/actions/LockingAction.cpp
    src/actions/PluginTriggerPickerAction.cpp
    src/actions/PluginTriggerAction.cpp
    src/actions/LabelProxyAction.cpp
	src/actions/ModelFilterAction.cpp
	src/actions/ModelSelectionAction.cpp
	src/actions/HorizontalHeaderAction.cpp
    src/actions/WidgetActionBadgeOverlayWidget.cpp
    src/actions/WidgetActionToolButton.cpp
    src/actions/WidgetActionToolButtonMenu.cpp
    src/actions/IconAction.cpp
    src/actions/IconPickerAction.cpp
    src/actions/ApplicationIconAction.cpp
    src/actions/ViewPluginSamplerAction.cpp
    src/actions/PluginLearningCenterAction.cpp
    src/actions/WatchVideoAction.cpp
    src/actions/PaletteAction.cpp
    src/actions/PaletteColorAction.cpp
    src/actions/PaletteColorRoleAction.cpp
    src/actions/ColorSchemeAction.cpp
    src/actions/EditColorSchemeAction.cpp
    src/actions/NavigationAction.cpp
)

set(PUBLIC_ACTIONS_INTERNAL_FILES
    ${PUBLIC_ACTIONS_INTERNAL_HEADERS}    
    ${PUBLIC_ACTIONS_INTERNAL_SOURCES}
)

set(PUBLIC_WIDGETS_HEADERS
    src/widgets/WebWidget.h
    src/widgets/DropWidget.h
    src/widgets/OpenGLWidget.h
    src/widgets/Divider.h
    src/widgets/ElidedLabel.h
    src/widgets/FileDialog.h
)

set(PUBLIC_WIDGETS_SOURCES
    src/widgets/WebWidget.cpp
    src/widgets/DropWidget.cpp
    src/widgets/OpenGLWidget.cpp
    src/widgets/Divider.cpp
    src/widgets/ElidedLabel.cpp
    src/widgets/FileDialog.cpp
)

set(PUBLIC_WIDGETS_FILES
    ${PUBLIC_WIDGETS_HEADERS}
    ${PUBLIC_WIDGETS_SOURCES}
)

set(PUBLIC_WIDGETS_INTERNAL_HEADERS
    src/widgets/HierarchyWidget.h
    src/widgets/InfoOverlayWidget.h
    src/widgets/OverlayWidget.h
    src/widgets/InfoWidget.h
    src/widgets/ViewPluginEditorDialog.h
    src/widgets/ActionsWidget.h
    src/widgets/SplashScreenWidget.h
    src/widgets/FlowLayout.h
	src/widgets/MarkdownDocument.h
	src/widgets/MarkdownDialog.h
	src/widgets/YouTubeVideoDialog.h
	src/widgets/ViewPluginOverlayWidget.h
	src/widgets/PluginAboutDialog.h
	src/widgets/PluginShortcutsDialog.h
	src/widgets/ViewPluginLearningCenterOverlayWidget.h
	src/widgets/IconLabel.h
	src/widgets/MultiSelectComboBox.h
)

set(PUBLIC_WIDGETS_INTERNAL_SOURCES
    src/widgets/HierarchyWidget.cpp
    src/widgets/InfoOverlayWidget.cpp
    src/widgets/OverlayWidget.cpp
    src/widgets/InfoWidget.cpp
    src/widgets/ViewPluginEditorDialog.cpp
    src/widgets/ActionsWidget.cpp
    src/widgets/SplashScreenWidget.cpp
    src/widgets/FlowLayout.cpp
	src/widgets/MarkdownDocument.cpp
	src/widgets/MarkdownDialog.cpp
	src/widgets/YouTubeVideoDialog.cpp
	src/widgets/ViewPluginOverlayWidget.cpp
	src/widgets/PluginAboutDialog.cpp
	src/widgets/PluginShortcutsDialog.cpp
	src/widgets/ViewPluginLearningCenterOverlayWidget.cpp
	src/widgets/IconLabel.cpp
	src/widgets/MultiSelectComboBox.cpp
)

set(PUBLIC_WIDGETS_INTERNAL_FILES
    ${PUBLIC_WIDGETS_INTERNAL_HEADERS}
    ${PUBLIC_WIDGETS_INTERNAL_SOURCES}
)

set(PUBLIC_RENDERERS_HEADERS
    src/renderers/Renderer.h
    src/renderers/PointRenderer.h
    src/renderers/DensityRenderer.h
    src/renderers/Renderer2D.h
    src/renderers/Navigator2D.h
)

set(PUBLIC_RENDERERS_SOURCES
    src/renderers/PointRenderer.cpp
    src/renderers/DensityRenderer.cpp
    src/renderers/Renderer2D.cpp
    src/renderers/Navigator2D.cpp
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
    src/util/Interpolation.h
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
    src/util/FileDownloader.h
    src/util/ShortcutMap.h
    src/util/Notification.h
    src/util/Notifications.h
    src/util/LearningCenterVideo.h
    src/util/LearningCenterTutorial.h
    src/util/StyledIconCommon.h
    src/util/StyledIcon.h
    src/util/StyledIconEngine.h
    src/util/Badge.h
    src/util/ColorScheme.h
    src/util/ProjectDatabaseProject.h
)

if(APPLE)
set(PUBLIC_UTIL_HEADERS
    ${PUBLIC_UTIL_HEADERS}
    src/util/MacThemeHelper.h
)
    set_source_files_properties(src/ApplicationSettingsAction.cpp src/util/MacThemeHelper.mm PROPERTIES
      SKIP_UNITY_BUILD_INCLUSION ON
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
    src/util/Interpolation.cpp
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
    src/util/FileDownloader.cpp
    src/util/ShortcutMap.cpp
    src/util/Notification.cpp
    src/util/Notifications.cpp
    src/util/LearningCenterVideo.cpp
    src/util/LearningCenterTutorial.cpp
    src/util/StyledIconCommon.cpp
    src/util/StyledIcon.cpp
    src/util/StyledIconEngine.cpp
    src/util/Badge.cpp
    src/util/ColorScheme.cpp
    src/util/ProjectDatabaseProject.cpp
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
    src/ApplicationStartupTask.h
)

set(PUBLIC_APPLICATION_SOURCES
    src/Application.cpp
    src/ApplicationStartupTask.cpp
)

set(PUBLIC_APPLICATION_FILES
    ${PUBLIC_APPLICATION_HEADERS}
    ${PUBLIC_APPLICATION_SOURCES}
)

set(PUBLIC_PROJECT_HEADERS
    src/Project.h
    src/ProjectMetaAction.h
    src/ProjectCompressionAction.h
    src/ProjectSerializationTask.h
)

set(PUBLIC_PROJECT_SOURCES
    src/Project.cpp
    src/ProjectMetaAction.cpp
    src/ProjectCompressionAction.cpp
    src/ProjectSerializationTask.cpp
)

set(PUBLIC_PROJECT_FILES
    ${PUBLIC_PROJECT_HEADERS}
    ${PUBLIC_PROJECT_SOURCES}
)

set(PUBLIC_WORKSPACE_HEADERS
    src/Workspace.h
)

set(PUBLIC_WORKSPACE_SOURCES
    src/Workspace.cpp
)

set(PUBLIC_WORKSPACE_FILES
    ${PUBLIC_WORKSPACE_HEADERS}
    ${PUBLIC_WORKSPACE_SOURCES}
)

set(PUBLIC_DATASET_HEADERS
    src/DataType.h
    src/RawData.h
    src/Set.h
    src/LinkedData.h
    src/SelectionGroup.h
    src/Dataset.h
    src/DatasetPrivate.h
    src/DatasetsMimeData.h
)

set(PUBLIC_DATASET_SOURCES
    src/Set.cpp
    src/LinkedData.cpp
    src/SelectionGroup.cpp
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
    src/PluginMetadata.h
    src/LoaderPlugin.h
    src/WriterPlugin.h
    src/AnalysisPlugin.h
    src/TransformationPlugin.h
    src/ViewPlugin.h
    src/PluginShortcuts.h
)

set(PUBLIC_PLUGIN_SOURCES
    src/Plugin.cpp
    src/PluginType.cpp
    src/PluginFactory.cpp
    src/PluginMetadata.cpp
    src/LoaderPlugin.cpp
    src/WriterPlugin.cpp
    src/AnalysisPlugin.cpp
    src/TransformationPlugin.cpp
    src/ViewPlugin.cpp
    src/PluginShortcuts.cpp
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

set(PUBLIC_COMMON_MODEL_HEADERS
    src/models/StandardItemModel.h
    src/models/SortFilterProxyModel.h
    src/models/NumberOfRowsAction.h
)

set(PUBLIC_COMMON_MODEL_SOURCES
    src/models/StandardItemModel.cpp
    src/models/SortFilterProxyModel.cpp
    src/models/NumberOfRowsAction.cpp
)

set(PUBLIC_COMMON_MODEL_FILES
    ${PUBLIC_COMMON_MODEL_HEADERS}
    ${PUBLIC_COMMON_MODEL_SOURCES}
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
    src/models/AbstractDataHierarchyModel.h
    src/models/DataHierarchyTreeModel.h
    src/models/DataHierarchyListModel.h
    src/models/DataHierarchyFilterModel.h
)

set(PUBLIC_DATA_HIERARCHY_MODEL_SOURCES
    src/models/AbstractDataHierarchyModel.cpp
    src/models/DataHierarchyTreeModel.cpp
    src/models/DataHierarchyListModel.cpp
    src/models/DataHierarchyFilterModel.cpp
)

set(PUBLIC_DATA_HIERARCHY_MODEL_FILES
    ${PUBLIC_DATA_HIERARCHY_MODEL_HEADERS}
    ${PUBLIC_DATA_HIERARCHY_MODEL_SOURCES}
)

set(PUBLIC_RAW_DATA_MODEL_HEADERS
    src/models/RawDataModel.h
    src/models/RawDataFilterModel.h
)

set(PUBLIC_RAW_DATA_MODEL_SOURCES
    src/models/RawDataModel.cpp
    src/models/RawDataFilterModel.cpp
)

set(PUBLIC_RAW_DATA_MODEL_FILES
    ${PUBLIC_RAW_DATA_MODEL_HEADERS}
    ${PUBLIC_RAW_DATA_MODEL_SOURCES}
)

set(PUBLIC_SELECTIONS_MODEL_HEADERS
    src/models/SelectionsModel.h
    src/models/SelectionsFilterModel.h
)

set(PUBLIC_SELECTIONS_MODEL_SOURCES
    src/models/SelectionsModel.cpp
    src/models/SelectionsFilterModel.cpp
)

set(PUBLIC_SELECTIONS_MODEL_FILES
    ${PUBLIC_SELECTIONS_MODEL_HEADERS}
    ${PUBLIC_SELECTIONS_MODEL_SOURCES}
)

set(PUBLIC_LOGGING_MODEL_HEADERS
    src/models/LoggingModel.h
    src/models/LoggingFilterModel.h
)

set(PUBLIC_LOGGING_MODEL_SOURCES
    src/models/LoggingModel.cpp
    src/models/LoggingFilterModel.cpp
)

set(PUBLIC_LOGGING_MODEL_FILES
    ${PUBLIC_LOGGING_MODEL_HEADERS}
    ${PUBLIC_LOGGING_MODEL_SOURCES}
)

set(PUBLIC_TASKS_MODEL_HEADERS
    src/models/AbstractTasksModel.h
    src/models/TasksTreeModel.h
    src/models/TasksListModel.h
    src/models/TasksFilterModel.h
)

set(PUBLIC_TASKS_MODEL_SOURCES
    src/models/AbstractTasksModel.cpp
    src/models/TasksTreeModel.cpp
    src/models/TasksListModel.cpp
    src/models/TasksFilterModel.cpp
)

set(PUBLIC_TASKS_MODEL_FILES
    ${PUBLIC_TASKS_MODEL_HEADERS}
    ${PUBLIC_TASKS_MODEL_SOURCES}
)

set(PUBLIC_PLUGINS_MODEL_HEADERS
    src/models/AbstractPluginsModel.h
    src/models/PluginsTreeModel.h
    src/models/PluginsListModel.h
    src/models/PluginsFilterModel.h
)

set(PUBLIC_PLUGINS_MODEL_SOURCES
    src/models/AbstractPluginsModel.cpp
    src/models/PluginsTreeModel.cpp
    src/models/PluginsListModel.cpp
    src/models/PluginsFilterModel.cpp
)

set(PUBLIC_PLUGINS_MODEL_FILES
    ${PUBLIC_PLUGINS_MODEL_HEADERS}
    ${PUBLIC_PLUGINS_MODEL_SOURCES}
)

set(PUBLIC_PLUGIN_FACTORIES_MODEL_HEADERS
    src/models/AbstractPluginFactoriesModel.h
    src/models/PluginFactoriesListModel.h
    src/models/PluginFactoriesTreeModel.h
    src/models/PluginFactoriesFilterModel.h
)

set(PUBLIC_PLUGIN_FACTORIES_MODEL_SOURCES
    src/models/AbstractPluginFactoriesModel.cpp
    src/models/PluginFactoriesListModel.cpp
    src/models/PluginFactoriesTreeModel.cpp
    src/models/PluginFactoriesFilterModel.cpp
)

set(PUBLIC_PLUGIN_FACTORIES_MODEL_FILES
    ${PUBLIC_PLUGIN_FACTORIES_MODEL_HEADERS}
    ${PUBLIC_PLUGIN_FACTORIES_MODEL_SOURCES}
)

set(PUBLIC_MISCELLANEOUS_MODEL_HEADERS
    src/models/CheckableStringListModel.h
)

set(PUBLIC_MISCELLANEOUS_MODEL_SOURCES
    src/models/CheckableStringListModel.cpp
)

set(PUBLIC_MISCELLANEOUS_MODEL_FILES
    ${PUBLIC_MISCELLANEOUS_MODEL_HEADERS}
    ${PUBLIC_MISCELLANEOUS_MODEL_SOURCES}
)

set(PUBLIC_DATASETS_MODEL_HEADERS
    src/models/AbstractDatasetsModel.h
    src/models/DatasetsListModel.h
    src/models/DatasetsFilterModel.h
)

set(PUBLIC_DATASETS_MODEL_SOURCES
    src/models/AbstractDatasetsModel.cpp
    src/models/DatasetsListModel.cpp
    src/models/DatasetsFilterModel.cpp
)

set(PUBLIC_DATASETS_MODEL_FILES
    ${PUBLIC_DATASETS_MODEL_HEADERS}
    ${PUBLIC_DATASETS_MODEL_SOURCES}
)

set(PUBLIC_LEARNING_CENTER_VIDEOS_MODEL_HEADERS
    src/models/LearningCenterVideosModel.h
	src/models/LearningCenterVideosFilterModel.h
)

set(PUBLIC_LEARNING_CENTER_VIDEOS_MODEL_SOURCES
    src/models/LearningCenterVideosModel.cpp
	src/models/LearningCenterVideosFilterModel.cpp
)

set(PUBLIC_LEARNING_CENTER_VIDEOS_MODEL_FILES
    ${PUBLIC_LEARNING_CENTER_VIDEOS_MODEL_HEADERS}
    ${PUBLIC_LEARNING_CENTER_VIDEOS_MODEL_SOURCES}
)

set(PUBLIC_LEARNING_CENTER_TUTORIALS_MODEL_HEADERS
    src/models/LearningCenterTutorialsModel.h
	src/models/LearningCenterTutorialsFilterModel.h
)

set(PUBLIC_LEARNING_CENTER_TUTORIALS_MODEL_SOURCES
    src/models/LearningCenterTutorialsModel.cpp
	src/models/LearningCenterTutorialsFilterModel.cpp
)

set(PUBLIC_LEARNING_CENTER_TUTORIALS_MODEL_FILES
    ${PUBLIC_LEARNING_CENTER_TUTORIALS_MODEL_HEADERS}
    ${PUBLIC_LEARNING_CENTER_TUTORIALS_MODEL_SOURCES}
)

set(PUBLIC_PROJECT_DATABASE_MODEL_HEADERS
    src/models/ProjectDatabaseModel.h
	src/models/ProjectDatabaseFilterModel.h
)

set(PUBLIC_PROJECT_DATABASE_MODEL_SOURCES
    src/models/ProjectDatabaseModel.cpp
	src/models/ProjectDatabaseFilterModel.cpp
)

set(PUBLIC_PROJECT_DATABASE_MODEL_FILES
    ${PUBLIC_PROJECT_DATABASE_MODEL_HEADERS}
    ${PUBLIC_PROJECT_DATABASE_MODEL_SOURCES}
)

set(PUBLIC_COLOR_SCHEMES_MODEL_HEADERS
    src/models/AbstractColorSchemesModel.h
	src/models/ColorSchemesListModel.h
    src/models/ColorSchemesFilterModel.h
)

set(PUBLIC_COLOR_SCHEMES_MODEL_SOURCES
    src/models/AbstractColorSchemesModel.cpp
	src/models/ColorSchemesListModel.cpp
    src/models/ColorSchemesFilterModel.cpp
)

set(PUBLIC_COLOR_SCHEMES_MODEL_FILES
    ${PUBLIC_COLOR_SCHEMES_MODEL_HEADERS}
    ${PUBLIC_COLOR_SCHEMES_MODEL_SOURCES}
)

set(PUBLIC_GLOBAL_SETTINGS_HEADERS
    src/GlobalSettingsGroupAction.h
    src/ParametersSettingsAction.h
    src/MiscellaneousSettingsAction.h
    src/TasksSettingsAction.h
    src/AppearanceSettingsAction.h
    src/TemporaryDirectoriesSettingsAction.h
	src/ErrorLoggingSettingsAction.h
    src/PluginGlobalSettingsGroupAction.h
)

set(PUBLIC_GLOBAL_SETTINGS_SOURCES
    src/GlobalSettingsGroupAction.cpp
    src/ParametersSettingsAction.cpp
    src/MiscellaneousSettingsAction.cpp
    src/TasksSettingsAction.cpp
    src/AppearanceSettingsAction.cpp
    src/TemporaryDirectoriesSettingsAction.cpp
    src/ErrorLoggingSettingsAction.cpp
    src/PluginGlobalSettingsGroupAction.cpp
)

set(PUBLIC_GLOBAL_SETTINGS_FILES
    ${PUBLIC_GLOBAL_SETTINGS_HEADERS}
    ${PUBLIC_GLOBAL_SETTINGS_SOURCES}
)

set(PUBLIC_TASK_HEADERS
    src/Task.h
    src/TaskTesterRunner.h
    src/AbstractTaskHandler.h
    src/AbstractTaskTester.h
    src/ModalTask.h
    src/ModalTaskTester.h
    src/ModalTaskHandler.h
    src/BackgroundTask.h
    src/BackgroundTaskTester.h
    src/BackgroundTaskHandler.h
    src/ForegroundTask.h
    src/ForegroundTaskTester.h
    src/ForegroundTaskHandler.h
    src/DatasetTask.h
    src/DatasetTaskHandler.h
)

set(PUBLIC_TASK_SOURCES
    src/Task.cpp
    src/TaskTesterRunner.cpp
    src/AbstractTaskHandler.cpp
    src/AbstractTaskTester.cpp
    src/ModalTask.cpp
    src/ModalTaskTester.cpp
    src/ModalTaskHandler.cpp
    src/BackgroundTask.cpp
    src/BackgroundTaskTester.cpp
    src/BackgroundTaskHandler.cpp
    src/ForegroundTask.cpp
    src/ForegroundTaskTester.cpp
    src/ForegroundTaskHandler.cpp
    src/DatasetTask.cpp
    src/DatasetTaskHandler.cpp
)

set(PUBLIC_TASK_FILES
    ${PUBLIC_TASK_HEADERS}
    ${PUBLIC_TASK_SOURCES}
)

# Automatically generated during cmake config
set(PUBLIC_VERSION_HEADERS
    src/ManiVaultVersion.h
)

set(PUBLIC_GLOBALS_HEADERS
    src/ManiVaultGlobals.h
)

set(PUBLIC_HEADERS
    ${PUBLIC_CORE_INTERFACE_HEADERS}
    ${PUBLIC_EVENT_HEADERS}
    ${PUBLIC_COLOR_MAP_ACTION_HEADERS}
    ${PUBLIC_NUMERICAL_ACTIONS_HEADERS}
    ${PUBLIC_TEXTUAL_ACTIONS_HEADERS}
    ${PUBLIC_GROUPING_ACTIONS_HEADERS}
    ${PUBLIC_TRIGGER_ACTIONS_HEADERS}
    ${PUBLIC_OPTION_ACTIONS_HEADERS}
    ${PUBLIC_COLOR_ACTIONS_HEADERS}
    ${PUBLIC_TOGGLE_ACTIONS_HEADERS}
    ${PUBLIC_FILE_ACTIONS_HEADERS}
    ${PUBLIC_TOOLBAR_ACTIONS_HEADERS}
    ${PUBLIC_MISCELLANEOUS_ACTIONS_HEADERS}
    ${PUBLIC_TASK_ACTIONS_HEADERS}
    ${PUBLIC_ACTIONS_INTERNAL_HEADERS}
    ${PUBLIC_WIDGETS_HEADERS}
    ${PUBLIC_WIDGETS_INTERNAL_HEADERS}
    ${PUBLIC_RENDERERS_HEADERS}
    ${PUBLIC_GRAPHICS_HEADERS}
    ${PUBLIC_UTIL_HEADERS}
    ${PUBLIC_APPLICATION_HEADERS}
    ${PUBLIC_PROJECT_HEADERS}
    ${PUBLIC_WORKSPACE_HEADERS}
    ${PUBLIC_DATASET_HEADERS}
    ${PUBLIC_PLUGIN_HEADERS}
    ${PUBLIC_DATA_HIERARCHY_HEADERS}
	${PUBLIC_COMMON_MODEL_HEADERS}
    ${PUBLIC_ACTIONS_MODEL_HEADERS}
    ${PUBLIC_DATA_HIERARCHY_MODEL_HEADERS}
    ${PUBLIC_RAW_DATA_MODEL_HEADERS}
    ${PUBLIC_SELECTIONS_MODEL_HEADERS}
    ${PUBLIC_LOGGING_MODEL_HEADERS}
    ${PUBLIC_TASKS_MODEL_HEADERS}
    ${PUBLIC_PLUGINS_MODEL_HEADERS}
    ${PUBLIC_PLUGIN_FACTORIES_MODEL_HEADERS}
    ${PUBLIC_MISCELLANEOUS_MODEL_HEADERS}
	${PUBLIC_DATASETS_MODEL_HEADERS}
	${PUBLIC_LEARNING_CENTER_VIDEOS_MODEL_HEADERS}
	${PUBLIC_LEARNING_CENTER_TUTORIALS_MODEL_HEADERS}
	${PUBLIC_PROJECT_DATABASE_MODEL_HEADERS}
    ${PUBLIC_COLOR_SCHEMES_MODEL_HEADERS}
    ${PUBLIC_GLOBAL_SETTINGS_HEADERS}
    ${PUBLIC_TASK_HEADERS}
    ${PUBLIC_NOTIFICATIONS_HEADERS}
    ${PUBLIC_VERSION_HEADERS}
    ${PUBLIC_GLOBALS_HEADERS}
)

set(PUBLIC_SOURCES
    ${PUBLIC_EVENT_SOURCES}
    ${PUBLIC_COLOR_MAP_ACTION_SOURCES}
    ${PUBLIC_NUMERICAL_ACTIONS_SOURCES}
    ${PUBLIC_TEXTUAL_ACTIONS_SOURCES}
    ${PUBLIC_GROUPING_ACTIONS_SOURCES}
    ${PUBLIC_TRIGGER_ACTIONS_SOURCES}
    ${PUBLIC_OPTION_ACTIONS_SOURCES}
    ${PUBLIC_COLOR_ACTIONS_SOURCES}
    ${PUBLIC_TOGGLE_ACTIONS_SOURCES}
    ${PUBLIC_FILE_ACTIONS_SOURCES}
    ${PUBLIC_TOOLBAR_ACTIONS_SOURCES}
    ${PUBLIC_MISCELLANEOUS_ACTIONS_SOURCES}
    ${PUBLIC_TASK_ACTIONS_SOURCES}
    ${PUBLIC_ACTIONS_INTERNAL_SOURCES}
    ${PUBLIC_WIDGETS_SOURCES}
    ${PUBLIC_WIDGETS_INTERNAL_SOURCES}
    ${PUBLIC_RENDERERS_SOURCES}
    ${PUBLIC_GRAPHICS_SOURCES}
    ${PUBLIC_UTIL_SOURCES}
    ${PUBLIC_APPLICATION_SOURCES}
    ${PUBLIC_PROJECT_SOURCES}
    ${PUBLIC_WORKSPACE_SOURCES}
    ${PUBLIC_DATASET_SOURCES}
    ${PUBLIC_PLUGIN_SOURCES}
    ${PUBLIC_DATA_HIERARCHY_SOURCES}
	${PUBLIC_COMMON_MODEL_SOURCES}
    ${PUBLIC_ACTIONS_MODEL_SOURCES}
    ${PUBLIC_DATA_HIERARCHY_MODEL_SOURCES}
    ${PUBLIC_RAW_DATA_MODEL_SOURCES}
    ${PUBLIC_SELECTIONS_MODEL_SOURCES}
    ${PUBLIC_LOGGING_MODEL_SOURCES}
    ${PUBLIC_TASKS_MODEL_SOURCES}
    ${PUBLIC_PLUGINS_MODEL_SOURCES}
    ${PUBLIC_PLUGIN_FACTORIES_MODEL_SOURCES}
    ${PUBLIC_MISCELLANEOUS_MODEL_SOURCES}
	${PUBLIC_DATASETS_MODEL_SOURCES}
	${PUBLIC_LEARNING_CENTER_VIDEOS_MODEL_SOURCES}
	${PUBLIC_LEARNING_CENTER_TUTORIALS_MODEL_SOURCES}
	${PUBLIC_PROJECT_DATABASE_MODEL_SOURCES}
    ${PUBLIC_COLOR_SCHEMES_MODEL_SOURCES}
    ${PUBLIC_GLOBAL_SETTINGS_SOURCES}
    ${PUBLIC_TASK_SOURCES}
    ${PUBLIC_NOTIFICATIONS_SOURCES}
    ${PUBLIC_HEADERS}
)

set(PRECOMPILE_HEADERS
    src/actions/Actions.h
    src/event/EventListener.h
    src/AbstractManager.h
    src/Task.h
    src/BackgroundTask.h
    src/ForegroundTask.h
    src/ModalTask.h
    ${PUBLIC_UTIL_HEADERS}
)

list(REMOVE_DUPLICATES PUBLIC_HEADERS)
list(REMOVE_DUPLICATES PUBLIC_SOURCES)

source_group(CoreInterface FILES ${PUBLIC_CORE_INTERFACE_FILES})
source_group(Event FILES ${PUBLIC_EVENT_FILES})
source_group(Actions\\Colormap FILES ${PUBLIC_COLOR_MAP_ACTION_FILES})
source_group(Actions\\Numerical FILES ${PUBLIC_NUMERICAL_ACTIONS_FILES})
source_group(Actions\\Textual FILES ${PUBLIC_TEXTUAL_ACTIONS_FILES})
source_group(Actions\\Grouping FILES ${PUBLIC_GROUPING_ACTIONS_FILES})
source_group(Actions\\Trigger FILES ${PUBLIC_TRIGGER_ACTIONS_FILES})
source_group(Actions\\Option FILES ${PUBLIC_OPTION_ACTIONS_FILES})
source_group(Actions\\Color FILES ${PUBLIC_COLOR_ACTIONS_FILES})
source_group(Actions\\Toggle FILES ${PUBLIC_TOGGLE_ACTIONS_FILES})
source_group(Actions\\File FILES ${PUBLIC_FILE_ACTIONS_FILES})
source_group(Actions\\Toolbar FILES ${PUBLIC_TOOLBAR_ACTIONS_FILES})
source_group(Actions\\Miscellaneous FILES ${PUBLIC_MISCELLANEOUS_ACTIONS_FILES})
source_group(Actions\\Task FILES ${PUBLIC_TASK_ACTIONS_FILES})
source_group(Actions\\Internal FILES ${PUBLIC_ACTIONS_INTERNAL_FILES})
source_group(Widgets FILES ${PUBLIC_WIDGETS_FILES})
source_group(Widgets\\Internal FILES ${PUBLIC_WIDGETS_INTERNAL_FILES})
source_group(Renderers FILES ${PUBLIC_RENDERERS_FILES})
source_group(Graphics FILES ${PUBLIC_GRAPHICS_FILES})
source_group(Util FILES ${PUBLIC_UTIL_FILES})
source_group(Application FILES ${PUBLIC_APPLICATION_FILES} ${PUBLIC_GLOBALS_HEADERS} ${PUBLIC_VERSION_HEADERS})
source_group(Project FILES ${PUBLIC_PROJECT_FILES})
source_group(Workspace FILES ${PUBLIC_WORKSPACE_FILES})
source_group(DataHierarchy FILES ${PUBLIC_DATA_HIERARCHY_FILES})
source_group(Dataset FILES ${PUBLIC_DATASET_FILES})
source_group(Plugin FILES ${PUBLIC_PLUGIN_FILES})
source_group(Models\\Common FILES ${PUBLIC_COMMON_MODEL_FILES})
source_group(Models\\Actions FILES ${PUBLIC_ACTIONS_MODEL_FILES})
source_group(Models\\DataHierarchy FILES ${PUBLIC_DATA_HIERARCHY_MODEL_FILES})
source_group(Models\\RawData FILES ${PUBLIC_RAW_DATA_MODEL_FILES})
source_group(Models\\Selections FILES ${PUBLIC_SELECTIONS_MODEL_FILES})
source_group(Models\\Logging FILES ${PUBLIC_LOGGING_MODEL_FILES})
source_group(Models\\Tasks FILES ${PUBLIC_TASKS_MODEL_FILES})
source_group(Models\\Plugins FILES ${PUBLIC_PLUGINS_MODEL_FILES})
source_group(Models\\PluginFactories FILES ${PUBLIC_PLUGIN_FACTORIES_MODEL_FILES})
source_group(Models\\Miscellaneous FILES ${PUBLIC_MISCELLANEOUS_MODEL_FILES})
source_group(Models\\Datasets FILES ${PUBLIC_DATASETS_MODEL_FILES})
source_group(Models\\LearningCenter\\Videos FILES ${PUBLIC_LEARNING_CENTER_VIDEOS_MODEL_FILES})
source_group(Models\\LearningCenter\\Tutorials FILES ${PUBLIC_LEARNING_CENTER_TUTORIALS_MODEL_FILES})
source_group(Models\\ProjectDatabase FILES ${PUBLIC_PROJECT_DATABASE_MODEL_FILES})
source_group(Models\\ColorSchemes FILES ${PUBLIC_COLOR_SCHEMES_MODEL_FILES})
source_group(GlobalSettings FILES ${PUBLIC_GLOBAL_SETTINGS_FILES})
source_group(Task FILES ${PUBLIC_TASK_FILES})
source_group(Notifications FILES ${PUBLIC_NOTIFICATIONS_FILES})
