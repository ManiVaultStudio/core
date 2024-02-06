// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataPropertiesWidget.h"
#include "DataPropertiesPlugin.h"

#include <CoreInterface.h>
#include <Set.h>

#include <QDebug>

using namespace mv;

DataPropertiesWidget::DataPropertiesWidget(DataPropertiesPlugin* dataPropertiesPlugin, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _dataPropertiesPlugin(dataPropertiesPlugin),
    _layout(),
    _groupsAction(parent, "Groups"),
    _groupsActionWidget(nullptr)
{
    setAutoFillBackground(true);
    setLayout(&_layout);

    _layout.setContentsMargins(0, 0, 0, 0);

    _groupsAction.getUpdateTask().setProgressTextFormatter([](Task& task) -> QString { return ""; });

    dataPropertiesPlugin->setProgressTask(&_groupsAction.getUpdateTask());

    _groupsActionWidget = dynamic_cast<GroupsAction::Widget*>(_groupsAction.createWidget(this));

    _layout.addWidget(_groupsActionWidget);

    connect(&mv::dataHierarchy(), &AbstractDataHierarchyManager::selectedItemsChanged, this, &DataPropertiesWidget::dataHierarchySelectionChanged, Qt::DirectConnection);

    dataHierarchySelectionChanged();
}

void DataPropertiesWidget::dataHierarchySelectionChanged()
{
    if (projects().isOpeningProject() || projects().isImportingProject())
        return;

    const auto lockSelectedDataHierarchyItems = [this](bool locked) -> void {
        for (auto selectedDataHierarchyItem : _selectedDataHierarchyItems)
            selectedDataHierarchyItem->setLocked(locked);
    };

    for (auto selectedDataHierarchyItem : _selectedDataHierarchyItems)
        disconnect(&selectedDataHierarchyItem->getDatasetReference(), &Dataset<DatasetImpl>::aboutToBeRemoved, this, nullptr);

    _selectedDataHierarchyItems = mv::dataHierarchy().getSelectedItems();

    _dataPropertiesPlugin->updateWindowTitle(_selectedDataHierarchyItems);

    for (auto selectedDataHierarchyItem : _selectedDataHierarchyItems) {
        connect(&selectedDataHierarchyItem->getDatasetReference(), &Dataset<DatasetImpl>::aboutToBeRemoved, this, [this]() -> void {
            _groupsAction.setGroupActions({});
        });
    }

    lockSelectedDataHierarchyItems(true);
    {
        try
        {
            if (_selectedDataHierarchyItems.isEmpty()) {
                _groupsAction.setGroupActions({});
            }
            else {
                GroupsAction::GroupActions groupActions;

                _groupsActionWidget->setEnabled(_selectedDataHierarchyItems.count() == 1);

                if (_selectedDataHierarchyItems.count() == 1) {
                    auto dataset = _selectedDataHierarchyItems.first()->getDataset();

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

                _groupsAction.setGroupActions(groupActions);

                QCoreApplication::processEvents();
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
    lockSelectedDataHierarchyItems(false);
}
