#include "DataViewAction.h"
#include "Core.h"
#include "Application.h"

#include <QMenu>

namespace hdps {

using namespace gui;

DataViewAction::DataViewAction(QObject* parent, const Dataset<DatasetImpl>& dataset) :
    WidgetAction(parent),
    _dataset(dataset),
    _pluginKinds()
{
    setText("View");
    setIcon(Application::getIconFont("FontAwesome").getIcon("eye"));

    _dataset->getDataHierarchyItem().addAction(*this);

    _pluginKinds = Application::core()->getPluginKindsByPluginTypeAndDataTypes(plugin::Type::VIEW, QVector<DataType>({ _dataset->getDataType() }));

    // List all possible view options
    for (auto pluginKind : _pluginKinds) {

        // Create trigger action for each view plugin
        auto exporterPluginAction = new TriggerAction(this, pluginKind);

        // Set the correct icon
        exporterPluginAction->setIcon(Application::core()->getPluginIcon(pluginKind));

        // And export when trigger action is triggered
        connect(exporterPluginAction, &TriggerAction::triggered, this, [this, pluginKind]() {
            Application::core()->viewDataset(pluginKind, Datasets({ _dataset }));
        });
    }
}

QMenu* DataViewAction::getContextMenu(QWidget* parent /*= nullptr*/)
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