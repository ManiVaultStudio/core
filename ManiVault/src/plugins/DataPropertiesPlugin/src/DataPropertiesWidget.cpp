// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataPropertiesWidget.h"
#include "DataPropertiesPlugin.h"

#include <CoreInterface.h>
#include <Set.h>

#include <actions/GroupAction.h>

#include <QDebug>

using namespace mv;
using namespace mv::gui;

DataPropertiesWidget::DataPropertiesWidget(DataPropertiesPlugin* dataPropertiesPlugin, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _dataPropertiesPlugin(dataPropertiesPlugin),
    _abortPopulate(false),
    _isPopulating(false),
    _groupsAction(parent, "Groups"),
    _groupsActionWidget(nullptr)
{
    setAutoFillBackground(true);
    setLayout(&_layout);

    _layout.setContentsMargins(0, 0, 0, 0);

    _groupsAction.getUpdateTask().setProgressTextFormatter([](Task& task) -> QString { return ""; });

    dataPropertiesPlugin->setProgressTask(&_groupsAction.getUpdateTask());

    _groupsActionWidget = dynamic_cast<GroupsAction::Widget*>(_groupsAction.createWidget(this));

    _groupsActionWidget->getFilteredActionsAction().setShowLabels(true);

    _layout.addWidget(_groupsActionWidget);

    connect(&mv::dataHierarchy(), &AbstractDataHierarchyManager::selectedItemsChanged, this, &DataPropertiesWidget::dataHierarchySelectionChanged, Qt::DirectConnection);

    dataHierarchySelectionChanged();

    _populateTimer.setSingleShot(true);
    _populateTimer.setInterval(250);

    connect(&_populateTimer, &QTimer::timeout, this, [this]() -> void {
        if (_isPopulating)
            _abortPopulate = true;
        else
            populate();
    });
}

void DataPropertiesWidget::dataHierarchySelectionChanged()
{
    if (projects().isOpeningProject() || projects().isImportingProject())
        return;

    _scheduledItems = mv::dataHierarchy().getSelectedItems();

    if (!_populateTimer.isActive())
        populate();
    else
        _populateTimer.start();
}

void DataPropertiesWidget::beginPopulate()
{
    _isPopulating = true;
}

void DataPropertiesWidget::populate()
{
    beginPopulate();
    {
        try
        {
            if (!_currentItems.isEmpty())
                disconnect(&_currentItems.first()->getDatasetReference(), &Dataset<DatasetImpl>::aboutToBeRemoved, this, nullptr);

            _currentItems = _scheduledItems;

            if (_currentItems.isEmpty()) {
                _groupsAction.setGroupActions({});
            }
            else {
                connect(&_currentItems.first()->getDatasetReference(), &Dataset<DatasetImpl>::aboutToBeRemoved, this, [this]() -> void {
                    _groupsAction.setGroupActions({});
                });

                GroupsAction::GroupActions groupActions;

                _groupsActionWidget->setEnabled(_currentItems.count() == 1);

                if (_currentItems.count() == 1) {
                    auto dataset = _currentItems.first()->getDataset();

                    if (dataset.isValid())
                        disconnect(&dataset->getDataHierarchyItem(), &DataHierarchyItem::actionAdded, this, nullptr);

                    if (dataset.isValid())
                    {
                        connect(&dataset->getDataHierarchyItem(), &DataHierarchyItem::actionAdded, this, [this](WidgetAction& widgetAction) {
                            auto groupAction = dynamic_cast<GroupAction*>(&widgetAction);

                            if (!groupAction)
                                return;

                            _currentItems.first()->lock();
                            {
                                _groupsAction.addGroupAction(groupAction);
                            }
                            _currentItems.first()->restoreLockedFromCache();
                        });
                    }

                    if (!dataset.isValid())
                        return;

#ifdef _DEBUG
                    qDebug().noquote() << QString("Loading %1 into data properties").arg(dataset->text());
#endif

                    const auto childGroupActions = static_cast<WidgetAction*>(dataset.get())->getChildren<mv::gui::GroupAction>();

                    _currentItems.first()->lock(true);
                    {
                        QCoreApplication::processEvents();

                        for (auto childGroupAction : childGroupActions) {
                            if (_abortPopulate)
                                break;

                            groupActions << childGroupAction;
                        }
                    }
                    _currentItems.first()->restoreLockedFromCache();
                }

                _groupsAction.setGroupActions(groupActions);
            }

            if (_abortPopulate)
                populate();
        }
        catch (std::exception& e)
        {
            exceptionMessageBox("Cannot update data properties", e);
        }
        catch (...) {
            exceptionMessageBox("Cannot update data properties");
        }
    }
    endPopulate();

    _abortPopulate = false;
}

void DataPropertiesWidget::endPopulate()
{
    _isPopulating = false;
}
