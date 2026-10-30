# -----------------------------------------------------------------------------
# Source files for target MV_PUBLIC_LIB
# -----------------------------------------------------------------------------
# defines PUBLIC_SOURCES and PUBLIC_HEADERS
# all file path are relative to ROOT/src

set(PUBLIC_CORE_INTERFACE_HEADERS
    CoreInterface.h
    AbstractManager.h
    AbstractDataManager.h
    AbstractPluginManager.h
    AbstractActionsManager.h
    AbstractDataHierarchyManager.h
    AbstractEventManager.h
    AbstractWorkspaceManager.h
    AbstractProjectManager.h
    AbstractSettingsManager.h
    AbstractTaskManager.h
    AbstractHelpManager.h
    AbstractErrorManager.h
    AbstractErrorLogger.h
    AbstractThemeManager.h
	AbstractScriptingManager.h
)

set(PUBLIC_CORE_INTERFACE_SOURCES
    CoreInterface.cpp
)

set(PUBLIC_CORE_INTERFACE_FILES
    ${PUBLIC_CORE_INTERFACE_HEADERS}
    ${PUBLIC_CORE_INTERFACE_SOURCES}
)

set(PUBLIC_EVENT_HEADERS
    event/Event.h
    event/EventListener.h
)

set(PUBLIC_EVENT_SOURCES
    event/Event.cpp
    event/EventListener.cpp
)

set(PUBLIC_EVENT_FILES
    ${PUBLIC_EVENT_HEADERS}
    ${PUBLIC_EVENT_SOURCES}
)

set(PUBLIC_COLOR_MAP_ACTION_HEADERS
    actions/ColorMapAction.h
    actions/ColorMap1DAction.h
    actions/ColorMap2DAction.h
    actions/ColorMapEditor1DAction.h
    actions/ColorMapEditor1DEdgesGraphicsItem.h
    actions/ColorMapEditor1DHistogramGraphicsItem.h
    actions/ColorMapEditor1DNode.h
    actions/ColorMapEditor1DNodeGraphicsItem.h
    actions/ColorMapEditor1DNodeAction.h
    actions/ColorMapEditor1DScene.h
    actions/ColorMapEditor1DWidget.h
    actions/ColorMapViewAction.h
    actions/ColorMapSettingsAction.h
    actions/ColorMapSettings1DAction.h
    actions/ColorMapSettings2DAction.h
)

set(PUBLIC_COLOR_MAP_ACTION_SOURCES
    actions/ColorMapAction.cpp
    actions/ColorMap1DAction.cpp
    actions/ColorMap2DAction.cpp
    actions/ColorMapEditor1DAction.cpp
    actions/ColorMapEditor1DEdgesGraphicsItem.cpp
    actions/ColorMapEditor1DHistogramGraphicsItem.cpp
    actions/ColorMapEditor1DNode.cpp
    actions/ColorMapEditor1DNodeGraphicsItem.cpp
    actions/ColorMapEditor1DNodeAction.cpp
    actions/ColorMapEditor1DScene.cpp
    actions/ColorMapEditor1DWidget.cpp
    actions/ColorMapViewAction.cpp
    actions/ColorMapSettingsAction.cpp
    actions/ColorMapSettings1DAction.cpp
    actions/ColorMapSettings2DAction.cpp
)

set(PUBLIC_COLOR_MAP_ACTION_FILES
    ${PUBLIC_COLOR_MAP_ACTION_HEADERS}    
    ${PUBLIC_COLOR_MAP_ACTION_SOURCES}
)

set(PUBLIC_NUMERICAL_ACTIONS_HEADERS
    actions/NumericalAction.h
    actions/DecimalAction.h
    actions/IntegralAction.h
    actions/NumericalRangeAction.h
    actions/DecimalRangeAction.h
    actions/IntegralRangeAction.h
    actions/NumericalPointAction.h
    actions/DecimalPointAction.h
    actions/IntegralPointAction.h
	actions/RectangleAction.h
    actions/IntegralRectangleAction.h
    actions/DecimalRectangleAction.h
)

set(PUBLIC_NUMERICAL_ACTIONS_SOURCES
    actions/NumericalAction.cpp
    actions/DecimalAction.cpp
    actions/IntegralAction.cpp
    actions/NumericalRangeAction.cpp
    actions/DecimalRangeAction.cpp
    actions/IntegralRangeAction.cpp
	actions/NumericalPointAction.cpp
    actions/DecimalPointAction.cpp
    actions/IntegralPointAction.cpp
	actions/RectangleAction.cpp
    actions/IntegralRectangleAction.cpp
    actions/DecimalRectangleAction.cpp
)

set(PUBLIC_NUMERICAL_ACTIONS_FILES
    ${PUBLIC_NUMERICAL_ACTIONS_HEADERS}    
    ${PUBLIC_NUMERICAL_ACTIONS_SOURCES}
)

set(PUBLIC_TEXTUAL_ACTIONS_HEADERS
    actions/StringAction.h
    actions/StringsAction.h
)

set(PUBLIC_TEXTUAL_ACTIONS_SOURCES
    actions/StringAction.cpp
    actions/StringsAction.cpp
)

set(PUBLIC_TEXTUAL_ACTIONS_FILES
    ${PUBLIC_TEXTUAL_ACTIONS_HEADERS}    
    ${PUBLIC_TEXTUAL_ACTIONS_SOURCES}
)

set(PUBLIC_GROUPING_ACTIONS_HEADERS
    actions/GroupAction.h
    actions/StretchAction.h
    actions/HorizontalGroupAction.h
    actions/VerticalGroupAction.h
    actions/GroupsAction.h
    actions/GroupSectionTreeItem.h
    actions/GroupWidgetTreeItem.h
    actions/ToolbarAction.h
    actions/HorizontalToolbarAction.h
    actions/VerticalToolbarAction.h
    actions/ToolbarActionItem.h
    actions/ToolbarActionItemWidget.h
)

set(PUBLIC_GROUPING_ACTIONS_SOURCES
    actions/GroupAction.cpp
    actions/StretchAction.cpp
    actions/HorizontalGroupAction.cpp
    actions/VerticalGroupAction.cpp
    actions/GroupsAction.cpp
    actions/GroupSectionTreeItem.cpp
    actions/GroupWidgetTreeItem.cpp
    actions/ToolbarAction.cpp
    actions/HorizontalToolbarAction.cpp
    actions/VerticalToolbarAction.cpp
    actions/ToolbarActionItem.cpp
    actions/ToolbarActionItemWidget.cpp
)

set(PUBLIC_GROUPING_ACTIONS_FILES
    ${PUBLIC_GROUPING_ACTIONS_HEADERS}    
    ${PUBLIC_GROUPING_ACTIONS_SOURCES}
)

set(PUBLIC_TRIGGER_ACTIONS_HEADERS
    actions/TriggerAction.h
    actions/TriggersAction.h
)

set(PUBLIC_TRIGGER_ACTIONS_SOURCES
    actions/TriggerAction.cpp
    actions/TriggersAction.cpp
)

set(PUBLIC_TRIGGER_ACTIONS_FILES
    ${PUBLIC_TRIGGER_ACTIONS_HEADERS}    
    ${PUBLIC_TRIGGER_ACTIONS_SOURCES}
)

set(PUBLIC_OPTION_ACTIONS_HEADERS
    actions/OptionAction.h
    actions/OptionsAction.h
)

set(PUBLIC_OPTION_ACTIONS_SOURCES
    actions/OptionAction.cpp
    actions/OptionsAction.cpp
)

set(PUBLIC_OPTION_ACTIONS_FILES
    ${PUBLIC_OPTION_ACTIONS_HEADERS}    
    ${PUBLIC_OPTION_ACTIONS_SOURCES}
)

set(PUBLIC_COLOR_ACTIONS_HEADERS
    actions/ColorAction.h
    actions/ColorPickerAction.h
)

set(PUBLIC_COLOR_ACTIONS_SOURCES
    actions/ColorAction.cpp
    actions/ColorPickerAction.cpp
)

set(PUBLIC_COLOR_ACTIONS_FILES
    ${PUBLIC_COLOR_ACTIONS_HEADERS}    
    ${PUBLIC_COLOR_ACTIONS_SOURCES}
)

set(PUBLIC_TOGGLE_ACTIONS_HEADERS
    actions/ToggleAction.h
)

set(PUBLIC_TOGGLE_ACTIONS_SOURCES
    actions/ToggleAction.cpp
)

set(PUBLIC_TOGGLE_ACTIONS_FILES
    ${PUBLIC_TOGGLE_ACTIONS_HEADERS}    
    ${PUBLIC_TOGGLE_ACTIONS_SOURCES}
)

set(PUBLIC_FILE_ACTIONS_HEADERS
    actions/DirectoryPickerAction.h
    actions/FilePickerAction.h
    actions/file/TemporaryDirectoryAction.h
)

set(PUBLIC_FILE_ACTIONS_SOURCES
    actions/DirectoryPickerAction.cpp
    actions/FilePickerAction.cpp
    actions/file/TemporaryDirectoryAction.cpp
)

set(PUBLIC_FILE_ACTIONS_FILES
    ${PUBLIC_FILE_ACTIONS_HEADERS}    
    ${PUBLIC_FILE_ACTIONS_SOURCES}
)

set(PUBLIC_TOOLBAR_ACTIONS_HEADERS
    actions/ToolbarAction.h
    actions/ToolbarActionItem.h
    actions/ToolbarActionItemWidget.h
)

set(PUBLIC_TOOLBAR_ACTIONS_SOURCES
    actions/ToolbarAction.cpp
    actions/ToolbarActionItem.cpp
    actions/ToolbarActionItemWidget.cpp
)

set(PUBLIC_TOOLBAR_ACTIONS_FILES
    ${PUBLIC_TOOLBAR_ACTIONS_HEADERS}    
    ${PUBLIC_TOOLBAR_ACTIONS_SOURCES}
)

set(PUBLIC_MISCELLANEOUS_ACTIONS_HEADERS
    actions/Actions.h
    actions/DatasetPickerAction.h
    actions/PluginPickerAction.h
    actions/ImageAction.h
    actions/PixelSelectionAction.h
    actions/WindowLevelAction.h
    actions/VariantAction.h
    actions/StatusAction.h
    actions/VersionAction.h
    actions/TreeAction.h
    actions/TableAction.h
    actions/StatusBarAction.h
    actions/PluginStatusBarAction.h
    actions/EventAction.h
    actions/CodecSettingsAction.h
    actions/MessageCardAction.h
)

set(PUBLIC_MISCELLANEOUS_ACTIONS_SOURCES
    actions/DatasetPickerAction.cpp
    actions/PluginPickerAction.cpp
    actions/ImageAction.cpp
    actions/PixelSelectionAction.cpp
    actions/WindowLevelAction.cpp
    actions/VariantAction.cpp
    actions/StatusAction.cpp
    actions/VersionAction.cpp
    actions/TreeAction.cpp
    actions/TableAction.cpp
    actions/StatusBarAction.cpp
    actions/PluginStatusBarAction.cpp
    actions/EventAction.cpp
    actions/CodecSettingsAction.cpp
    actions/MessageCardAction.cpp
)

set(PUBLIC_MISCELLANEOUS_ACTIONS_FILES
    ${PUBLIC_MISCELLANEOUS_ACTIONS_HEADERS}    
    ${PUBLIC_MISCELLANEOUS_ACTIONS_SOURCES}
)

set(PUBLIC_TASK_ACTIONS_HEADERS
    actions/ProgressAction.h
    actions/TaskAction.h
)

set(PUBLIC_TASK_ACTIONS_SOURCES
    actions/ProgressAction.cpp
    actions/TaskAction.cpp   
)

set(PUBLIC_TASK_ACTIONS_FILES
    ${PUBLIC_TASK_ACTIONS_HEADERS}    
    ${PUBLIC_TASK_ACTIONS_SOURCES}
)

set(PUBLIC_ACTIONS_INTERNAL_HEADERS
    actions/WidgetAction.h
    actions/WidgetActionWidget.h
    actions/WidgetActionCollapsedWidget.h
    actions/WidgetActionLabel.h
    actions/WidgetActionContextMenu.h
    actions/WidgetActionViewWidget.h
    actions/WidgetActionMimeData.h
    actions/WidgetActionHighlightWidget.h
    actions/WidgetActionDrag.h
    actions/TasksTreeAction.h
    actions/TasksListAction.h
    actions/SplashScreenAction.h
    actions/RecentFilesAction.h
    actions/PresetsAction.h
    actions/LockingAction.h
    actions/PluginTriggerPickerAction.h
    actions/PluginTriggerAction.h
	actions/ScriptTriggerAction.h
    actions/LabelProxyAction.h
    actions/ModelFilterAction.h
    actions/ModelSelectionAction.h
    actions/HorizontalHeaderAction.h
    actions/WidgetActionBadgeOverlayWidget.h
    actions/WidgetActionToolButton.h
    actions/WidgetActionToolButtonMenu.h
    actions/IconAction.h
    actions/IconPickerAction.h
    actions/ApplicationIconAction.h
    actions/ViewPluginSamplerAction.h
    actions/PluginLearningCenterAction.h
    actions/WatchVideoAction.h
    actions/PaletteAction.h
    actions/PaletteColorAction.h
    actions/PaletteColorRoleAction.h
    actions/ColorSchemeAction.h
    actions/EditColorSchemeAction.h
    actions/NavigationAction.h
    actions/ZoomMarginAction.h
    actions/ViewPluginHeadsUpDisplayAction.h
    actions/ApplicationConfigurationAction.h
    actions/StartPageConfigurationAction.h
    actions/BrandingConfigurationAction.h
    actions/ProjectsConfigurationAction.h
)

set(PUBLIC_ACTIONS_INTERNAL_SOURCES
    actions/WidgetAction.cpp
    actions/WidgetActionWidget.cpp
    actions/WidgetActionCollapsedWidget.cpp
    actions/WidgetActionLabel.cpp
    actions/WidgetActionContextMenu.cpp
    actions/WidgetActionViewWidget.cpp
    actions/WidgetActionMimeData.cpp
    actions/WidgetActionHighlightWidget.cpp
    actions/WidgetActionDrag.cpp
    actions/TasksTreeAction.cpp
    actions/TasksListAction.cpp
    actions/SplashScreenAction.cpp
    actions/RecentFilesAction.cpp
    actions/PresetsAction.cpp
    actions/LockingAction.cpp
    actions/PluginTriggerPickerAction.cpp
    actions/PluginTriggerAction.cpp
	actions/ScriptTriggerAction.cpp
    actions/LabelProxyAction.cpp
	actions/ModelFilterAction.cpp
	actions/ModelSelectionAction.cpp
	actions/HorizontalHeaderAction.cpp
    actions/WidgetActionBadgeOverlayWidget.cpp
    actions/WidgetActionToolButton.cpp
    actions/WidgetActionToolButtonMenu.cpp
    actions/IconAction.cpp
    actions/IconPickerAction.cpp
    actions/ApplicationIconAction.cpp
    actions/ViewPluginSamplerAction.cpp
    actions/PluginLearningCenterAction.cpp
    actions/WatchVideoAction.cpp
    actions/PaletteAction.cpp
    actions/PaletteColorAction.cpp
    actions/PaletteColorRoleAction.cpp
    actions/ColorSchemeAction.cpp
    actions/EditColorSchemeAction.cpp
    actions/NavigationAction.cpp
    actions/ZoomMarginAction.cpp
    actions/ViewPluginHeadsUpDisplayAction.cpp
    actions/ApplicationConfigurationAction.cpp
    actions/StartPageConfigurationAction.cpp
    actions/BrandingConfigurationAction.cpp
    actions/ProjectsConfigurationAction.cpp
)

set(PUBLIC_ACTIONS_INTERNAL_FILES
    ${PUBLIC_ACTIONS_INTERNAL_HEADERS}    
    ${PUBLIC_ACTIONS_INTERNAL_SOURCES}
)

set(PUBLIC_WIDGETS_HEADERS
    widgets/WebWidget.h
    widgets/DropWidget.h
    widgets/OpenGLWidget.h
    widgets/Divider.h
    widgets/ElidedLabel.h
    widgets/FileDialog.h
)

set(PUBLIC_WIDGETS_SOURCES
    widgets/WebWidget.cpp
    widgets/DropWidget.cpp
    widgets/OpenGLWidget.cpp
    widgets/Divider.cpp
    widgets/ElidedLabel.cpp
    widgets/FileDialog.cpp
)

set(PUBLIC_WIDGETS_FILES
    ${PUBLIC_WIDGETS_HEADERS}
    ${PUBLIC_WIDGETS_SOURCES}
)

set(PUBLIC_WIDGETS_INTERNAL_HEADERS
    widgets/HierarchyWidget.h
    widgets/InfoOverlayWidget.h
    widgets/OverlayWidget.h
    widgets/ActionOverlayWidget.h
    widgets/InfoWidget.h
    widgets/ViewPluginEditorDialog.h
    widgets/ActionsWidget.h
    widgets/SplashScreenWidget.h
    widgets/FlowLayout.h
	widgets/MarkdownDocument.h
	widgets/MarkdownDialog.h
	widgets/YouTubeVideoDialog.h
	widgets/ViewPluginOverlayWidget.h
	widgets/PluginAboutDialog.h
	widgets/PluginShortcutsDialog.h
	widgets/ViewPluginLearningCenterOverlayWidget.h
	widgets/IconLabel.h
	widgets/MultiSelectComboBox.h
)

set(PUBLIC_WIDGETS_INTERNAL_SOURCES
    widgets/HierarchyWidget.cpp
    widgets/InfoOverlayWidget.cpp
    widgets/OverlayWidget.cpp
    widgets/ActionOverlayWidget.cpp
    widgets/InfoWidget.cpp
    widgets/ViewPluginEditorDialog.cpp
    widgets/ActionsWidget.cpp
    widgets/SplashScreenWidget.cpp
    widgets/FlowLayout.cpp
	widgets/MarkdownDocument.cpp
	widgets/MarkdownDialog.cpp
	widgets/YouTubeVideoDialog.cpp
	widgets/ViewPluginOverlayWidget.cpp
	widgets/PluginAboutDialog.cpp
	widgets/PluginShortcutsDialog.cpp
	widgets/ViewPluginLearningCenterOverlayWidget.cpp
	widgets/IconLabel.cpp
	widgets/MultiSelectComboBox.cpp
)

set(PUBLIC_WIDGETS_INTERNAL_FILES
    ${PUBLIC_WIDGETS_INTERNAL_HEADERS}
    ${PUBLIC_WIDGETS_INTERNAL_SOURCES}
)

set(PUBLIC_RENDERERS_HEADERS
    renderers/Renderer.h
    renderers/PointRenderer.h
    renderers/DensityRenderer.h
    renderers/Renderer2D.h
    renderers/Navigator2D.h
)

set(PUBLIC_RENDERERS_SOURCES
    renderers/PointRenderer.cpp
    renderers/DensityRenderer.cpp
    renderers/Renderer2D.cpp
    renderers/Navigator2D.cpp
)

set(PUBLIC_RENDERERS_FILES
    ${PUBLIC_RENDERERS_HEADERS}
    ${PUBLIC_RENDERERS_SOURCES}
)

set(PUBLIC_GRAPHICS_HEADERS
    graphics/BufferObject.h
    graphics/Vector2f.h
    graphics/Vector3f.h
    graphics/Matrix3f.h
    graphics/Selection.h
    graphics/Texture.h
    graphics/Shader.h
    graphics/Framebuffer.h
    graphics/OffscreenBuffer.h
    graphics/Bounds.h
)

set(PUBLIC_GRAPHICS_SOURCES
    graphics/BufferObject.cpp
    graphics/Shader.cpp
    graphics/Selection.cpp
    graphics/Matrix3f.cpp
    graphics/Vector2f.cpp
    graphics/Vector3f.cpp
    graphics/Bounds.cpp
)

set(PUBLIC_GRAPHICS_FILES
    ${PUBLIC_GRAPHICS_HEADERS}
    ${PUBLIC_GRAPHICS_SOURCES}
)

set(PUBLIC_UTIL_HEADERS
    util/Miscellaneous.h
    util/FileUtil.h
    util/MeanShift.h
    util/DensityComputation.h
    util/Exception.h
    util/Math.h
    util/Timer.h
    util/Icon.h
    util/Interpolation.h
    util/ColorMap.h
    util/ColorMapFilterModel.h
    util/ColorMapModel.h
    util/ColorSpace.h
    util/PixelSelectionTool.h
    util/PixelSelection.h
    util/Preset.h
    util/PresetsModel.h
    util/PresetsFilterModel.h
    util/WidgetFader.h
    util/WidgetOverlayer.h
    util/Serialization.h
    util/Serializable.h
    util/DockArea.h
    util/Logger.h
    util/AbstractItemModelTester.h
    util/Version.h
    util/DockWidgetPermission.h
    util/NumericalRange.h
    util/FileDownloader.h
    util/ShortcutMap.h
    util/Notification.h
    util/Notifications.h
    util/LearningCenterVideo.h
    util/LearningCenterTutorial.h
    util/StyledIconCommon.h
    util/StyledIcon.h
    util/StyledIconEngine.h
    util/Badge.h
    util/ColorScheme.h
    util/JSON.h
    util/Script.h
    util/RecentFile.h
    util/Model.h
    util/View.h
    util/SecureNetworkAccessManager.h
    util/HttpsErrorReply.h
    util/HeadsUpDisplayItem.h
    util/SplashScreenBridge.h
    util/StandardPaths.h
    util/CustomAssetsUrlSchemeHandler.h
    util/BlobCodec.h
    util/BlobCodecFactory.h
    util/CodecRegistry.h
    util/CodecActionBinding.h
    util/SeverityLevel.h
    util/StackFrame.h
    util/PresetSerializationScope.h
)

if(APPLE)
set(PUBLIC_UTIL_HEADERS
    ${PUBLIC_UTIL_HEADERS}
    util/MacThemeHelper.h
)
    set_source_files_properties(ApplicationSettingsAction.cpp util/MacThemeHelper.mm PROPERTIES
      SKIP_UNITY_BUILD_INCLUSION ON
    )
endif()

set(PUBLIC_UTIL_SOURCES
    util/Miscellaneous.cpp
    util/FileUtil.cpp
    util/MeanShift.cpp
    util/DensityComputation.cpp
    util/Exception.cpp
    util/Math.cpp
    util/Timer.cpp
    util/Icon.cpp
    util/Interpolation.cpp
    util/ColorMap.cpp
    util/ColorMapFilterModel.cpp
    util/ColorMapModel.cpp
    util/ColorSpace.cpp
    util/PixelSelectionTool.cpp
    util/PixelSelection.cpp
    util/Preset.cpp
    util/PresetsModel.cpp
    util/PresetsFilterModel.cpp
    util/WidgetFader.cpp
    util/WidgetOverlayer.cpp
    util/Serialization.cpp
    util/Serializable.cpp
    util/DockArea.cpp
    util/Logger.cpp
    util/AbstractItemModelTester.cpp
    util/Version.cpp
    util/DockWidgetPermission.cpp
    util/NumericalRange.cpp
    util/FileDownloader.cpp
    util/ShortcutMap.cpp
    util/Notification.cpp
    util/Notifications.cpp
    util/LearningCenterVideo.cpp
    util/LearningCenterTutorial.cpp
    util/StyledIconCommon.cpp
    util/StyledIcon.cpp
    util/StyledIconEngine.cpp
    util/Badge.cpp
    util/ColorScheme.cpp
    util/JSON.cpp
    util/Script.cpp
    util/RecentFile.cpp
	util/Model.cpp
	util/View.cpp
	util/SecureNetworkAccessManager.cpp
	util/HttpsErrorReply.cpp
	util/HeadsUpDisplayItem.cpp
	util/SplashScreenBridge.cpp
	util/StandardPaths.cpp
	util/CustomAssetsUrlSchemeHandler.cpp
	util/BlobCodec.cpp
    util/BlobCodecFactory.cpp
    util/CodecRegistry.cpp
    util/CodecActionBinding.cpp
    util/SeverityLevel.cpp
    util/PresetSerializationScope.cpp
)

if(APPLE)
set(PUBLIC_UTIL_SOURCES
    ${PUBLIC_UTIL_SOURCES}
    util/MacThemeHelper.mm
)
endif()

set(PUBLIC_UTIL_FILES
    ${PUBLIC_UTIL_HEADERS}    
    ${PUBLIC_UTIL_SOURCES}
)

set(PUBLIC_WORKFLOW_HEADERS
    workflow/AbstractWorkflowPlanExecutor.h
    workflow/WorkflowExecutionContext.h
    workflow/WorkflowContextBase.h
    workflow/WorkflowResultBase.h
    workflow/WorkflowJobResult.h
    workflow/WorkflowStageResult.h
    workflow/WorkflowResult.h
    workflow/WorkflowMessage.h
    workflow/WorkflowReportNode.h
    workflow/WorkflowProgressNode.h
    workflow/WorkflowExecutionState.h
    workflow/WorkflowResultFuture.h
    workflow/WorkflowGuiThreadDispatcher.h
    workflow/WorkflowPlan.h
    workflow/WorkflowMetric.h
    workflow/WorkflowExecutionMetrics.h
    workflow/WorkflowResultRegistry.h
    workflow/WorkflowResultDialog.h
    workflow/WorkflowConsoleFormatter.h
    workflow/WorkflowExecutionLifecycleScope.h
    workflow/WorkflowStageSummary.h
    workflow/WorkflowExecutionNodeType.h
    workflow/WorkflowConsoleDashboard.h
    workflow/WorkflowConsoleDashboardScope.h
    workflow/WorkflowRuntimeScoped.h
    workflow/WorkflowContextVariantMap.h
    workflow/WorkflowHandle.h
    workflow/WorkflowContextVariantMap.h
    workflow/WorkflowBatchingOptions.h
    workflow/WorkflowMessageDetailsDelegate.h
    workflow/WorkflowReportingOptions.h
    workflow/WorkflowProfilingOptions.h
    workflow/WorkflowParallelizationOptions.h
    workflow/WorkflowCancellationOptions.h
    workflow/WorkflowOptions.h
    workflow/WorkflowResultDetailsOptions.h
)

set(PUBLIC_WORKFLOW_SOURCES
    workflow/AbstractWorkflowPlanExecutor.cpp    
    workflow/WorkflowJobResult.cpp
    workflow/WorkflowStageResult.cpp
    workflow/WorkflowResult.cpp
    workflow/WorkflowExecutionContext.cpp
    workflow/WorkflowReportNode.cpp
    workflow/WorkflowProgressNode.cpp
    workflow/WorkflowExecutionState.cpp
    workflow/WorkflowResultFuture.cpp
    workflow/WorkflowPlan.cpp
    workflow/WorkflowGuiThreadDispatcher.cpp
    workflow/WorkflowMetric.cpp
    workflow/WorkflowExecutionMetrics.cpp
    workflow/WorkflowResultRegistry.cpp
    workflow/WorkflowResultDialog.cpp
    workflow/WorkflowConsoleFormatter.cpp
    workflow/WorkflowExecutionLifecycleScope.cpp
    workflow/WorkflowStageSummary.cpp
    workflow/WorkflowExecutionNodeType.cpp
    workflow/WorkflowConsoleDashboard.cpp
    workflow/WorkflowConsoleDashboardScope.cpp
    workflow/WorkflowRuntimeScoped.cpp
    workflow/WorkflowContextVariantMap.cpp
    workflow/WorkflowHandle.cpp
    workflow/WorkflowContextVariantMap.cpp
    workflow/WorkflowBatchingOptions.cpp
    workflow/WorkflowResultBase.cpp
    workflow/WorkflowMessageDetailsDelegate.cpp
)

set(PUBLIC_WORKFLOW_FILES
    ${PUBLIC_WORKFLOW_HEADERS}
    ${PUBLIC_WORKFLOW_SOURCES}
)

set(PUBLIC_PARALLEL_HEADERS
    parallel/Parallel.h
    parallel/ParallelExecutionChain.h
)

set(PUBLIC_PARALLEL_SOURCES
    parallel/Parallel.cpp
    parallel/ParallelExecutionChain.cpp
)

set(PUBLIC_PARALLEL_FILES
    ${PUBLIC_PARALLEL_HEADERS}
    ${PUBLIC_PARALLEL_SOURCES}
)

set(PUBLIC_HARDWARE_SPEC_UTIL_HEADERS
	util/HardwareComponentSpec.h
	util/DisplayComponentSpec.h
	util/CpuComponentSpec.h
	util/RamComponentSpec.h
	util/StorageComponentSpec.h
	util/HardwareSpec.h
)

set(PUBLIC_HARDWARE_SPEC_UTIL_SOURCES
	util/HardwareComponentSpec.cpp
	util/DisplayComponentSpec.cpp
	util/CpuComponentSpec.cpp
	util/RamComponentSpec.cpp
	util/StorageComponentSpec.cpp
	util/HardwareSpec.cpp
)

set(PUBLIC_HARDWARE_SPEC_UTIL_FILES
    ${PUBLIC_HARDWARE_SPEC_UTIL_HEADERS}    
    ${PUBLIC_HARDWARE_SPEC_UTIL_SOURCES}
)

set(PUBLIC_APPLICATION_HEADERS
    Application.h
    ApplicationStartupTask.h
)

set(PUBLIC_APPLICATION_SOURCES
    Application.cpp
    ApplicationStartupTask.cpp
)

set(PUBLIC_APPLICATION_FILES
    ${PUBLIC_APPLICATION_HEADERS}
    ${PUBLIC_APPLICATION_SOURCES}
)

set(PUBLIC_PROJECT_HEADERS
    Project.h
    ProjectMetaAction.h
    ProjectCompressionAction.h
)

set(PUBLIC_PROJECT_SOURCES
    Project.cpp
    ProjectMetaAction.cpp
    ProjectCompressionAction.cpp
)

set(PUBLIC_PROJECT_FILES
    ${PUBLIC_PROJECT_HEADERS}
    ${PUBLIC_PROJECT_SOURCES}
)

set(PUBLIC_WORKSPACE_HEADERS
    Workspace.h
)

set(PUBLIC_WORKSPACE_SOURCES
    Workspace.cpp
)

set(PUBLIC_WORKSPACE_FILES
    ${PUBLIC_WORKSPACE_HEADERS}
    ${PUBLIC_WORKSPACE_SOURCES}
)

set(PUBLIC_DATASET_HEADERS
    DataType.h
    RawData.h
    Set.h
    SetLegacySerialization.h
    LinkedData.h
    SelectionGroup.h
    Dataset.h
    DatasetPrivate.h
    DatasetsMimeData.h
    PropertiesSerializer.h
)

set(PUBLIC_DATASET_SOURCES
    Set.cpp
    SetLegacySerialization.cpp
    LinkedData.cpp
    SelectionGroup.cpp
    Dataset.cpp
    DatasetPrivate.cpp
    DatasetsMimeData.cpp
    PropertiesSerializer.cpp
)

set(PUBLIC_DATASET_FILES
    ${PUBLIC_DATASET_HEADERS}
    ${PUBLIC_DATASET_SOURCES}
)

set(PUBLIC_PLUGIN_HEADERS
    Plugin.h
    PluginType.h
    PluginFactory.h
    PluginMetadata.h
    LoaderPlugin.h
    WriterPlugin.h
    AnalysisPlugin.h
    TransformationPlugin.h
    ViewPlugin.h
    PluginShortcuts.h
)

set(PUBLIC_PLUGIN_SOURCES
    Plugin.cpp
    PluginType.cpp
    PluginFactory.cpp
    PluginMetadata.cpp
    LoaderPlugin.cpp
    WriterPlugin.cpp
    AnalysisPlugin.cpp
    TransformationPlugin.cpp
    ViewPlugin.cpp
    PluginShortcuts.cpp
)

list(APPEND PUBLIC_PLUGIN_FILES
    ${PUBLIC_PLUGIN_HEADERS}
    ${PUBLIC_PLUGIN_SOURCES}
)

set(PUBLIC_DATA_HIERARCHY_HEADERS
    DataHierarchyItem.h
)

set(PUBLIC_DATA_HIERARCHY_SOURCES
    DataHierarchyItem.cpp
)

list(APPEND PUBLIC_DATA_HIERARCHY_FILES
    ${PUBLIC_DATA_HIERARCHY_HEADERS}
    ${PUBLIC_DATA_HIERARCHY_SOURCES}
)

set(PUBLIC_COMMON_MODEL_HEADERS
    models/StandardItemModel.h
    models/SortFilterProxyModel.h
    models/NumberOfRowsAction.h
    models/TextOnlyProxyModel.h
)

set(PUBLIC_COMMON_MODEL_SOURCES
    models/StandardItemModel.cpp
    models/SortFilterProxyModel.cpp
    models/NumberOfRowsAction.cpp
    models/TextOnlyProxyModel.cpp
)

set(PUBLIC_COMMON_MODEL_FILES
    ${PUBLIC_COMMON_MODEL_HEADERS}
    ${PUBLIC_COMMON_MODEL_SOURCES}
)

set(PUBLIC_ACTIONS_MODEL_HEADERS
    models/AbstractActionsModel.h
    models/ActionsListModel.h
    models/ActionsHierarchyModel.h
    models/ActionsFilterModel.h
)

set(PUBLIC_ACTIONS_MODEL_SOURCES
    models/AbstractActionsModel.cpp
    models/ActionsListModel.cpp
    models/ActionsHierarchyModel.cpp
    models/ActionsFilterModel.cpp
)

set(PUBLIC_ACTIONS_MODEL_FILES
    ${PUBLIC_ACTIONS_MODEL_HEADERS}
    ${PUBLIC_ACTIONS_MODEL_SOURCES}
)

set(PUBLIC_DATA_HIERARCHY_MODEL_HEADERS
    models/AbstractDataHierarchyModel.h
    models/DataHierarchyTreeModel.h
    models/DataHierarchyListModel.h
    models/DataHierarchyFilterModel.h
)

set(PUBLIC_DATA_HIERARCHY_MODEL_SOURCES
    models/AbstractDataHierarchyModel.cpp
    models/DataHierarchyTreeModel.cpp
    models/DataHierarchyListModel.cpp
    models/DataHierarchyFilterModel.cpp
)

set(PUBLIC_DATA_HIERARCHY_MODEL_FILES
    ${PUBLIC_DATA_HIERARCHY_MODEL_HEADERS}
    ${PUBLIC_DATA_HIERARCHY_MODEL_SOURCES}
)

set(PUBLIC_RAW_DATA_MODEL_HEADERS
    models/RawDataModel.h
    models/RawDataFilterModel.h
)

set(PUBLIC_RAW_DATA_MODEL_SOURCES
    models/RawDataModel.cpp
    models/RawDataFilterModel.cpp
)

set(PUBLIC_RAW_DATA_MODEL_FILES
    ${PUBLIC_RAW_DATA_MODEL_HEADERS}
    ${PUBLIC_RAW_DATA_MODEL_SOURCES}
)

set(PUBLIC_SELECTIONS_MODEL_HEADERS
    models/SelectionsModel.h
    models/SelectionsFilterModel.h
)

set(PUBLIC_SELECTIONS_MODEL_SOURCES
    models/SelectionsModel.cpp
    models/SelectionsFilterModel.cpp
)

set(PUBLIC_SELECTIONS_MODEL_FILES
    ${PUBLIC_SELECTIONS_MODEL_HEADERS}
    ${PUBLIC_SELECTIONS_MODEL_SOURCES}
)

set(PUBLIC_LOGGING_MODEL_HEADERS
    models/LoggingModel.h
    models/LoggingFilterModel.h
)

set(PUBLIC_LOGGING_MODEL_SOURCES
    models/LoggingModel.cpp
    models/LoggingFilterModel.cpp
)

set(PUBLIC_LOGGING_MODEL_FILES
    ${PUBLIC_LOGGING_MODEL_HEADERS}
    ${PUBLIC_LOGGING_MODEL_SOURCES}
)

set(PUBLIC_TASKS_MODEL_HEADERS
    models/AbstractTasksModel.h
    models/TasksTreeModel.h
    models/TasksListModel.h
    models/TasksFilterModel.h
)

set(PUBLIC_TASKS_MODEL_SOURCES
    models/AbstractTasksModel.cpp
    models/TasksTreeModel.cpp
    models/TasksListModel.cpp
    models/TasksFilterModel.cpp
)

set(PUBLIC_TASKS_MODEL_FILES
    ${PUBLIC_TASKS_MODEL_HEADERS}
    ${PUBLIC_TASKS_MODEL_SOURCES}
)

set(PUBLIC_PLUGINS_MODEL_HEADERS
    models/AbstractPluginsModel.h
    models/PluginsTreeModel.h
    models/PluginsListModel.h
    models/PluginsFilterModel.h
)

set(PUBLIC_PLUGINS_MODEL_SOURCES
    models/AbstractPluginsModel.cpp
    models/PluginsTreeModel.cpp
    models/PluginsListModel.cpp
    models/PluginsFilterModel.cpp
)

set(PUBLIC_PLUGINS_MODEL_FILES
    ${PUBLIC_PLUGINS_MODEL_HEADERS}
    ${PUBLIC_PLUGINS_MODEL_SOURCES}
)

set(PUBLIC_THIRD_PARTY_LICENSES_MODEL_HEADERS
    models/AbstractThirdPartyLicensesModel.h
    models/ThirdPartyLicensesListModel.h
    models/ThirdPartyLicensesFilterModel.h
)

set(PUBLIC_THIRD_PARTY_LICENSES_MODEL_SOURCES
    models/AbstractThirdPartyLicensesModel.cpp
    models/ThirdPartyLicensesListModel.cpp
    models/ThirdPartyLicensesFilterModel.cpp
)

set(PUBLIC_THIRD_PARTY_LICENSES_MODEL_FILES
    ${PUBLIC_THIRD_PARTY_LICENSES_MODEL_HEADERS}
    ${PUBLIC_THIRD_PARTY_LICENSES_MODEL_SOURCES}
)

set(PUBLIC_PLUGIN_FACTORIES_MODEL_HEADERS
    models/AbstractPluginFactoriesModel.h
    models/PluginFactoriesListModel.h
    models/PluginFactoriesTreeModel.h
    models/PluginFactoriesFilterModel.h
)

set(PUBLIC_PLUGIN_FACTORIES_MODEL_SOURCES
    models/AbstractPluginFactoriesModel.cpp
    models/PluginFactoriesListModel.cpp
    models/PluginFactoriesTreeModel.cpp
    models/PluginFactoriesFilterModel.cpp
)

set(PUBLIC_PLUGIN_FACTORIES_MODEL_FILES
    ${PUBLIC_PLUGIN_FACTORIES_MODEL_HEADERS}
    ${PUBLIC_PLUGIN_FACTORIES_MODEL_SOURCES}
)

set(PUBLIC_MISCELLANEOUS_MODEL_HEADERS
    models/CheckableStringListModel.h
)

set(PUBLIC_MISCELLANEOUS_MODEL_SOURCES
    models/CheckableStringListModel.cpp
)

set(PUBLIC_MISCELLANEOUS_MODEL_FILES
    ${PUBLIC_MISCELLANEOUS_MODEL_HEADERS}
    ${PUBLIC_MISCELLANEOUS_MODEL_SOURCES}
)

set(PUBLIC_DATASETS_MODEL_HEADERS
    models/AbstractDatasetsModel.h
    models/DatasetsListModel.h
    models/DatasetsFilterModel.h
)

set(PUBLIC_DATASETS_MODEL_SOURCES
    models/AbstractDatasetsModel.cpp
    models/DatasetsListModel.cpp
    models/DatasetsFilterModel.cpp
)

set(PUBLIC_DATASETS_MODEL_FILES
    ${PUBLIC_DATASETS_MODEL_HEADERS}
    ${PUBLIC_DATASETS_MODEL_SOURCES}
)

set(PUBLIC_LEARNING_CENTER_VIDEOS_MODEL_HEADERS
    models/LearningCenterVideosModel.h
	models/LearningCenterVideosFilterModel.h
)

set(PUBLIC_LEARNING_CENTER_VIDEOS_MODEL_SOURCES
    models/LearningCenterVideosModel.cpp
	models/LearningCenterVideosFilterModel.cpp
)

set(PUBLIC_LEARNING_CENTER_VIDEOS_MODEL_FILES
    ${PUBLIC_LEARNING_CENTER_VIDEOS_MODEL_HEADERS}
    ${PUBLIC_LEARNING_CENTER_VIDEOS_MODEL_SOURCES}
)

set(PUBLIC_LEARNING_CENTER_TUTORIALS_MODEL_HEADERS
    models/LearningCenterTutorialsModel.h
	models/LearningCenterTutorialsFilterModel.h
)

set(PUBLIC_LEARNING_CENTER_TUTORIALS_MODEL_SOURCES
    models/LearningCenterTutorialsModel.cpp
	models/LearningCenterTutorialsFilterModel.cpp
)

set(PUBLIC_LEARNING_CENTER_TUTORIALS_MODEL_FILES
    ${PUBLIC_LEARNING_CENTER_TUTORIALS_MODEL_HEADERS}
    ${PUBLIC_LEARNING_CENTER_TUTORIALS_MODEL_SOURCES}
)

set(PUBLIC_PROJECTS_MODEL_HEADERS
    models/AbstractProjectsModel.h
    models/ProjectsListModel.h
    models/ProjectsTreeModel.h
	models/ProjectsFilterModel.h
	models/ProjectsModelProject.h
	models/ProjectsModelVisibilityController.h
)

set(PUBLIC_PROJECTS_MODEL_SOURCES
    models/AbstractProjectsModel.cpp
    models/ProjectsListModel.cpp
    models/ProjectsTreeModel.cpp
	models/ProjectsFilterModel.cpp
	models/ProjectsModelProject.cpp
	models/ProjectsModelVisibilityController.cpp
)

set(PUBLIC_PROJECTS_MODEL_FILES
    ${PUBLIC_PROJECTS_MODEL_HEADERS}
    ${PUBLIC_PROJECTS_MODEL_SOURCES}
)

set(PUBLIC_HARDWARE_SPEC_MODEL_HEADERS
    models/AbstractHardwareSpecModel.h
    models/HardwareSpecTreeModel.h
	models/HardwareSpecFilterModel.h
)

set(PUBLIC_HARDWARE_SPEC_MODEL_SOURCES
    models/AbstractHardwareSpecModel.cpp
    models/HardwareSpecTreeModel.cpp
	models/HardwareSpecFilterModel.cpp
)

set(PUBLIC_HARDWARE_SPEC_MODEL_FILES
    ${PUBLIC_HARDWARE_SPEC_MODEL_HEADERS}
    ${PUBLIC_HARDWARE_SPEC_MODEL_SOURCES}
)

set(PUBLIC_COLOR_SCHEMES_MODEL_HEADERS
    models/AbstractColorSchemesModel.h
	models/ColorSchemesListModel.h
    models/ColorSchemesFilterModel.h
)

set(PUBLIC_COLOR_SCHEMES_MODEL_SOURCES
    models/AbstractColorSchemesModel.cpp
	models/ColorSchemesListModel.cpp
    models/ColorSchemesFilterModel.cpp
)

set(PUBLIC_COLOR_SCHEMES_MODEL_FILES
    ${PUBLIC_COLOR_SCHEMES_MODEL_HEADERS}
    ${PUBLIC_COLOR_SCHEMES_MODEL_SOURCES}
)

set(PUBLIC_SCRIPTS_MODEL_HEADERS
    models/AbstractScriptsModel.h
    models/ScriptsListModel.h
    models/ScriptsTreeModel.h
    models/ScriptsFilterModel.h
)

set(PUBLIC_SCRIPTS_MODEL_SOURCES
    models/AbstractScriptsModel.cpp
    models/ScriptsListModel.cpp
    models/ScriptsTreeModel.cpp
    models/ScriptsFilterModel.cpp
)

set(PUBLIC_SCRIPTS_MODEL_FILES
    ${PUBLIC_SCRIPTS_MODEL_HEADERS}
    ${PUBLIC_SCRIPTS_MODEL_SOURCES}
)

set(PUBLIC_RECENT_FILES_MODEL_HEADERS
    models/AbstractRecentFilesModel.h
    models/RecentFilesListModel.h
    models/RecentFilesFilterModel.h
)

set(PUBLIC_RECENT_FILES_MODEL_SOURCES
	models/AbstractRecentFilesModel.cpp
    models/RecentFilesListModel.cpp
    models/RecentFilesFilterModel.cpp
)

set(PUBLIC_RECENT_FILES_MODEL_FILES
    ${PUBLIC_RECENT_FILES_MODEL_HEADERS}
    ${PUBLIC_RECENT_FILES_MODEL_SOURCES}
)

set(PUBLIC_HUD_MODEL_HEADERS
    models/AbstractHeadsUpDisplayModel.h
    models/HeadsUpDisplayTreeModel.h
    models/HeadsUpDisplayFilterModel.h
)

set(PUBLIC_HUD_MODEL_SOURCES
    models/AbstractHeadsUpDisplayModel.cpp
    models/HeadsUpDisplayTreeModel.cpp
    models/HeadsUpDisplayFilterModel.cpp
)

set(PUBLIC_HUD_MODEL_FILES
    ${PUBLIC_HUD_MODEL_HEADERS}
    ${PUBLIC_HUD_MODEL_SOURCES}
)

set(PUBLIC_WORKFLOW_MESSAGES_MODEL_HEADERS
    models/AbstractWorkflowMessagesModel.h
    models/WorkflowMessagesListModel.h
    models/WorkflowMessagesTreeModel.h
    models/WorkflowMessagesFilterModel.h
)

set(PUBLIC_WORKFLOW_MESSAGES_MODEL_SOURCES
    models/AbstractWorkflowMessagesModel.cpp
    models/WorkflowMessagesListModel.cpp
    models/WorkflowMessagesTreeModel.cpp
    models/WorkflowMessagesFilterModel.cpp
)

set(PUBLIC_WORKFLOW_MESSAGES_MODEL_FILES
    ${PUBLIC_WORKFLOW_MESSAGES_MODEL_HEADERS}
    ${PUBLIC_WORKFLOW_MESSAGES_MODEL_SOURCES}
)

set(PUBLIC_GLOBAL_SETTINGS_HEADERS
    GlobalSettingsGroupAction.h
    ParametersSettingsAction.h
    MiscellaneousSettingsAction.h
    TasksSettingsAction.h
    AppearanceSettingsAction.h
    TemporaryDirectoriesSettingsAction.h
	ErrorLoggingSettingsAction.h
    PluginGlobalSettingsGroupAction.h
)

set(PUBLIC_GLOBAL_SETTINGS_SOURCES
    GlobalSettingsGroupAction.cpp
    ParametersSettingsAction.cpp
    MiscellaneousSettingsAction.cpp
    TasksSettingsAction.cpp
    AppearanceSettingsAction.cpp
    TemporaryDirectoriesSettingsAction.cpp
    ErrorLoggingSettingsAction.cpp
    PluginGlobalSettingsGroupAction.cpp
)

set(PUBLIC_GLOBAL_SETTINGS_FILES
    ${PUBLIC_GLOBAL_SETTINGS_HEADERS}
    ${PUBLIC_GLOBAL_SETTINGS_SOURCES}
)

set(PUBLIC_TASK_HEADERS
    Task.h
    TaskTesterRunner.h
    AbstractTaskHandler.h
    AbstractTaskTester.h
    ModalTask.h
    ModalTaskTester.h
    ModalTaskHandler.h
    BackgroundTask.h
    BackgroundTaskTester.h
    BackgroundTaskHandler.h
    ForegroundTask.h
    ForegroundTaskTester.h
    ForegroundTaskHandler.h
    DatasetTask.h
    DatasetTaskHandler.h
)

set(PUBLIC_TASK_SOURCES
    Task.cpp
    TaskTesterRunner.cpp
    AbstractTaskHandler.cpp
    AbstractTaskTester.cpp
    ModalTask.cpp
    ModalTaskTester.cpp
    ModalTaskHandler.cpp
    BackgroundTask.cpp
    BackgroundTaskTester.cpp
    BackgroundTaskHandler.cpp
    ForegroundTask.cpp
    ForegroundTaskTester.cpp
    ForegroundTaskHandler.cpp
    DatasetTask.cpp
    DatasetTaskHandler.cpp
)

set(PUBLIC_TASK_FILES
    ${PUBLIC_TASK_HEADERS}
    ${PUBLIC_TASK_SOURCES}
)

set(PUBLIC_EXCEPTION_HEADERS
    exception/ManiVaultException.h
)

set(PUBLIC_EXCEPTION_SOURCES
    exception/ManiVaultException.cpp
)
    
set(PUBLIC_EXCEPTION_FILES
    ${PUBLIC_EXCEPTION_HEADERS}
    ${PUBLIC_EXCEPTION_SOURCES}
)

# Automatically generated during cmake config
set(PUBLIC_VERSION_HEADERS
    ManiVaultVersion.h
)

set(PUBLIC_GLOBALS_HEADERS
    ManiVaultGlobals.h
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
    ${PUBLIC_WORKFLOW_HEADERS}
    ${PUBLIC_PARALLEL_HEADERS}
    ${PUBLIC_HARDWARE_SPEC_UTIL_HEADERS}
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
    ${PUBLIC_THIRD_PARTY_LICENSES_MODEL_HEADERS}
    ${PUBLIC_PLUGIN_FACTORIES_MODEL_HEADERS}
    ${PUBLIC_MISCELLANEOUS_MODEL_HEADERS}
	${PUBLIC_DATASETS_MODEL_HEADERS}
	${PUBLIC_LEARNING_CENTER_VIDEOS_MODEL_HEADERS}
	${PUBLIC_LEARNING_CENTER_TUTORIALS_MODEL_HEADERS}
	${PUBLIC_PROJECTS_MODEL_HEADERS}
	${PUBLIC_HARDWARE_SPEC_MODEL_HEADERS}
    ${PUBLIC_COLOR_SCHEMES_MODEL_HEADERS}
    ${PUBLIC_SCRIPTS_MODEL_HEADERS}
    ${PUBLIC_RECENT_FILES_MODEL_HEADERS}
    ${PUBLIC_HUD_MODEL_HEADERS}
    ${PUBLIC_WORKFLOW_MESSAGES_MODEL_HEADERS}
    ${PUBLIC_GLOBAL_SETTINGS_HEADERS}
    ${PUBLIC_TASK_HEADERS}
    ${PUBLIC_EXCEPTION_HEADERS}
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
    ${PUBLIC_WORKFLOW_SOURCES}
    ${PUBLIC_PARALLEL_SOURCES}
    ${PUBLIC_HARDWARE_SPEC_UTIL_SOURCES}
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
    ${PUBLIC_THIRD_PARTY_LICENSES_MODEL_SOURCES}
    ${PUBLIC_PLUGIN_FACTORIES_MODEL_SOURCES}
    ${PUBLIC_MISCELLANEOUS_MODEL_SOURCES}
	${PUBLIC_DATASETS_MODEL_SOURCES}
	${PUBLIC_LEARNING_CENTER_VIDEOS_MODEL_SOURCES}
	${PUBLIC_LEARNING_CENTER_TUTORIALS_MODEL_SOURCES}
	${PUBLIC_PROJECTS_MODEL_SOURCES}
	${PUBLIC_HARDWARE_SPEC_MODEL_SOURCES}
    ${PUBLIC_COLOR_SCHEMES_MODEL_SOURCES}
    ${PUBLIC_SCRIPTS_MODEL_SOURCES}
    ${PUBLIC_RECENT_FILES_MODEL_SOURCES}
    ${PUBLIC_HUD_MODEL_SOURCES}
    ${PUBLIC_WORKFLOW_MESSAGES_MODEL_SOURCES}
    ${PUBLIC_GLOBAL_SETTINGS_SOURCES}
    ${PUBLIC_TASK_SOURCES}
    ${PUBLIC_EXCEPTION_SOURCES}
    ${PUBLIC_NOTIFICATIONS_SOURCES}
    ${PUBLIC_HEADERS}
)

set(PRECOMPILE_HEADERS
    actions/Actions.h
    event/EventListener.h
    AbstractManager.h
    Task.h
    BackgroundTask.h
    ForegroundTask.h
    ModalTask.h
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
source_group(Util\\HardwareSpec FILES ${PUBLIC_HARDWARE_SPEC_UTIL_FILES})
source_group(Workflow FILES ${PUBLIC_WORKFLOW_FILES})
source_group(Parallel FILES ${PUBLIC_PARALLEL_FILES})
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
source_group(Models\\ThirdPartyLicenses FILES ${PUBLIC_THIRD_PARTY_LICENSES_MODEL_FILES})
source_group(Models\\PluginFactories FILES ${PUBLIC_PLUGIN_FACTORIES_MODEL_FILES})
source_group(Models\\Miscellaneous FILES ${PUBLIC_MISCELLANEOUS_MODEL_FILES})
source_group(Models\\Datasets FILES ${PUBLIC_DATASETS_MODEL_FILES})
source_group(Models\\LearningCenter\\Videos FILES ${PUBLIC_LEARNING_CENTER_VIDEOS_MODEL_FILES})
source_group(Models\\LearningCenter\\Tutorials FILES ${PUBLIC_LEARNING_CENTER_TUTORIALS_MODEL_FILES})
source_group(Models\\Projects FILES ${PUBLIC_PROJECTS_MODEL_FILES})
source_group(Models\\ColorSchemes FILES ${PUBLIC_COLOR_SCHEMES_MODEL_FILES})
source_group(Models\\Scripts FILES ${PUBLIC_SCRIPTS_MODEL_FILES})
source_group(Models\\WorkflowMessages FILES ${PUBLIC_WORKFLOW_MESSAGES_MODEL_FILES})
source_group(Models\\HardwareSpec FILES ${PUBLIC_HARDWARE_SPEC_MODEL_FILES})
source_group(Models\\RecentFiles FILES ${PUBLIC_RECENT_FILES_MODEL_FILES})
source_group(Models\\HUD FILES ${PUBLIC_HUD_MODEL_FILES})
source_group(GlobalSettings FILES ${PUBLIC_GLOBAL_SETTINGS_FILES})
source_group(Task FILES ${PUBLIC_TASK_FILES})
source_group(Exception FILES ${PUBLIC_EXCEPTION_FILES})
source_group(Notifications FILES ${PUBLIC_NOTIFICATIONS_FILES})

# -----------------------------------------------------------------------------
# Target MV_PUBLIC_LIB
# -----------------------------------------------------------------------------

add_library(${MV_PUBLIC_LIB} SHARED)

add_library(ManiVault::Core ALIAS ${MV_PUBLIC_LIB})

set_target_properties(${MV_PUBLIC_LIB} PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    AUTOMOC ON
    PROJECT_LABEL "MV_Public"       # Set the project name in VS different to the target name
    OUTPUT_NAME "MV_Public"         # Set the library output name different to the target name
    UNITY_BUILD ${MV_UNITY_BUILD}
)

target_sources(${MV_PUBLIC_LIB}
    PRIVATE
    ${PUBLIC_SOURCES}
    ${SHADERS}
)

target_include_directories(${MV_PUBLIC_LIB} PRIVATE "${MV_ROOT_DIR}/src/lib")

target_include_directories(${MV_PUBLIC_LIB} PRIVATE "${nlohmann_json_SOURCE_DIR}/include")
target_include_directories(${MV_PUBLIC_LIB} PRIVATE "${valijson_SOURCE_DIR}/include")

target_compile_features(${MV_PUBLIC_LIB} PUBLIC cxx_std_${MV_CXX_STANDARD})

target_compile_definitions(${MV_PUBLIC_LIB} 
    PRIVATE 
        MV_SHARED_EXPORT
        VALIJSON_USE_EXCEPTIONS=1
)

target_compile_options(${MV_PUBLIC_LIB} 
    PRIVATE
        $<$<AND:$<BOOL:${MV_WARNINGS_AS_ERRORS}>,$<CXX_COMPILER_ID:MSVC>>:/WX>
)

target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::Core)
target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::Gui)
target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::Widgets)
target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::OpenGL)
target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::Concurrent)

# Use AVX if enabled and available
mv_check_and_set_AVX(${MV_PUBLIC_LIB} ${MV_USE_AVX})

if(MV_PRECOMPILE_HEADERS)
    target_precompile_headers(${MV_PUBLIC_LIB} PRIVATE 
        ${PRECOMPILE_HEADERS}
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${MV_PUBLIC_LIB} PRIVATE /bigobj)
endif()


# -----------------------------------------------------------------------------
# Installation library
# -----------------------------------------------------------------------------

install(TARGETS ${MV_PUBLIC_LIB}
    EXPORT "ManiVaultPublicExport"
    RUNTIME DESTINATION $<CONFIGURATION>        COMPONENT MVPUBLIC_LINKLIB  # Windows .dll
    ARCHIVE DESTINATION "$<CONFIGURATION>/lib/" COMPONENT MVPUBLIC_LINKLIB  # Linux/Mac .so
    LIBRARY DESTINATION "$<CONFIGURATION>/lib/" COMPONENT MVPUBLIC_LINKLIB  # Windows .lib
)

# Install public headers with preserved folder structure
foreach(PUBLIC_HEADER ${PUBLIC_HEADERS})
    string(REGEX REPLACE "^(.*/).*" "\\1" STRIPPED_PUBLIC_HEADER "${PUBLIC_HEADER}")
    if("${STRIPPED_PUBLIC_HEADER}" STREQUAL "${PUBLIC_HEADER}")
        set(STRIPPED_PUBLIC_HEADER "")
    endif()
    set(HEADER_DEST "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/${STRIPPED_PUBLIC_HEADER}")    
    install(FILES ${PUBLIC_HEADER} DESTINATION ${HEADER_DEST} COMPONENT MVPUBLIC_HEADERS)
endforeach()

# Generate a .cmake file that can be imported from the install to enable plugin building
install(EXPORT "ManiVaultPublicExport"
        FILE ManiVaultTargets.cmake
        NAMESPACE ManiVault::
        DESTINATION cmake/mv
        COMPONENT MVPUBLIC_EXPORT
)

# Copy cmake helper files
install(FILES 
        "${MV_ROOT_DIR}/cmake/mv_install_dependencies_utils.cmake"
        "${MV_ROOT_DIR}/cmake/mv_install_dependencies.cmake.in"
        "${MV_ROOT_DIR}/cmake/mv_check_and_set_AVX.cmake"
        "${MV_ROOT_DIR}/cmake/mv_plugin_meta_data.cmake"
        "${MV_ROOT_DIR}/cmake/mv_project_defaults.cmake"
        DESTINATION cmake/mv
        COMPONENT MVPUBLIC_CMAKE_HELPER
)
# Generate the version file for use with find_package
write_basic_package_version_file(
  ManiVaultConfigVersion.cmake
  VERSION ${MV_VERSION}
  COMPATIBILITY SameMajorVersion
)

# create config file
configure_package_config_file(
    "${MV_ROOT_DIR}/cmake/MvCoreConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ManiVaultConfig.cmake"
    INSTALL_DESTINATION cmake/mv
    PATH_VARS MV_INSTALL_DIR
)

# install the package files
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/ManiVaultConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/ManiVaultConfigVersion.cmake"
        DESTINATION cmake/mv
        COMPONENT MVPUBLIC_PACKAGE
)

# Install program database (.pdb) files
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES $<TARGET_PDB_FILE:${MV_PUBLIC_LIB}> DESTINATION $<CONFIGURATION> COMPONENT MVPUBLIC_LINKLIB CONFIGURATIONS "Debug" OPTIONAL)
    install(FILES $<TARGET_PDB_FILE:${MV_EXE}>        DESTINATION .                COMPONENT MV_EXECUTABLE    CONFIGURATIONS "Debug" OPTIONAL)
    install(FILES $<TARGET_PDB_FILE:${MV_PUBLIC_LIB}> DESTINATION $<CONFIGURATION> COMPONENT MVPUBLIC_LINKLIB CONFIGURATIONS "RelWithDebInfo" OPTIONAL)
    install(FILES $<TARGET_PDB_FILE:${MV_EXE}>        DESTINATION .                COMPONENT MV_EXECUTABLE    CONFIGURATIONS "RelWithDebInfo" OPTIONAL)

    if(MV_RELWITHDEBUGINFO)
        install(FILES $<TARGET_PDB_FILE:${MV_PUBLIC_LIB}> DESTINATION $<CONFIGURATION> COMPONENT MVPUBLIC_LINKLIB CONFIGURATIONS "Release" OPTIONAL)
        install(FILES $<TARGET_PDB_FILE:${MV_EXE}>        DESTINATION .                COMPONENT MV_EXECUTABLE    CONFIGURATIONS "Release" OPTIONAL)
    endif()

endif()

# Call the install functions of the all public targets
# needed for plugins directly after the build
add_custom_command(TARGET ${MV_PUBLIC_LIB} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault public headers"
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --component MVPUBLIC_HEADERS
        --verbose
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault public link library"
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --component MVPUBLIC_LINKLIB
        --prefix ${MV_INSTALL_DIR}
        --verbose
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault cmake config files"
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --component MVPUBLIC_PACKAGE
        --prefix ${MV_INSTALL_DIR}
        --verbose
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault cmake target files"
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --component MVPUBLIC_EXPORT
        --prefix ${MV_INSTALL_DIR}
        --verbose
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault cmake helper files"
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --component MVPUBLIC_CMAKE_HELPER
        --prefix ${MV_INSTALL_DIR}
        --verbose
    # If no plugin directory exists, ManiVault might try to import 
    # any library in the executable directory as a plugin
    COMMAND ${CMAKE_COMMAND}
        -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/Plugins
)
