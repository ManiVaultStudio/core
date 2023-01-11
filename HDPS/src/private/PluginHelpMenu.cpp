#include "PluginHelpMenu.h"
#include "PluginManager.h"

#include <Application.h>
#include <CoreInterface.h>
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

    connect(this, &QMenu::aboutToShow, this, [this]() -> void {
        clear();

        QVector<QPointer<TriggerAction>> actions;

        for (auto plugin : plugins().getPluginsByTypes({ Type::ANALYSIS, Type::DATA, Type::LOADER, Type::WRITER, Type::TRANSFORMATION, Type::VIEW }))
            actions << &plugin->getTriggerHelpAction();

        sortActions(actions);

        for (auto action : actions)
            addAction(action);
    });
}
