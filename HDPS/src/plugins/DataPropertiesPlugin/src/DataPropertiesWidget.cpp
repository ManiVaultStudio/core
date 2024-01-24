// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataPropertiesWidget.h"

#include <Application.h>
#include <AbstractDataHierarchyManager.h>
#include <AbstractProjectManager.h>
#include <DataHierarchyItem.h>
#include <Set.h>

#include <actions/GroupAction.h>
#include <actions/OptionsAction.h>
#include <actions/PluginTriggerAction.h>

#include <QDebug>

DataPropertiesWidget::DataPropertiesWidget(QWidget* parent) :
    QWidget(parent),
    _layout(),
    _groupsAction(parent, "Groups"),
    _groupsActionWidget(nullptr)
{
    setAutoFillBackground(true);
    setLayout(&_layout);

    _layout.setContentsMargins(0, 0, 0, 0);

    _groupsActionWidget = dynamic_cast<GroupsAction::Widget*>(_groupsAction.createWidget(this));

    _layout.addWidget(_groupsActionWidget);

    connect(&mv::dataHierarchy(), &AbstractDataHierarchyManager::selectedItemsChanged, this, &DataPropertiesWidget::dataHierarchySelectionChanged);

    dataHierarchySelectionChanged();
}

void DataPropertiesWidget::dataHierarchySelectionChanged()
{
    if (projects().isOpeningProject() || projects().isImportingProject())
        return;

    const auto selectedItems = mv::dataHierarchy().getSelectedItems();

    try
    {
        if (selectedItems.isEmpty()) {
            _groupsAction.setGroupActions({});
        }
        else {
            GroupsAction::GroupActions groupActions;

            _groupsActionWidget->setEnabled(selectedItems.count() == 1);

            if (selectedItems.count() == 1) {
                auto dataset = selectedItems.first()->getDataset();

                if (dataset.isValid())
                    disconnect(&dataset->getDataHierarchyItem(), &DataHierarchyItem::actionAdded, this, nullptr);

                if (dataset.isValid())
                {
                    connect(&dataset->getDataHierarchyItem(), &DataHierarchyItem::actionAdded, this, [this](WidgetAction& widgetAction) {
                        auto groupAction = dynamic_cast<GroupAction*>(&widgetAction);

                        if (groupAction)
                            _groupsAction.addGroupAction(groupAction);
                     });
                }

                if (!dataset.isValid())
                    return;

#ifdef _DEBUG
                qDebug().noquote() << QString("Loading %1 into data properties").arg(dataset->text());
#endif

                for (auto childObject : dataset->children()) {
                    auto groupAction = dynamic_cast<GroupAction*>(childObject);

                    if (groupAction)
                        groupActions << groupAction;
                }

                _groupsActionWidget->getFilteredActionsAction().setShowLabels(true);
            }
            else {
                /*
                Datasets datasets;

                for (const auto& selectedItem : selectedItems)
                    datasets << selectedItem->getDataset();

                auto groupAction = new GroupAction(nullptr, "Actions", true);

                groupAction->setToolTip("Actions for the current selection");
                groupAction->setShowLabels(false);

                QVector<WidgetAction*> triggerActions;

                const auto createPluginTypeActionsGroup = [datasets, &groupActions, groupAction, &triggerActions](const plugin::Type& type) -> void {
                    for (auto pluginTriggerAction : mv::Application::core()->getPluginManager().getPluginTriggerActions(type, datasets)) {
                        switch (type)
                        {
                            case plugin::Type::VIEW:        pluginTriggerAction->setText(QString("View %1").arg(pluginTriggerAction->text()));      break;
                            case plugin::Type::ANALYSIS:    pluginTriggerAction->setText(QString("Analyze %1").arg(pluginTriggerAction->text()));   break;
                            case plugin::Type::WRITER:      pluginTriggerAction->setText(QString("Export %1").arg(pluginTriggerAction->text()));    break;

                            default:
                                break;
                        }

                        pluginTriggerAction->setParent(groupAction);

                        triggerActions << pluginTriggerAction;
                    }
                };

                auto groupDataAction = new TriggerAction(groupAction, "Group data");

                connect(groupDataAction, &TriggerAction::triggered, this, [datasets]() -> void {
                    mv::data().groupDatasets(datasets);
                });

                triggerActions << groupDataAction;

                //createPluginTypeActionsGroup(plugin::Type::VIEW);
                //createPluginTypeActionsGroup(plugin::Type::ANALYSIS);
                //createPluginTypeActionsGroup(plugin::Type::WRITER);

                if (!triggerActions.isEmpty())
                    groupActions << groupAction;

                _groupsActionWidget->getFilteredActionsAction().setShowLabels(false);
                */
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
