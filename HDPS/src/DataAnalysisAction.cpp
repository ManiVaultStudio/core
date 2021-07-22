#include "DataAnalysisAction.h"
#include "Core.h"
#include "Application.h"

#include <QMenu>

namespace hdps {

using namespace gui;

DataAnalysisAction::DataAnalysisAction(QObject* parent, Core* core, const QString& datasetName) :
    WidgetAction(parent),
    _meanShiftAction(this, "Mean-shift"),
    _tsneAction(this, "TSNE")
{
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("microchip"));

    connect(&_meanShiftAction, &TriggerAction::triggered, this, [this, core, datasetName]() {
        core->analyzeDataset("Mean Shift Analysis", datasetName);
    });

    connect(&_tsneAction, &TriggerAction::triggered, this, [this, core, datasetName]() {
        core->analyzeDataset("TSNE", datasetName);
    });
}

QMenu* DataAnalysisAction::getContextMenu()
{
    auto menu = new QMenu("Analysis");

    menu->setIcon(icon());

    menu->addAction(&_meanShiftAction);
    menu->addAction(&_tsneAction);

    menu->addSeparator();

    return menu;
}

}