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
    _groupsAction(parent),
    _groupsActionWidget(nullptr)
{
    setAutoFillBackground(true);
    setLayout(&_layout);

    _layout.setContentsMargins(6, 6, 6, 6);

    _groupsActionWidget = dynamic_cast<GroupsAction::Widget*>(_groupsAction.createWidget(parent));

    _layout.addWidget(_groupsActionWidget);

    connect(&Application::core()->getDataHierarchyManager(), &DataHierarchyManager::selectedItemsChanged, this, &DataPropertiesWidget::selectedItemsChanged);

    connect(&_dataset, &Dataset<DatasetImpl>::dataRemoved, this, [this]() -> void {
        _groupsAction.setGroupActions({});
    });
}

void DataPropertiesWidget::selectedItemsChanged(DataHierarchyItems selectedItems)
{
    try
    {
        // Reset when the selection is empty
        if (selectedItems.isEmpty()) {
            _groupsAction.setGroupActions({});
        }
        else {
            GroupsAction::GroupActions groupActions;

            if (selectedItems.count() == 1) {
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

                for (auto childObject : _dataset->children()) {
                    auto groupAction = dynamic_cast<GroupAction*>(childObject);

                    if (groupAction)
                        groupActions << groupAction;
                }

                _groupsActionWidget->getFilteredActionsAction().setShowLabels(true);
            }
            else {
                Datasets datasets;

                for (const auto& selectedItem : selectedItems)
                    datasets << selectedItem->getDataset();

                auto groupAction = new GroupAction(nullptr, true);

                groupAction->setText("Actions");
                groupAction->setToolTip("Actions for the current selection");
                groupAction->setShowLabels(false);

                QVector<WidgetAction*> triggerActions;

                const auto createPluginTypeActionsGroup = [datasets, &groupActions, groupAction, &triggerActions](const plugin::Type& type) -> void {
                    for (auto triggerAction : hdps::Application::core()->getPluginActionsByPluginTypeAndDatasets(type, datasets)) {
                        switch (type)
                        {
                            case plugin::Type::VIEW:        triggerAction->setText(QString("View %1").arg(triggerAction->text()));      break;
                            case plugin::Type::ANALYSIS:    triggerAction->setText(QString("Analyze %1").arg(triggerAction->text()));   break;
                            case plugin::Type::WRITER:      triggerAction->setText(QString("Export %1").arg(triggerAction->text()));    break;

                            default:
                                break;
                        }

                        triggerAction->setParent(groupAction);

                        triggerActions << triggerAction;
                    }
                };

                createPluginTypeActionsGroup(plugin::Type::VIEW);
                createPluginTypeActionsGroup(plugin::Type::ANALYSIS);
                createPluginTypeActionsGroup(plugin::Type::WRITER);

                if (!triggerActions.isEmpty())
                    groupActions << groupAction;

                _groupsActionWidget->getFilteredActionsAction().setShowLabels(false);
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
