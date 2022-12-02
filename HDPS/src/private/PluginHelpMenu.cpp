#include "PluginHelpMenu.h"
#include "PluginManager.h"

#include <Application.h>
#include <Plugin.h>

#include <actions/TriggerAction.h>

#include <util/Miscellaneous.h>

using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;

PluginHelpMenu::PluginHelpMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent)
{
    setTitle("Help");
    setToolTip("HDPS plugin documentation");
    setIcon(Application::getIconFont("FontAwesome").getIcon("plug"));
    setEnabled(false);
}

void PluginHelpMenu::showEvent(QShowEvent* showEvent)
{
    clear();

    QVector<QPointer<TriggerAction>> actions;

    const auto plugins = Application::core()->getPluginsByType({ Type::ANALYSIS, Type::DATA, Type::LOADER, Type::WRITER, Type::TRANSFORMATION, Type::VIEW });

    for (auto plugin : plugins)
        actions << &plugin->getTriggerHelpAction();

    sortActions(actions);

    for (auto action : actions)
        addAction(action);
}
