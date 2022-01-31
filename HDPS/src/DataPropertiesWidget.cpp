#include "DataPropertiesWidget.h"
#include "Application.h"
#include "Core.h"
#include "DataHierarchyItem.h"

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

    layout->setContentsMargins(0,0,0,0);
    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(_groupsAction.createWidget(this));

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
                if (dynamic_cast<GroupAction*>(&widgetAction) == nullptr)
                    return;

                loadDataset();
            });
        }

        // Initial dataset load
        loadDataset();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Cannot update data properties", e);
    }
    catch (...) {
        exceptionMessageBox("Cannot update data properties");
    }
}

void DataPropertiesWidget::loadDataset()
{
    // Inform others that the loaded dataset changed
    emit currentDatasetGuiNameChanged(_dataset.isValid() ? _dataset->getDataHierarchyItem().getFullPathName() : "");

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
