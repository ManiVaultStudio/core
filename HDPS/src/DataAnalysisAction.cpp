#include "DataAnalysisAction.h"
#include "DataHierarchyItem.h"
#include "Core.h"
#include "Set.h"

#include <QMenu>

namespace hdps {

using namespace gui;

DataAnalysisAction::DataAnalysisAction(QObject* parent, Core* core, DataHierarchyItem* dataHierarchyItem) :
    WidgetAction(parent),
    _dataHierarchyItem(dataHierarchyItem),
    _pluginKinds()
{
    //setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("square-root-alt"));

    dataHierarchyItem->addAction(*this);

    _pluginKinds = core->requestPluginKindsByPluginTypeAndDataType(plugin::Type::ANALYSIS, _dataHierarchyItem->getDataset().getDataType());

    for (auto pluginKind : _pluginKinds) {
        auto analysisPluginAction = new TriggerAction(this, pluginKind);
        
        connect(analysisPluginAction, &TriggerAction::triggered, this, [this, pluginKind]() {
            _dataHierarchyItem->analyzeDataset(pluginKind);
        });
    }
}

QMenu* DataAnalysisAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    if (_pluginKinds.isEmpty())
        return nullptr;

    auto menu = new QMenu("Analyze", parent);

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