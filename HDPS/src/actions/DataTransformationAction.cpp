#include "DataTransformationAction.h"
#include "Core.h"
#include "Application.h"

#include <QMenu>

namespace hdps {

using namespace gui;

DataTransformationAction::DataTransformationAction(QObject* parent, const Dataset<DatasetImpl>& dataset) :
    WidgetAction(parent),
    _dataset(dataset),
    _pluginKinds()
{
    setText("Transform");
    setIcon(Application::getIconFont("FontAwesome").getIcon("random"));

    _dataset->getDataHierarchyItem().addAction(*this);

    _pluginKinds = Application::core()->getPluginKindsByPluginTypeAndDataTypes(plugin::Type::TRANSFORMATION, QVector<DataType>({ _dataset->getDataType() }));

    // List all possible view options
    for (auto pluginKind : _pluginKinds) {

        // Create trigger action for each view plugin
        auto transformationPluginAction = new TriggerAction(this, pluginKind);

        // Set the correct icon
        transformationPluginAction->setIcon(Application::core()->getPluginIcon(pluginKind));

        // And transform data when trigger action is triggered
        connect(transformationPluginAction, &TriggerAction::triggered, this, [this, pluginKind]() {
            Application::core()->transformDatasets(pluginKind, Datasets({ _dataset }));
        });
    }
}

QMenu* DataTransformationAction::getContextMenu(QWidget* parent /*= nullptr*/)
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