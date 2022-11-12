#include "ImportDataMenu.h"
#include "PluginManager.h"

#include <Application.h>
#include <actions/PluginTriggerAction.h>

using namespace hdps;
using namespace hdps::gui;

ImportDataMenu::ImportDataMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent)
{
    setTitle("Import data...");
    setToolTip("Import data into HDPS");

    connect(Application::current(), &Application::coreSet, this, [this]() -> void {
        connect(&Application::core()->getPluginManager(), &AbstractPluginManager::addLoadImportPluginTriggerAction, this, [this](PluginTriggerAction& pluginTriggerAction) -> void {
            addAction(&pluginTriggerAction);
        });
    });
    
}
