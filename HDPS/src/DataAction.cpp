#include "DataAction.h"
#include "DataHierarchyItem.h"
#include "Application.h"

#include <QMenu>

namespace hdps {

using namespace gui;

DataAction::DataAction(QObject* parent, const QString& datasetName) :
    WidgetAction(parent),
    _dataset(datasetName),
    _dataAnalysisAction(this, datasetName),
    _dataExportAction(this, datasetName),
    _dataRemoveAction(this, datasetName)
{
    setText("Data");
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("trash-alt"));

    _dataset->getDataHierarchyItem().addAction(*this);
}

}