# -----------------------------------------------------------------------------
# Source files for target MV_EXE
# -----------------------------------------------------------------------------
# defines MAIN_SOURCES, PRIVATE_SOURCES and PRIVATE_HEADERS
# all file path are relative to ROOT/src

set(MAIN_SOURCES
    Main.cpp
)

set(PRIVATE_CORE_HEADERS
    private/Core.h
)

set(PRIVATE_CORE_SOURCES
    private/Core.cpp
)

set(PRIVATE_CORE_FILES
    ${PRIVATE_CORE_HEADERS}
    ${PRIVATE_CORE_SOURCES}
)

set(PRIVATE_ACTIONS_HEADERS
    private/ManiVaultVersionStatusBarAction.h
    private/PluginsStatusBarAction.h
    private/LoggingStatusBarAction.h
    private/BackgroundTasksStatusBarAction.h
    private/FrontPagesStatusBarAction.h
    private/SettingsStatusBarAction.h
    private/WorkspaceStatusBarAction.h
)

set(PRIVATE_ACTIONS_SOURCES
    private/PluginsStatusBarAction.cpp
    private/ManiVaultVersionStatusBarAction.cpp
    private/LoggingStatusBarAction.cpp
    private/BackgroundTasksStatusBarAction.cpp
    private/FrontPagesStatusBarAction.cpp
    private/SettingsStatusBarAction.cpp
    private/WorkspaceStatusBarAction.cpp
)

set(PRIVATE_ACTIONS_FILES
    ${PRIVATE_ACTIONS_HEADERS}    
    ${PRIVATE_ACTIONS_SOURCES}
)

set(PRIVATE_WORKSPACE_MANAGER_HEADERS
    private/WorkspaceManager.h
    private/DockManager.h
    private/DockComponentsFactory.h
    private/ViewPluginsDockWidget.h
    private/ViewPluginDockWidget.h
    private/CentralDockWidget.h
    private/DockWidget.h
    private/DockAreaTitleBar.h
    private/WorkspaceSettingsDialog.h
)

set(PRIVATE_WORKSPACE_MANAGER_SOURCES
    private/WorkspaceManager.cpp
    private/DockManager.cpp
    private/DockComponentsFactory.cpp
    private/ViewPluginsDockWidget.cpp
    private/ViewPluginDockWidget.cpp
    private/CentralDockWidget.cpp
    private/DockWidget.cpp
    private/DockAreaTitleBar.cpp
    private/WorkspaceSettingsDialog.cpp
)

set(PRIVATE_WORKSPACE_MANAGER_FILES
    ${PRIVATE_WORKSPACE_MANAGER_HEADERS}
    ${PRIVATE_WORKSPACE_MANAGER_SOURCES}
)

set(PRIVATE_PLUGIN_MANAGER_HEADERS
    private/PluginManager.h
    private/PluginManagerDialog.h
)

set(PRIVATE_PLUGIN_MANAGER_SOURCES
    private/PluginManager.cpp
    private/PluginManagerDialog.cpp
)

set(PRIVATE_PLUGIN_MANAGER_FILES
    ${PRIVATE_PLUGIN_MANAGER_HEADERS}
    ${PRIVATE_PLUGIN_MANAGER_SOURCES}
)

set(PRIVATE_DATA_MANAGER_HEADERS
    private/DataManager.h
    private/GroupDataDialog.h
    private/RemoveDatasetsDialog.h
    private/DatasetsToRemoveModel.h
    private/DatasetsToRemoveFilterModel.h
)

set(PRIVATE_DATA_MANAGER_SOURCES
    private/DataManager.cpp
    private/GroupDataDialog.cpp
    private/RemoveDatasetsDialog.cpp
    private/DatasetsToRemoveModel.cpp
    private/DatasetsToRemoveFilterModel.cpp
)

set(PRIVATE_DATA_MANAGER_FILES
    ${PRIVATE_DATA_MANAGER_HEADERS}
    ${PRIVATE_DATA_MANAGER_SOURCES}
)

set(PRIVATE_DATA_HIERARCHY_MANAGER_HEADERS
    private/DataHierarchyManager.h
)

set(PRIVATE_DATA_HIERARCHY_MANAGER_SOURCES
    private/DataHierarchyManager.cpp
)

set(PRIVATE_DATA_HIERARCHY_MANAGER_FILES
    ${PRIVATE_DATA_HIERARCHY_MANAGER_HEADERS}
    ${PRIVATE_DATA_HIERARCHY_MANAGER_SOURCES}
)

set(PRIVATE_EVENT_MANAGER_HEADERS
    private/EventManager.h
)

set(PRIVATE_EVENT_MANAGER_SOURCES
    private/EventManager.cpp
)

set(PRIVATE_EVENT_MANAGER_FILES
    ${PRIVATE_EVENT_MANAGER_HEADERS}
    ${PRIVATE_EVENT_MANAGER_SOURCES}
)

set(PRIVATE_ACTIONS_MANAGER_HEADERS
    private/ActionsManager.h
)

set(PRIVATE_ACTIONS_MANAGER_SOURCES
    private/ActionsManager.cpp
)

set(PRIVATE_ACTIONS_MANAGER_FILES
    ${PRIVATE_ACTIONS_MANAGER_HEADERS}
    ${PRIVATE_ACTIONS_MANAGER_SOURCES}
)

set(PRIVATE_PROJECT_MANAGER_HEADERS
    private/ProjectManager.h
    private/ProjectWidget.h
    private/ProjectSettingsDialog.h
    private/NewProjectDialog.h
)

set(PRIVATE_PROJECT_MANAGER_SOURCES
    private/ProjectManager.cpp
    private/ProjectWidget.cpp
    private/ProjectSettingsDialog.cpp
    private/NewProjectDialog.cpp
)

set(PRIVATE_PROJECT_MANAGER_FILES
    ${PRIVATE_PROJECT_MANAGER_HEADERS}
    ${PRIVATE_PROJECT_MANAGER_SOURCES}
)

set(PRIVATE_SETTINGS_MANAGER_HEADERS
    private/SettingsManager.h
    private/SettingsManagerDialog.h
)

set(PRIVATE_SETTINGS_MANAGER_SOURCES
    private/SettingsManager.cpp
    private/SettingsManagerDialog.cpp
)

set(PRIVATE_SETTINGS_MANAGER_FILES
    ${PRIVATE_SETTINGS_MANAGER_HEADERS}
    ${PRIVATE_SETTINGS_MANAGER_SOURCES}
)

set(PRIVATE_TASK_MANAGER_HEADERS
    private/TaskManager.h
)

set(PRIVATE_TASK_MANAGER_SOURCES
    private/TaskManager.cpp
)

set(PRIVATE_TASK_MANAGER_FILES
    ${PRIVATE_TASK_MANAGER_HEADERS}
    ${PRIVATE_TASK_MANAGER_SOURCES}
)

set(PRIVATE_HELP_MANAGER_HEADERS
    private/HelpManager.h
)

set(PRIVATE_HELP_MANAGER_SOURCES
    private/HelpManager.cpp
)

set(PRIVATE_HELP_MANAGER_FILES
    ${PRIVATE_HELP_MANAGER_HEADERS}
    ${PRIVATE_HELP_MANAGER_SOURCES}
)

set(PRIVATE_ERROR_MANAGER_HEADERS
    private/ErrorManager.h
)

set(PRIVATE_ERROR_MANAGER_SOURCES
    private/ErrorManager.cpp
)

set(PRIVATE_ERROR_MANAGER_FILES
    ${PRIVATE_ERROR_MANAGER_HEADERS}
    ${PRIVATE_ERROR_MANAGER_SOURCES}
)

set(PRIVATE_THEME_MANAGER_HEADERS
    private/ThemeManager.h
)

set(PRIVATE_THEME_MANAGER_SOURCES
    private/ThemeManager.cpp
)

set(PRIVATE_THEME_MANAGER_FILES
    ${PRIVATE_THEME_MANAGER_HEADERS}
    ${PRIVATE_THEME_MANAGER_SOURCES}
)

set(PRIVATE_SCRIPTING_MANAGER_HEADERS
    private/ScriptingManager.h
)

set(PRIVATE_SCRIPTING_MANAGER_SOURCES
    private/ScriptingManager.cpp
)

set(PRIVATE_SCRIPTING_MANAGER_FILES
    ${PRIVATE_SCRIPTING_MANAGER_HEADERS}
    ${PRIVATE_SCRIPTING_MANAGER_SOURCES}
)

set(PRIVATE_MANAGER_HEADERS
    ${PRIVATE_WORKSPACE_MANAGER_HEADERS}
    ${PRIVATE_PLUGIN_MANAGER_HEADERS}
    ${PRIVATE_DATA_MANAGER_HEADERS}
    ${PRIVATE_DATA_HIERARCHY_MANAGER_HEADERS}
    ${PRIVATE_EVENT_MANAGER_HEADERS}
    ${PRIVATE_ACTIONS_MANAGER_HEADERS}
    ${PRIVATE_PROJECT_MANAGER_HEADERS}
    ${PRIVATE_SETTINGS_MANAGER_HEADERS}
    ${PRIVATE_TASK_MANAGER_HEADERS}
    ${PRIVATE_HELP_MANAGER_HEADERS}
    ${PRIVATE_ERROR_MANAGER_HEADERS}
    ${PRIVATE_THEME_MANAGER_HEADERS}
    ${PRIVATE_SCRIPTING_MANAGER_HEADERS}
    ${PRIVATE_SERIALIZATION_HEADERS}
)

set(PRIVATE_MANAGER_SOURCES
    ${PRIVATE_WORKSPACE_MANAGER_SOURCES}
    ${PRIVATE_PLUGIN_MANAGER_SOURCES}
    ${PRIVATE_DATA_MANAGER_SOURCES}
    ${PRIVATE_DATA_HIERARCHY_MANAGER_SOURCES}
    ${PRIVATE_EVENT_MANAGER_SOURCES}
    ${PRIVATE_ACTIONS_MANAGER_SOURCES}
    ${PRIVATE_PROJECT_MANAGER_SOURCES}
    ${PRIVATE_WORKSPACE_MANAGER_SOURCES}
    ${PRIVATE_SETTINGS_MANAGER_SOURCES}
    ${PRIVATE_TASK_MANAGER_SOURCES}
    ${PRIVATE_HELP_MANAGER_SOURCES}
    ${PRIVATE_ERROR_MANAGER_SOURCES}
    ${PRIVATE_THEME_MANAGER_SOURCES}
    ${PRIVATE_SCRIPTING_MANAGER_SOURCES}
    ${PRIVATE_SERIALIZATION_SOURCES}
)

set(PRIVATE_MANAGER_FILES
    ${PRIVATE_MANAGER_HEADERS}
    ${PRIVATE_MANAGER_SOURCES}
)

set(PRIVATE_APPLICATION_HEADERS
    private/MainWindow.h
    private/FileMenu.h
    private/ViewMenu.h
    private/LoadSystemViewMenu.h
    private/LoadedViewsMenu.h
    private/ProjectsMenu.h
    private/HelpMenu.h
    private/UserFeedbackDialog.h
    private/NoProxyRectanglesFusionStyle.h
    private/GifEncoder.h
    private/GifRecorder.h
)

if(WIN32)
    list(APPEND PRIVATE_APPLICATION_HEADERS
        private/DeveloperMenu.h
    )
endif()

if(MV_USE_ERROR_LOGGING)
	list(APPEND PRIVATE_APPLICATION_HEADERS
		private/SentryErrorLogger.h
		private/CrashReportDialog.h
		private/ErrorLoggingConsentDialog.h
	)
endif()

set(PRIVATE_APPLICATION_SOURCES
    private/MainWindow.cpp
    private/FileMenu.cpp
    private/ViewMenu.cpp
    private/LoadSystemViewMenu.cpp
    private/LoadedViewsMenu.cpp
    private/ProjectsMenu.cpp
    private/HelpMenu.cpp
    private/UserFeedbackDialog.cpp
    private/NoProxyRectanglesFusionStyle.cpp
    private/GifEncoder.cpp
    private/GifRecorder.cpp
)

if(WIN32)
    list(APPEND PRIVATE_APPLICATION_SOURCES
        private/DeveloperMenu.cpp
    )
endif()

if(MV_USE_ERROR_LOGGING)
	list(APPEND PRIVATE_APPLICATION_SOURCES
		private/SentryErrorLogger.cpp
		private/CrashReportDialog.cpp
		private/ErrorLoggingConsentDialog.cpp
	)
endif()

set(PRIVATE_APPLICATION_FILES
    ${PRIVATE_APPLICATION_HEADERS}
    ${PRIVATE_APPLICATION_SOURCES}
)

set(PRIVATE_WORKFLOW_HEADERS
    private/ProjectOpenWorkflowPlan.h
    private/ProjectSaveWorkflowPlan.h
    private/ProjectPublishWorkflowPlan.h
    private/ProjectPublishContext.h
    private/ParallelPhantomTestSuite.h
    private/TaskflowWorkflowPlanExecutor.h
    private/Taskflow.h
)

set(PRIVATE_WORKFLOW_SOURCES
    private/ProjectOpenWorkflowPlan.cpp
    private/ProjectSaveWorkflowPlan.cpp
    private/ProjectPublishWorkflowPlan.cpp
    private/ProjectPublishContext.cpp
    private/ParallelPhantomTestSuite.cpp
    private/TaskflowWorkflowPlanExecutor.cpp
)

set(PRIVATE_WORKFLOW_FILES
    ${PRIVATE_WORKFLOW_HEADERS}
    ${PRIVATE_WORKFLOW_SOURCES}
)

set(PRIVATE_CODEC_HEADERS
    private/PassthroughBlobCodec.h
    private/PassthroughBlobCodecFactory.h
    private/ZstdBlobCodec.h
    private/ZstdBlobCodecFactory.h
    private/ZstdCodecSettingsAction.h
    private/PassthroughCodecSettingsAction.h
)

set(PRIVATE_CODEC_SOURCES
    private/PassthroughBlobCodec.cpp
    private/PassthroughBlobCodecFactory.cpp
    private/ZstdBlobCodec.cpp
    private/ZstdBlobCodecFactory.cpp
    private/ZstdCodecSettingsAction.cpp
    private/PassthroughCodecSettingsAction.cpp
)

set(PRIVATE_CODEC_FILES
    ${PRIVATE_CODEC_HEADERS}
    ${PRIVATE_CODEC_SOURCES}
)

set(PRIVATE_PAGES_COMMON_HEADERS
    private/PageWidget.h
    private/PageHeaderWidget.h
    private/PageContentWidget.h
    private/LogoWidget.h
	private/PageActionsWidget.h
    private/AbstractPageActionsModel.h
    private/PageActionsTreeModel.h
    private/PageActionsFilterModel.h
    private/PageActionDelegate.h
    private/PageActionDelegateEditorWidget.h
    private/PageAction.h
    private/PageSubAction.h
	private/PageTutorialsWidget.h
)

set(PRIVATE_PAGES_COMMON_SOURCES
    private/PageWidget.cpp
    private/PageHeaderWidget.cpp
    private/PageContentWidget.cpp
    private/LogoWidget.cpp
	private/PageActionsWidget.cpp
    private/AbstractPageActionsModel.cpp
    private/PageActionsTreeModel.cpp
    private/PageActionsFilterModel.cpp
    private/PageActionDelegate.cpp
    private/PageActionDelegateEditorWidget.cpp
    private/PageAction.cpp
    private/PageSubAction.cpp
	private/PageTutorialsWidget.cpp
)

set(PRIVATE_PAGES_COMMON_FILES
    ${PRIVATE_PAGES_COMMON_HEADERS}
    ${PRIVATE_PAGES_COMMON_SOURCES}
)

set(PRIVATE_START_PAGE_HEADERS
    private/StartPageWidget.h
    private/StartPageContentWidget.h
    private/StartPageOpenProjectWidget.h
    private/StartPageGetStartedWidget.h
)

set(PRIVATE_START_PAGE_SOURCES
    private/StartPageWidget.cpp
    private/StartPageContentWidget.cpp
    private/StartPageOpenProjectWidget.cpp
    private/StartPageGetStartedWidget.cpp
)

set(PRIVATE_START_PAGE_FILES
    ${PRIVATE_START_PAGE_HEADERS}
    ${PRIVATE_START_PAGE_SOURCES}
)

set(PRIVATE_LEARNING_PAGE_HEADERS
    private/LearningPageWidget.h
    private/LearningPageContentWidget.h
    private/LearningPageVideosWidget.h
    private/LearningPageVideoWidget.h
    private/LearningPageVideoStyledItemDelegate.h
	private/LearningPageExamplesWidget.h
    private/LearningPagePluginResourcesWidget.h
    private/LearningPagePluginAction.h
)

set(PRIVATE_LEARNING_PAGE_SOURCES
    private/LearningPageWidget.cpp
    private/LearningPageContentWidget.cpp
    private/LearningPageVideosWidget.cpp
    private/LearningPageVideoWidget.cpp
    private/LearningPageVideoStyledItemDelegate.cpp
	private/LearningPageExamplesWidget.cpp
    private/LearningPagePluginResourcesWidget.cpp
    private/LearningPagePluginAction.cpp
)

set(PRIVATE_LEARNING_PAGE_FILES
    ${PRIVATE_LEARNING_PAGE_HEADERS}
    ${PRIVATE_LEARNING_PAGE_SOURCES}
)

set(PRIVATE_MISCELLANEOUS_HEADERS
    private/Archiver.h
)

set(PRIVATE_MISCELLANEOUS_SOURCES
    private/Archiver.cpp
)

set(PRIVATE_MISCELLANEOUS_FILES
    ${PRIVATE_MISCELLANEOUS_HEADERS}
    ${PRIVATE_MISCELLANEOUS_SOURCES}
)

set(PRIVATE_HEADERS
    ${PRIVATE_CORE_HEADERS}
    ${PRIVATE_MANAGER_HEADERS}
    ${PRIVATE_APPLICATION_HEADERS}
	${PRIVATE_PAGES_COMMON_HEADERS}
    ${PRIVATE_START_PAGE_HEADERS}
	${PRIVATE_LEARNING_PAGE_HEADERS}
    ${PRIVATE_MISCELLANEOUS_HEADERS}
    ${PRIVATE_ACTIONS_HEADERS}
    ${PRIVATE_WORKFLOW_HEADERS}
    ${PRIVATE_CODEC_HEADERS}
)

set(PRIVATE_SOURCES
    ${PRIVATE_CORE_SOURCES}
    ${PRIVATE_MANAGER_SOURCES}
    ${PRIVATE_APPLICATION_SOURCES}
	${PRIVATE_PAGES_COMMON_SOURCES}
    ${PRIVATE_START_PAGE_SOURCES}
	${PRIVATE_LEARNING_PAGE_SOURCES}
    ${PRIVATE_MISCELLANEOUS_SOURCES}
    ${PRIVATE_ACTIONS_SOURCES}
    ${PRIVATE_WORKFLOW_SOURCES}
    ${PRIVATE_CODEC_SOURCES}
    ${PRIVATE_HEADERS}
)

list(REMOVE_DUPLICATES PRIVATE_HEADERS)
list(REMOVE_DUPLICATES PRIVATE_SOURCES)

source_group(Core FILES ${PRIVATE_CORE_FILES})
source_group(Actions FILES ${PRIVATE_ACTION_FILES})
source_group(Application FILES ${PRIVATE_APPLICATION_FILES} ${MAIN_SOURCES})
source_group(Managers\\Layout FILES ${PRIVATE_LAYOUT_MANAGER_FILES})
source_group(Managers\\Plugin FILES ${PRIVATE_PLUGIN_MANAGER_FILES})
source_group(Managers\\Data FILES ${PRIVATE_DATA_MANAGER_FILES})
source_group(Managers\\DataHierarchy FILES ${PRIVATE_DATA_HIERARCHY_MANAGER_FILES})
source_group(Managers\\Event FILES ${PRIVATE_EVENT_MANAGER_FILES})
source_group(Managers\\Actions FILES ${PRIVATE_ACTIONS_MANAGER_FILES})
source_group(Managers\\Project FILES ${PRIVATE_PROJECT_MANAGER_FILES})
source_group(Managers\\Workspace FILES ${PRIVATE_WORKSPACE_MANAGER_FILES})
source_group(Managers\\Settings FILES ${PRIVATE_SETTINGS_MANAGER_FILES})
source_group(Managers\\Task FILES ${PRIVATE_TASK_MANAGER_FILES})
source_group(Managers\\Help FILES ${PRIVATE_HELP_MANAGER_FILES})
source_group(Managers\\Error FILES ${PRIVATE_ERROR_MANAGER_FILES})
source_group(Managers\\Theme FILES ${PRIVATE_THEME_MANAGER_FILES})
source_group(Managers\\Scripting FILES ${PRIVATE_SCRIPTING_MANAGER_FILES})
source_group(Pages\\Common FILES ${PRIVATE_PAGES_COMMON_FILES})
source_group(Pages\\StartPage FILES ${PRIVATE_START_PAGE_FILES})
source_group(Pages\\Learning FILES ${PRIVATE_LEARNING_PAGE_FILES})
source_group(Miscellaneous FILES ${PRIVATE_MISCELLANEOUS_FILES})
source_group(Actions FILES ${PRIVATE_ACTIONS_FILES})
source_group(Workflow FILES ${PRIVATE_WORKFLOW_FILES})
source_group(Codec FILES ${PRIVATE_CODEC_FILES})
