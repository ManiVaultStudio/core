#include "DataAction.h"
#include "DataHierarchyItem.h"
#include "Application.h"

#include <QMenu>

namespace hdps {

using namespace gui;

DataAction::DataAction(QObject* parent, DataSet& dataset) :
    WidgetAction(parent),
    _dataset(dataset),
    _dataAnalysisAction(this, dataset),
    _dataExportAction(this, dataset),
    _dataRemoveAction(this, dataset)
{
    setText("Data");
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("trash-alt"));

    _dataset->getDataHierarchyItem().addAction(*this);
}

}