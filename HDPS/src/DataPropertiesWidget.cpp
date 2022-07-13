#include "DataPropertiesWidget.h"

#include <Application.h>
#include <DataHierarchyItem.h>
#include <actions/GroupAction.h>
#include <actions/OptionsAction.h>

#include <QDebug>

namespace hdps
{

namespace gui
{

DataPropertiesWidget::DataPropertiesWidget(QWidget* parent) :
    QWidget(parent),
    _dataset(),
    _layout(),
    _groupsAction(parent)
{
    setAutoFillBackground(true);
    setLayout(&_layout);

    _layout.setContentsMargins(6, 6, 6, 6);
    _layout.addWidget(_groupsAction.createWidget(parent));

    // Update the UI when the data hierarchy item selection changes
    connect(&Application::core()->getDataHierarchyManager(), &DataHierarchyManager::selectedItemsChanged, this, &DataPropertiesWidget::selectedItemsChanged);

    // Reset when the dataset is removed
    connect(&_dataset, &Dataset<DatasetImpl>::dataRemoved, this, [this]() -> void {
        _groupsAction.setGroupActions({});
    });
}

void DataPropertiesWidget::selectedItemsChanged(DataHierarchyItems selectedItems)
{
    try
    {
        // Reset when the selection is empty
        if (selectedItems.isEmpty())
            _groupsAction.setGroupActions({});

        // Disconnect any previous connection to data hierarchy item
        if (_dataset.isValid())
            disconnect(&_dataset->getDataHierarchyItem(), &DataHierarchyItem::actionAdded, this, nullptr);

        // Get dataset to display
        _dataset = selectedItems.first()->getDataset();

        // Only proceed if we have a valid pointer to a dataset
        if (_dataset.isValid())
        {
            // Reload when actions are added on-the-fly
            connect(&_dataset->getDataHierarchyItem(), &DataHierarchyItem::actionAdded, this, [this](WidgetAction& widgetAction) {
                auto groupAction = dynamic_cast<GroupAction*>(&widgetAction);

                if (groupAction)
                    _groupsAction.addGroupAction(groupAction);
            });
        }

        if (!_dataset.isValid())
            return;

#ifdef _DEBUG
        qDebug().noquote() << QString("Loading %1 into data properties").arg(_dataset->getGuiName());
#endif

        GroupsAction::GroupActions groupActions;

        // Loop over all child objects and add if it is a group action
        for (auto childObject : _dataset->children()) {
            auto groupAction = dynamic_cast<GroupAction*>(childObject);

            // Add when the action is a group action
            if (groupAction)
                groupActions << groupAction;
        }

        // Set group actions
        _groupsAction.setGroupActions(groupActions);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Cannot update data properties", e);
    }
    catch (...) {
        exceptionMessageBox("Cannot update data properties");
    }
}

}
}
