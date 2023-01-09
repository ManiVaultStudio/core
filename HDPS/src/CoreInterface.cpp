#include "CoreInterface.h"
#include "Application.h"

namespace hdps
{

AbstractActionsManager& actionsManager()
{
    return Application::core()->getActionsManager();
}

AbstractPluginManager& pluginManager()
{
    return Application::core()->getPluginManager();
}

AbstractDataManager& dataManager()
{
    return Application::core()->getDataManager();
}

AbstractDataHierarchyManager& dataHierarchyManager()
{
    return Application::core()->getDataHierarchyManager();
}

AbstractWorkspaceManager& workspaceManager()
{
    return Application::core()->getWorkspaceManager();
}

AbstractProjectManager& projectManager()
{
    return Application::core()->getProjectManager();
}

AbstractSettingsManager& settingsManager()
{
    return Application::core()->getSettingsManager();
}

}
