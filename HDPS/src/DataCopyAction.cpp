#include "DataCopyAction.h"
#include "Core.h"
#include "Application.h"

#include <QMenu>

namespace hdps {

using namespace gui;

DataCopyAction::DataCopyAction(QObject* parent, const Dataset<DatasetImpl>& dataset) :
    WidgetAction(parent),
    _dataset(dataset),
    _copyAction(this, "Copy")
{
    setText("Copy");
    setIcon(Application::getIconFont("FontAwesome").getIcon("copy"));

    _dataset->getDataHierarchyItem().addAction(*this);

    // Copy the dataset when the action is triggered
    connect(&_copyAction, &TriggerAction::triggered, this, [this, dataset]() {
        Application::core()->copyDataset(dataset, _dataset->getGuiName());
    });
} 

QMenu* DataCopyAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu(text(), parent);

    menu->setIcon(icon());

    menu->addAction(&_copyAction);

    return menu;
}

}