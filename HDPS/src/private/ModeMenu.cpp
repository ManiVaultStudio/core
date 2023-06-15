#include "ModeMenu.h"
#include "PluginManager.h"
#include "CoreInterface.h"

#include <AbstractProjectManager.h>

#include <util/Miscellaneous.h>

using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;

ModeMenu::ModeMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent),
    _studioModeAction(this, "Studio Mode")
{
    setTitle("Mode");
    setToolTip("Switch between ManiVault modes");

    _studioModeAction.setIcon(Application::getIconFont("FontAwesome").getIcon("pencil-ruler"));

    addAction(&_studioModeAction);

    connect(&_studioModeAction, &ToggleAction::toggled, this, [](bool toggled) -> void {
        auto viewPlugins = plugins().getPluginsByType(plugin::Type::VIEW);

        if (toggled) {
            for (auto viewPlugin : viewPlugins)
                viewPlugin->cacheConnectionPermissions(true);

            for (auto viewPlugin : viewPlugins)
                viewPlugin->setConnectionPermissionsToAll(true);
        }
        else {
            for (auto viewPlugin : viewPlugins)
                viewPlugin->restoreConnectionPermissions(true);
        }
    });

    const auto updateStudioModeActionReadOnly = [&]() -> void {
        _studioModeAction.setEnabled(projects().hasProject());
    };

    updateStudioModeActionReadOnly();

    connect(&projects(), &AbstractProjectManager::projectCreated, this, updateStudioModeActionReadOnly);
    connect(&projects(), &AbstractProjectManager::projectDestroyed, this, updateStudioModeActionReadOnly);
    connect(&projects(), &AbstractProjectManager::projectOpened, this, updateStudioModeActionReadOnly);
    connect(&projects(), &AbstractProjectManager::projectSaved, this, updateStudioModeActionReadOnly);
}

