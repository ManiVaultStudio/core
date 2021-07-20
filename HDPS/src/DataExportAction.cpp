#include "DataExportAction.h"
#include "Application.h"

#include <QMenu>

using namespace hdps::gui;

DataExportAction::DataExportAction(QObject* parent) :
    WidgetAction(parent),
    _binaryAction(this, "Binary"),
    _csvAction(this, "CSV")
{
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("palette"));
}

QMenu* DataExportAction::getContextMenu()
{
    auto menu = new QMenu("Export");

    menu->setIcon(icon());

    menu->addAction(&_binaryAction);
    menu->addAction(&_csvAction);

    return menu;
}