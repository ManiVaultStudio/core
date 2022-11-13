#include "HelpMenu.h"
#include "PluginManager.h"

#include <Application.h>
#include <actions/PluginTriggerAction.h>

using namespace hdps;
using namespace hdps::gui;

HelpMenu::HelpMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent),
    _pluginsHelpMenu("Plugins")
{
    setTitle("Help");
    setToolTip("HDPS help");

    _pluginsHelpMenu.setIcon(Application::getIconFont("FontAwesome").getIcon("plug"));

    addMenu(&_pluginsHelpMenu);

    connect(Application::current(), &Application::coreSet, this, [this]() -> void {
        connect(&Application::core()->getPluginManager(), &AbstractPluginManager::addPluginTriggerHelpAction, this, [this](TriggerAction& pluginTriggerHelpAction) -> void {
            _pluginsHelpMenu.addAction(&pluginTriggerHelpAction);
        });
    });
}
