#include "DataPropertiesWidget.h"
#include "Application.h"
#include "Core.h"

#include "actions/GroupAction.h"

#include <QDebug>
#include <QVBoxLayout>

namespace hdps
{

namespace gui
{

DataPropertiesWidget::DataPropertiesWidget(QWidget* parent) :
    QWidget(parent),
    _dataset(),
    _groupsAction(this)
{
    setAutoFillBackground(true);

    auto layout = new QVBoxLayout();

    setLayout(layout);

    layout->setMargin(0);
    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(_groupsAction.createWidget(this));

    /*
    connect(&_dataset, &DatasetRef<DataSet>::datasetNameChanged, this, [this](const QString& oldDatasetName, const QString& newDatasetName) {
        loadDataset();
    });

    connect(&_dataset, &DatasetRef<DataSet>::datasetAboutToBeRemoved, this, [this]() {
        setDatasetName("");
    });
    */

    // Initial dataset name change
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
        _dataset = Application::core()->requestDataset(datasetId);

        // Only proceed if we have a valid reference
        if (!_dataset.isValid())
            return;

        // Reload when actions are added on-the-fly
        connect(&_dataset->getDataHierarchyItem(), &DataHierarchyItem::actionAdded, this, [this](WidgetAction& widgetAction) {
            if (dynamic_cast<GroupAction*>(&widgetAction) == nullptr)
                return;

            loadDataset();
        });

        // Initial dataset load
        loadDataset();
    }
    catch (std::exception& e)
    {
        qDebug() << QString("Cannot update data properties for %1: %2").arg(_dataset->getGuiName(), e.what());
    }
}

void DataPropertiesWidget::loadDataset()
{
    // Inform others that the loaded dataset changed
    emit currentDatasetGuiNameChanged(_dataset->getGuiName());

    // Clear groups if the reference is invalid
    if (!_dataset.isValid()) {
        _groupsAction.set(QVector<GroupAction*>());
        return;
    }

    // Section in the data properties
    GroupsAction::GroupActions groupActions;

    // Loop over all actions in the dataset and to section if the action is a group action
    for (auto action : _dataset->getDataHierarchyItem().getActions()) {
        auto groupAction = dynamic_cast<GroupAction*>(action);

        // Only add groups action to accordion
        if (groupAction == nullptr)
            continue;

        // Add the group action
        groupActions << groupAction;
    }

    // Assign the groups to the accordion
    _groupsAction.set(groupActions);
}

}
}
