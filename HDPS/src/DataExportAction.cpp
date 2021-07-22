#include "DataExportAction.h"
#include "Core.h"
#include "Application.h"

#include <QMenu>

namespace hdps {

using namespace gui;

DataExportAction::DataExportAction(QObject* parent, Core* core, const QString& datasetName) :
    WidgetAction(parent),
    _binaryAction(this, "Binary"),
    _csvAction(this, "CSV")
{
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("file-export"));

    connect(&_binaryAction, &TriggerAction::triggered, this, [this, core, datasetName]() {
        core->exportDataset("BinExporter", datasetName);
    });

    connect(&_csvAction, &TriggerAction::triggered, this, [this, core, datasetName]() {
        core->exportDataset("CsvPlugin", datasetName);
    });
}

QMenu* DataExportAction::getContextMenu()
{
    auto menu = new QMenu("Export");

    menu->setIcon(icon());

    menu->addAction(&_binaryAction);
    menu->addAction(&_csvAction);

    return menu;
}

}