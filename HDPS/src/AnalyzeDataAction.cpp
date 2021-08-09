#include "AnalyzeDataAction.h"
#include "DataHierarchyItem.h"

#include <QMenu>

namespace hdps {

using namespace gui;

AnalyzeDataAction::AnalyzeDataAction(QObject* parent, DataHierarchyItem* dataHierarchyItem) :
    WidgetAction(parent),
    _dataHierarchyItem(dataHierarchyItem),
    _meanShiftAction(this, "Mean Shift Clustering"),
    _tsneAction(this, "TSNE"),
    _hsneAction(this, "HSNE")
{
    //setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("square-root-alt"));

    connect(&_meanShiftAction, &TriggerAction::triggered, this, [this]() {
        _dataHierarchyItem->analyzeDataset("Mean Shift Clustering");
    });

    connect(&_tsneAction, &TriggerAction::triggered, this, [this]() {
        _dataHierarchyItem->analyzeDataset("tSNE Analysis");
    });

    connect(&_hsneAction, &TriggerAction::triggered, this, [this]() {
        _dataHierarchyItem->analyzeDataset("HSNE Analysis");
    });
}

QMenu* AnalyzeDataAction::getContextMenu()
{
    auto menu = new QMenu("Analyze");

    menu->setIcon(icon());

    menu->addAction(&_meanShiftAction);
    menu->addAction(&_tsneAction);
    menu->addAction(&_hsneAction);

    menu->addSeparator();

    return menu;
}

}