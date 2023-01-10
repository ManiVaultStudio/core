#include "CoreInterface.h"
#include "Application.h"

namespace hdps
{

CoreInterface* core()
{
    return Application::core();
}

AbstractActionsManager& actions()
{
    return core()->getActionsManager();
}

AbstractPluginManager& plugins()
{
    return core()->getPluginManager();
}

AbstractDataManager& data()
{
    return core()->getDataManager();
}

AbstractDataHierarchyManager& dataHierarchy()
{
    return core()->getDataHierarchyManager();
}

AbstractWorkspaceManager& workspaces()
{
    return core()->getWorkspaceManager();
}

AbstractProjectManager& projects()
{
    return core()->getProjectManager();
}

AbstractSettingsManager& settings()
{
    return core()->getSettingsManager();
}

}
