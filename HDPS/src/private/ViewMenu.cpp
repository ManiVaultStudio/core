#include "ViewMenu.h"
#include "PluginManager.h"

#include <Application.h>
#include <actions/PluginTriggerAction.h>

using namespace hdps;
using namespace hdps::gui;

ViewMenu::ViewMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent),
    _standardViewsMenu(this),
    _loadedViewsMenu(this)
{
    setTitle("View");
    setToolTip("Manage view plugins");

    _standardViewsMenu.setTitle("Standard");
    _loadedViewsMenu.setTitle("Loaded");

    _standardViewsMenu.setEnabled(false);
    _loadedViewsMenu.setEnabled(false);

    auto separator1 = addSeparator();

    separator1->setVisible(false);

    addMenu(&_standardViewsMenu);

    auto separator2 = addSeparator();

    addMenu(&_loadedViewsMenu);

    connect(Application::current(), &Application::coreSet, this, [this, separator1, separator2]() -> void {
        connect(&Application::core()->getPluginManager(), &AbstractPluginManager::addLoadViewPluginTriggerAction, this, [this, separator1, separator2](PluginTriggerAction& pluginTriggerAction) -> void {
            separator1->setVisible(true);
            insertAction(separator1, &pluginTriggerAction);
        });

        connect(&Application::core()->getPluginManager(), &AbstractPluginManager::addLoadStandardViewPluginTriggerAction, this, [&](PluginTriggerAction& pluginTriggerAction) -> void {
            _standardViewsMenu.addAction(&pluginTriggerAction);
            _standardViewsMenu.setEnabled(true);
        });

        connect(&Application::core()->getPluginManager(), &AbstractPluginManager::addViewPluginVisibleAction, this, [this](ToggleAction& viewPluginVisibleAction) -> void {
            _loadedViewsMenu.addAction(&viewPluginVisibleAction);
            _loadedViewsMenu.setEnabled(true);
        });
    });
    
}
