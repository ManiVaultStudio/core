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

    connect(&Application::core()->getDataHierarchyManager(), &DataHierarchyManager::selectedItemsChanged, this, &DataPropertiesWidget::selectedItemsChanged);

    connect(&_dataset, &Dataset<DatasetImpl>::dataRemoved, this, [this]() -> void {
        _groupsAction.setGroupActions({});
    });
}

void DataPropertiesWidget::selectedItemsChanged(DataHierarchyItems selectedItems)
{
    qDebug() << "selectedItemsChanged" << selectedItems.count();

    try
    {
        // Reset when the selection is empty
        if (selectedItems.isEmpty()) {
            _groupsAction.setGroupActions({});
        }
        else {
            if (_dataset.isValid())
                disconnect(&_dataset->getDataHierarchyItem(), &DataHierarchyItem::actionAdded, this, nullptr);

            _dataset = selectedItems.first()->getDataset();

            if (_dataset.isValid())
            {
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

            for (auto childObject : _dataset->children()) {
                auto groupAction = dynamic_cast<GroupAction*>(childObject);

                if (groupAction)
                    groupActions << groupAction;
            }

            _groupsAction.setGroupActions(groupActions);
        }
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
