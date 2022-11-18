#include "LoadSystemViewMenu.h"
#include "PluginManager.h"

#include <Application.h>
#include <AbstractLayoutManager.h>

#include <actions/PluginTriggerAction.h>

#include <algorithm>

using namespace std;

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::gui;

auto sortActions = [](QVector<QAction*>& actions) -> void {
    sort(actions.begin(), actions.end(), [](auto actionA, auto actionB) {
        return actionA->text() < actionB->text();
    });
};

LoadSystemViewMenu::LoadSystemViewMenu(QWidget *parent /*= nullptr*/) :
    QMenu(parent)
{
    setTitle("System views");
    setToolTip("Manage system view plugins");
}

void LoadSystemViewMenu::showEvent(QShowEvent* showEvent)
{
    clear();

    QVector<QAction*> actions;

    auto pluginTriggerActions = Application::core()->getPluginManager().getPluginTriggerActions(plugin::Type::VIEW);

    for (auto pluginTriggerAction : pluginTriggerActions) {
        auto viewPluginFactory = dynamic_cast<const ViewPluginFactory*>(pluginTriggerAction->getPluginFactory());

        if (!viewPluginFactory->producesSystemViewPlugins())
            continue;

        auto action = new QAction(pluginTriggerAction->icon(), viewPluginFactory->getKind(), this);

        connect(action, &QAction::triggered, this, [pluginTriggerAction]() -> void {
            auto viewPlugin = Application::core()->requestPlugin(pluginTriggerAction->getPluginFactory()->getKind());
            Application::core()->getLayoutManager().addViewPlugin(dynamic_cast<ViewPlugin*>(viewPlugin));
        });

        actions << action;
    }

    sortActions(actions);

    for (auto action : actions)
        addAction(action);
}
