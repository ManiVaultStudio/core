#include "DataExportAction.h"
#include "Core.h"
#include "Application.h"

#include <QMenu>

namespace hdps {

using namespace gui;

DataExportAction::DataExportAction(QObject* parent, const QString& datasetName) :
    WidgetAction(parent),
    _dataset(datasetName),
    _pluginKinds()
{
    setText("Export");
    setIcon(Application::getIconFont("FontAwesome").getIcon("file-export"));

    _dataset->getHierarchyItem().addAction(*this);

    _pluginKinds = Application::core()->requestPluginKindsByPluginTypeAndDataTypes(plugin::Type::WRITER, QVector<DataType>({ _dataset->getDataType() }));

    for (auto pluginKind : _pluginKinds) {
        auto exporterPluginAction = new TriggerAction(this, pluginKind);

        connect(exporterPluginAction, &TriggerAction::triggered, this, [this, pluginKind]() {
            _dataset->getHierarchyItem().exportDataset(pluginKind);
        });
    }
}

QMenu* DataExportAction::getContextMenu(QWidget* parent /*= nullptr*/)
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