#include "DataImportAction.h"
#include "Application.h"

#include <QMenu>

using namespace hdps::gui;

DataImportAction::DataImportAction(QObject* parent) :
    WidgetAction(parent),
    _imagesAction(this, "Images"),
    _csvAction(this, "CSV"),
    _binaryAction(this, "Binary"),
    _hdf5Action(this, "HDF5")
{
    hdps::Application::getIconFont("FontAwesome").getIcon("file-import");
}

QMenu* DataImportAction::getContextMenu()
{
    auto menu = new QMenu("Import");

    menu->setIcon(icon());

    menu->addAction(&_imagesAction);
    menu->addAction(&_csvAction);
    menu->addAction(&_binaryAction);
    menu->addAction(&_hdf5Action);

    return menu;
}