// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/StringAction.h"
#include "actions/OptionsAction.h"

#include "TasksModel.h"

#include <QSortFilterProxyModel>
#include <QMap>

namespace hdps
{

/**
 * Tasks filter class
 *
 * Proxy model for filtering tasks
 *
 * @author Thomas Kroes
 */
class TasksFilterModel final : public QSortFilterProxyModel
{
public:

    /** Construct with parent \p parent object
     * @param parent Pointer to parent object
    */
    TasksFilterModel(QObject* parent = nullptr);

    /**
     * Returns whether \p row with \p parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     * @return Boolean indicating whether the item is filtered in or out
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

    /**
     * Compares two cluster items
     * @param lhs Left cluster
     * @param rhs Right cluster
     */
    bool lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const override;

    /**
     * Override to connect to source model signals
     * @param sourceModel Pointer to source model
     */
    void setSourceModel(QAbstractItemModel* sourceModel) override;

private:

    /**
     * Get source variant data for \p index, \p column and data \p role
     * @param index Source model index
     * @param column Source model column
     * @param role Data role
     * @return Data in variant form
     */
    QVariant getSourceData(const QModelIndex& index, const TasksModel::Column& column, int role) const;

public: // Action getters

    gui::OptionsAction& getTaskTypeFilterAction() { return _taskTypeFilterAction; }
    gui::OptionsAction& getTaskStatusFilterAction() { return _taskStatusFilterAction; }

private:
    gui::OptionsAction              _taskTypeFilterAction;      /** Action for filtering based on task type */
    gui::OptionsAction              _taskStatusFilterAction;    /** Action for filtering based on task status */
    QMap<QString, std::uint32_t>    _statusTypeCounts;          /** Counts the number of task per status type */
};

}