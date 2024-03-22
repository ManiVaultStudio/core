# -----------------------------------------------------------------------------
# Source files for target MV_EXE
# -----------------------------------------------------------------------------
# defines MAIN_SOURCES, PRIVATE_SOURCES and PRIVATE_HEADERS

set(MAIN_SOURCES
    src/Main.cpp
    res/styles/default.qss
)

set(PRIVATE_CORE_HEADERS
    src/private/Core.h
)

set(PRIVATE_CORE_SOURCES
    src/private/Core.cpp
)

set(PRIVATE_CORE_FILES
    ${PRIVATE_CORE_HEADERS}
    ${PRIVATE_CORE_SOURCES}
)

set(PRIVATE_ACTIONS_HEADERS
    src/private/ManiVaultVersionStatusBarAction.h
    src/private/PluginsStatusBarAction.h
    src/private/LoggingStatusBarAction.h
    src/private/BackgroundTasksStatusBarAction.h
    src/private/ForegroundTasksStatusBarAction.h
    src/private/StartPageStatusBarAction.h
    src/private/SettingsStatusBarAction.h
)

set(PRIVATE_ACTIONS_SOURCES
    src/private/PluginsStatusBarAction.cpp
    src/private/ManiVaultVersionStatusBarAction.cpp
    src/private/LoggingStatusBarAction.cpp
    src/private/BackgroundTasksStatusBarAction.cpp
    src/private/ForegroundTasksStatusBarAction.cpp
    src/private/StartPageStatusBarAction.cpp
    src/private/SettingsStatusBarAction.cpp
)

set(PRIVATE_ACTIONS_FILES
    ${PRIVATE_ACTIONS_HEADERS}    
    ${PRIVATE_ACTIONS_SOURCES}
)

set(PRIVATE_WORKSPACE_MANAGER_HEADERS
    src/private/WorkspaceManager.h
    src/private/DockManager.h
    src/private/DockComponentsFactory.h
    src/private/ViewPluginsDockWidget.h
    src/private/ViewPluginDockWidget.h
    src/private/CentralDockWidget.h
    src/private/DockWidget.h
    src/private/DockAreaTitleBar.h
    src/private/WorkspaceSettingsDialog.h
)

set(PRIVATE_WORKSPACE_MANAGER_SOURCES
    src/private/WorkspaceManager.cpp
    src/private/DockManager.cpp
    src/private/DockComponentsFactory.cpp
    src/private/ViewPluginsDockWidget.cpp
    src/private/ViewPluginDockWidget.cpp
    src/private/CentralDockWidget.cpp
    src/private/DockWidget.cpp
    src/private/DockAreaTitleBar.cpp
    src/private/WorkspaceSettingsDialog.cpp
)

set(PRIVATE_WORKSPACE_MANAGER_FILES
    ${PRIVATE_WORKSPACE_MANAGER_HEADERS}
    ${PRIVATE_WORKSPACE_MANAGER_SOURCES}
)

set(PRIVATE_PLUGIN_MANAGER_HEADERS
    src/private/PluginManager.h
    src/private/PluginManagerDialog.h
)

set(PRIVATE_PLUGIN_MANAGER_SOURCES
    src/private/PluginManager.cpp
    src/private/PluginManagerDialog.cpp
)

set(PRIVATE_PLUGIN_MANAGER_FILES
    ${PRIVATE_PLUGIN_MANAGER_HEADERS}
    ${PRIVATE_PLUGIN_MANAGER_SOURCES}
)

set(PRIVATE_DATA_MANAGER_HEADERS
    src/private/DataManager.h
    src/private/GroupDataDialog.h
    src/private/RemoveDatasetsDialog.h
    src/private/DatasetsToRemoveModel.h
    src/private/DatasetsToRemoveFilterModel.h
)

set(PRIVATE_DATA_MANAGER_SOURCES
    src/private/DataManager.cpp
    src/private/GroupDataDialog.cpp
    src/private/RemoveDatasetsDialog.cpp
    src/private/DatasetsToRemoveModel.cpp
    src/private/DatasetsToRemoveFilterModel.cpp
)

set(PRIVATE_DATA_MANAGER_FILES
    ${PRIVATE_DATA_MANAGER_HEADERS}
    ${PRIVATE_DATA_MANAGER_SOURCES}
)

set(PRIVATE_DATA_HIERARCHY_MANAGER_HEADERS
    src/private/DataHierarchyManager.h
)

set(PRIVATE_DATA_HIERARCHY_MANAGER_SOURCES
    src/private/DataHierarchyManager.cpp
)

set(PRIVATE_DATA_HIERARCHY_MANAGER_FILES
    ${PRIVATE_DATA_HIERARCHY_MANAGER_HEADERS}
    ${PRIVATE_DATA_HIERARCHY_MANAGER_SOURCES}
)

set(PRIVATE_EVENT_MANAGER_HEADERS
    src/private/EventManager.h
)

set(PRIVATE_EVENT_MANAGER_SOURCES
    src/private/EventManager.cpp
)

set(PRIVATE_EVENT_MANAGER_FILES
    ${PRIVATE_EVENT_MANAGER_HEADERS}
    ${PRIVATE_EVENT_MANAGER_SOURCES}
)

set(PRIVATE_ACTIONS_MANAGER_HEADERS
    src/private/ActionsManager.h
)

set(PRIVATE_ACTIONS_MANAGER_SOURCES
    src/private/ActionsManager.cpp
)

set(PRIVATE_ACTIONS_MANAGER_FILES
    ${PRIVATE_ACTIONS_MANAGER_HEADERS}
    ${PRIVATE_ACTIONS_MANAGER_SOURCES}
)

set(PRIVATE_PROJECT_MANAGER_HEADERS
    src/private/ProjectManager.h
    src/private/ProjectWidget.h
    src/private/ProjectSettingsDialog.h
    src/private/NewProjectDialog.h
)

set(PRIVATE_PROJECT_MANAGER_SOURCES
    src/private/ProjectManager.cpp
    src/private/ProjectWidget.cpp
    src/private/ProjectSettingsDialog.cpp
    src/private/NewProjectDialog.cpp
)

set(PRIVATE_PROJECT_MANAGER_FILES
    ${PRIVATE_PROJECT_MANAGER_HEADERS}
    ${PRIVATE_PROJECT_MANAGER_SOURCES}
)

set(PRIVATE_SETTINGS_MANAGER_HEADERS
    src/private/SettingsManager.h
    src/private/SettingsManagerDialog.h
)

set(PRIVATE_SETTINGS_MANAGER_SOURCES
    src/private/SettingsManager.cpp
    src/private/SettingsManagerDialog.cpp
)

set(PRIVATE_SETTINGS_MANAGER_FILES
    ${PRIVATE_SETTINGS_MANAGER_HEADERS}
    ${PRIVATE_SETTINGS_MANAGER_SOURCES}
)

set(PRIVATE_TASK_MANAGER_HEADERS
    src/private/TaskManager.h
)

set(PRIVATE_TASK_MANAGER_SOURCES
    src/private/TaskManager.cpp
)

set(PRIVATE_TASK_MANAGER_FILES
    ${PRIVATE_TASK_MANAGER_HEADERS}
    ${PRIVATE_TASK_MANAGER_SOURCES}
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
)

set(PRIVATE_MANAGER_FILES
    ${PRIVATE_MANAGER_HEADERS}
    ${PRIVATE_MANAGER_SOURCES}
)

set(PRIVATE_APPLICATION_HEADERS
    src/private/MainWindow.h
    src/private/FileMenu.h
    src/private/ViewMenu.h
    src/private/LoadSystemViewMenu.h
    src/private/LoadedViewsMenu.h
    src/private/HelpMenu.h
)

set(PRIVATE_APPLICATION_SOURCES
    src/private/MainWindow.cpp
    src/private/FileMenu.cpp
    src/private/ViewMenu.cpp
    src/private/LoadSystemViewMenu.cpp
    src/private/LoadedViewsMenu.cpp
    src/private/HelpMenu.cpp
)

set(PRIVATE_APPLICATION_FILES
    ${PRIVATE_APPLICATION_HEADERS}
    ${PRIVATE_APPLICATION_SOURCES}
)

set(PRIVATE_START_PAGE_HEADERS
    src/private/StartPageWidget.h
    src/private/StartPageHeaderWidget.h
    src/private/StartPageContentWidget.h
    src/private/StartPageOpenProjectWidget.h
    src/private/StartPageGetStartedWidget.h
    src/private/StartPageActionsWidget.h
    src/private/StartPageActionsModel.h
    src/private/StartPageActionsFilterModel.h
    src/private/StartPageActionDelegate.h
    src/private/StartPageActionDelegateEditorWidget.h
    src/private/StartPageAction.h
    src/private/LogoWidget.h
)

set(PRIVATE_START_PAGE_SOURCES
    src/private/StartPageWidget.cpp
    src/private/StartPageHeaderWidget.cpp
    src/private/StartPageContentWidget.cpp
    src/private/StartPageOpenProjectWidget.cpp
    src/private/StartPageGetStartedWidget.cpp
    src/private/StartPageActionsWidget.cpp
    src/private/StartPageActionsModel.cpp
    src/private/StartPageActionsFilterModel.cpp
    src/private/StartPageActionDelegate.cpp
    src/private/StartPageActionDelegateEditorWidget.cpp
    src/private/StartPageAction.cpp
    src/private/LogoWidget.cpp
)

set(PRIVATE_START_PAGE_FILES
    ${PRIVATE_START_PAGE_HEADERS}
    ${PRIVATE_START_PAGE_SOURCES}
)

set(PRIVATE_MISCELLANEOUS_HEADERS
    src/private/Archiver.h
)

set(PRIVATE_MISCELLANEOUS_SOURCES
    src/private/Archiver.cpp
)

set(PRIVATE_MISCELLANEOUS_FILES
    ${PRIVATE_MISCELLANEOUS_HEADERS}
    ${PRIVATE_MISCELLANEOUS_SOURCES}
)

set(PRIVATE_HEADERS
    ${PRIVATE_CORE_HEADERS}
    ${PRIVATE_MANAGER_HEADERS}
    ${PRIVATE_APPLICATION_HEADERS}
    ${PRIVATE_START_PAGE_HEADERS}
    ${PRIVATE_MISCELLANEOUS_HEADERS}
    ${PRIVATE_ACTIONS_HEADERS}
)

set(PRIVATE_SOURCES
    ${PRIVATE_CORE_SOURCES}
    ${PRIVATE_MANAGER_SOURCES}
    ${PRIVATE_APPLICATION_SOURCES}
    ${PRIVATE_START_PAGE_SOURCES}
    ${PRIVATE_MISCELLANEOUS_SOURCES}
    ${PRIVATE_ACTIONS_SOURCES}
    ${PRIVATE_HEADERS}
)

list(REMOVE_DUPLICATES PRIVATE_HEADERS)
list(REMOVE_DUPLICATES PRIVATE_SOURCES)

source_group(Core FILES ${PRIVATE_CORE_FILES})
source_group(Actions FILES ${PRIVATE_ACTION_FILES})
source_group(Application FILES ${PRIVATE_APPLICATION_FILES})
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
source_group(StartPage FILES ${PRIVATE_START_PAGE_FILES})
source_group(Miscellaneous FILES ${PRIVATE_MISCELLANEOUS_FILES})
source_group(Actions FILES ${PRIVATE_ACTIONS_FILES})
