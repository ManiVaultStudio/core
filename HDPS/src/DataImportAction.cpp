#include "DataImportAction.h"
#include "Core.h"
#include "Application.h"

#include <QMenu>

namespace hdps {

using namespace gui;

DataImportAction::DataImportAction(QObject* parent) :
    WidgetAction(parent),
    _pluginKinds()
{
    setText("Import");
    setIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));

    _pluginKinds = Application::core()->getPluginKindsByPluginTypeAndDataTypes(plugin::Type::LOADER);

    for (auto pluginKind : _pluginKinds) {
        auto importerPluginAction = new TriggerAction(this, Application::core()->getPluginGuiName(pluginKind));

        connect(importerPluginAction, &TriggerAction::triggered, this, [this, pluginKind]() {
            Application::core()->importDataset(pluginKind);
        });
    }
}

QMenu* DataImportAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    if (_pluginKinds.isEmpty())
        return nullptr;

    auto menu = new QMenu(text(), parent);

    menu->setIcon(icon());

    for (auto child : children()) {
        auto triggerAction = dynamic_cast<TriggerAction*>(child);

        if (triggerAction == nullptr)
            continue;

        menu->addAction(triggerAction);
    }

    menu->addSeparator();

    return menu;
}

}