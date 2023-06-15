#include "ModeMenu.h"
#include "PluginManager.h"
#include "CoreInterface.h"

#include <AbstractProjectManager.h>

#include <util/Miscellaneous.h>

using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;

ModeMenu::ModeMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent)
{
    setTitle("Mode");
    setToolTip("Switch between ManiVault modes");

    const auto updateStudioModeActionReadOnly = [&]() -> void {
        setEnabled(projects().hasProject());
    };

    updateStudioModeActionReadOnly();

    connect(&projects(), &AbstractProjectManager::projectCreated, this, [this, updateStudioModeActionReadOnly](const hdps::Project& project) -> void {
        clear();

        updateStudioModeActionReadOnly();

        addAction(const_cast<ToggleAction*>(&project.getStudioModeAction()));
    });

    connect(&projects(), &AbstractProjectManager::projectAboutToBeDestroyed, this, &ModeMenu::clear);
    connect(&projects(), &AbstractProjectManager::projectOpened, this, updateStudioModeActionReadOnly);
    connect(&projects(), &AbstractProjectManager::projectSaved, this, updateStudioModeActionReadOnly);
}

