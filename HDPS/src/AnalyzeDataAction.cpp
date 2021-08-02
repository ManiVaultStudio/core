#include "AnalyzeDataAction.h"
#include "Core.h"
#include "Application.h"

#include <QMenu>

namespace hdps {

using namespace gui;

AnalyzeDataAction::AnalyzeDataAction(QObject* parent, Core* core, const QString& datasetName) :
    WidgetAction(parent),
    _meanShiftAction(this, "Mean Shift Clustering"),
    _tsneAction(this, "TSNE"),
    _hsneAction(this, "HSNE")
{
    //setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("square-root-alt"));

    connect(&_meanShiftAction, &TriggerAction::triggered, this, [this, core, datasetName]() {
        core->analyzeDataset("Mean Shift Clustering", datasetName);
    });

    connect(&_tsneAction, &TriggerAction::triggered, this, [this, core, datasetName]() {
        core->analyzeDataset("tSNE Analysis", datasetName);
    });

    connect(&_hsneAction, &TriggerAction::triggered, this, [this, core, datasetName]() {
        core->analyzeDataset("HSNE Analysis", datasetName);
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