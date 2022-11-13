#include "ViewMenu.h"
#include "PluginManager.h"

#include <Application.h>
#include <actions/PluginTriggerAction.h>

using namespace hdps;
using namespace hdps::gui;

ViewMenu::ViewMenu(QWidget* parent /*= nullptr*/, bool standardViews /*= true*/, bool loadedViews /*= true*/) :
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

    auto standardViewsMenuSeparator = addSeparator();

    standardViewsMenuSeparator->setVisible(false);

    if (standardViews)
        addMenu(&_standardViewsMenu);

    auto loadedViewsMenuSeparator = addSeparator();

    addMenu(&_loadedViewsMenu);

    connect(Application::current(), &Application::coreSet, this, [this, standardViewsMenuSeparator, loadedViewsMenuSeparator, standardViews, loadedViews]() -> void {
        connect(&Application::core()->getPluginManager(), &AbstractPluginManager::addLoadViewPluginTriggerAction, this, [this, standardViewsMenuSeparator, loadedViewsMenuSeparator](PluginTriggerAction& pluginTriggerAction) -> void {
            standardViewsMenuSeparator->setVisible(true);
            insertAction(standardViewsMenuSeparator, &pluginTriggerAction);
        });

        if (standardViews) {
            connect(&Application::core()->getPluginManager(), &AbstractPluginManager::addLoadStandardViewPluginTriggerAction, this, [&](PluginTriggerAction& pluginTriggerAction) -> void {
                _standardViewsMenu.addAction(&pluginTriggerAction);
                _standardViewsMenu.setEnabled(true);
            });
        }

        loadedViewsMenuSeparator->setVisible(loadedViews);

        if (loadedViews) {
            connect(&Application::core()->getPluginManager(), &AbstractPluginManager::addViewPluginVisibleAction, this, [this](ToggleAction& viewPluginVisibleAction) -> void {
                _loadedViewsMenu.addAction(&viewPluginVisibleAction);
                _loadedViewsMenu.setEnabled(true);
            });
        }
    });
    
}
