#include "ImportDataMenu.h"
#include "PluginManager.h"

#include <Application.h>
#include <actions/PluginTriggerAction.h>

using namespace hdps;
using namespace hdps::gui;
using namespace hdps::util;

ImportDataMenu::ImportDataMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent)
{
    setTitle("Import data...");
    setToolTip("Import data into HDPS");

    connect(this, &QMenu::aboutToShow, this, [this]() -> void {
        clear();

        for (auto pluginTriggerAction : Application::core()->getPluginManager().getPluginTriggerActions(plugin::Type::LOADER))
            addAction(pluginTriggerAction);
    });
}
