#include "ViewMenu.h"
#include "PluginManager.h"

#include <Application.h>
#include <ViewPlugin.h>
#include <actions/PluginTriggerAction.h>

using namespace hdps;
using namespace hdps::gui;

ViewMenu::ViewMenu(QWidget* parent /*= nullptr*/, const Options& options /*= Option::Default*/) :
    QMenu(parent),
    _options(options),
    _standardViewsMenu(this),
    _loadedViewsMenu(this),
    _separator(nullptr)
{
    setTitle("View");
    setToolTip("Manage view plugins");

    _standardViewsMenu.setTitle("Standard");
    _loadedViewsMenu.setTitle("Loaded");

    _standardViewsMenu.setEnabled(false);
    _loadedViewsMenu.setEnabled(false);

    _separator = addSeparator();

    _separator->setVisible(false);

    if (options.testFlag(LoadStandardViewSubMenu))
        addMenu(&_standardViewsMenu);

    if (options.testFlag(LoadedViewsSubMenu))
        addMenu(&_loadedViewsMenu);
}

void ViewMenu::showEvent(QShowEvent* showEvent)
{
    const auto pluginTriggerActions = Application::core()->getPluginManager().getPluginTriggerActions(plugin::Type::VIEW);

    for (const auto& pluginTriggerAction : pluginTriggerActions) {
        auto viewPluginFactorty = dynamic_cast<const ViewPluginFactory*>(pluginTriggerAction->getPluginFactory());

        if (viewPluginFactorty == nullptr)
            continue;

        if (viewPluginFactorty->isStandardView()) {
            if (_options.testFlag(LoadStandardViewSubMenu)) {
                _standardViewsMenu.addAction(pluginTriggerAction);
                _standardViewsMenu.setEnabled(true);
            }
        }
        else {
            if (_options.testFlag(LoadView))
                insertAction(_separator, pluginTriggerAction);
        }
    }

    if (_options.testFlag(LoadedViewsSubMenu)) {
        const auto viewPlugins = Application::core()->getPluginsByType({ plugin::Type::VIEW });

        _loadedViewsMenu.setEnabled(!viewPlugins.isEmpty());

        for (auto plugin : viewPlugins) {
            auto viewPlugin = dynamic_cast<ViewPlugin*>(plugin);

            if (viewPlugin == nullptr)
                continue;

            _loadedViewsMenu.addAction(&viewPlugin->getVisibleAction());
        }
    }

    _separator->setVisible(!pluginTriggerActions.isEmpty() && (_options.testFlag(LoadStandardViewSubMenu) || _options.testFlag(LoadedViewsSubMenu)));
}