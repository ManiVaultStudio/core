#include "DataAnalysisAction.h"
#include "DataHierarchyItem.h"
#include "Core.h"
#include "Set.h"

#include <QMenu>

namespace hdps {

using namespace gui;

DataAnalysisAction::DataAnalysisAction(QObject* parent, DataSet& dataset) :
    WidgetAction(parent),
    _dataset(dataset),
    _pluginKinds()
{
    setText("Analyze");
    setIcon(Application::getIconFont("FontAwesome").getIcon("square-root-alt"));

    _dataset->getDataHierarchyItem().addAction(*this);

    _pluginKinds = Application::core()->getPluginKindsByPluginTypeAndDataTypes(plugin::Type::ANALYSIS, QVector<DataType>({ _dataset->getDataType() }));

    for (auto pluginKind : _pluginKinds) {
        auto analysisPluginAction = new TriggerAction(this, pluginKind);

        analysisPluginAction->setIcon(Application::core()->getPluginIcon(pluginKind));

        connect(analysisPluginAction, &TriggerAction::triggered, this, [this, pluginKind]() {
            _dataset->getDataHierarchyItem().analyzeDataset(pluginKind);
        });
    }
}

QMenu* DataAnalysisAction::getContextMenu(QWidget* parent /*= nullptr*/)
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