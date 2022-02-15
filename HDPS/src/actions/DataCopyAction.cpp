#include "DataCopyAction.h"
#include "Core.h"
#include "Application.h"

#include <QMenu>

namespace hdps {

using namespace gui;

DataCopyAction::DataCopyAction(QObject* parent, const Dataset<DatasetImpl>& dataset) :
    TriggerAction(parent),
    _dataset(dataset)
{
    setText("Copy");
    setIcon(Application::getIconFont("FontAwesome").getIcon("copy"));
    setEnabled(false);
    setSerializable(false);

    // Copy the dataset when the action is triggered
    connect(this, &TriggerAction::triggered, this, [this, dataset]() {
        Application::core()->copyDataset(dataset, _dataset->getGuiName());
    });
} 

}
