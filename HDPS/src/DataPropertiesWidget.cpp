#include "DataPropertiesWidget.h"

#include <Application.h>
#include <DataHierarchyItem.h>
#include <actions/GroupAction.h>

#include <QDebug>

namespace hdps
{

namespace gui
{

DataPropertiesWidget::DataPropertiesWidget(QWidget* parent) :
    QWidget(parent),
    _dataset(),
    _layout(),
    _groupsAction(this)
{
    setAutoFillBackground(true);
    setLayout(&_layout);

    _layout.addWidget(_groupsAction.createWidget(this));

    emit currentDatasetGuiNameChanged("");
}

void DataPropertiesWidget::setDatasetId(const QString& datasetId)
{
    try
    {
        // Disconnect any previous connection to data hierarchy item
        if (_dataset.isValid())
            disconnect(&_dataset->getDataHierarchyItem(), &DataHierarchyItem::actionAdded, this, nullptr);

        // Assign the dataset reference
        if (!datasetId.isEmpty())
            _dataset = Application::core()->requestDataset(datasetId);
        else
            _dataset.reset();

        // Only proceed if we have a valid reference
        if (_dataset.isValid())
        {
            // Reload when actions are added on-the-fly
            connect(&_dataset->getDataHierarchyItem(), &DataHierarchyItem::actionAdded, this, [this](WidgetAction& widgetAction) {
                auto groupAction = dynamic_cast<GroupAction*>(&widgetAction);

                if (groupAction)
                    _groupsAction.addGroupAction(groupAction);
            });
        }

        // Initial dataset load
        loadDatasetGroupActions();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Cannot update data properties", e);
    }
    catch (...) {
        exceptionMessageBox("Cannot update data properties");
    }
}

void DataPropertiesWidget::loadDatasetGroupActions()
{
    // Inform others that the loaded dataset changed
    emit currentDatasetGuiNameChanged(_dataset.isValid() ? _dataset->getDataHierarchyItem().getFullPathName() : "");

    if (!_dataset.isValid())
        return;

#ifdef _DEBUG
    qDebug().noquote() << QString("Loading %1 into data properties").arg(_dataset->getGuiName());
#endif

    // Populate groups action with group actions from the dataset
    _groupsAction.setSourceWidgetAction(_dataset.get());
}

}
}
