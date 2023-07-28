// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/StringAction.h"
#include "actions/OptionsAction.h"

#include "TasksModel.h"

#include <QSortFilterProxyModel>

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

    gui::StringAction& getTypeFilterAction() { return _typeFilterAction; }
    gui::OptionsAction& getStatusFilterAction() { return _statusFilterAction; }

private:
    gui::StringAction   _typeFilterAction;      /** Action for filtering by task type */
    gui::OptionsAction  _statusFilterAction;    /** Action for filtering based on task status */
};

}
