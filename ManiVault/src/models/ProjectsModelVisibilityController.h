// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QStandardItemModel>
#include <QObject>
#include <functional>

namespace mv {

class AbstractProjectsModel;

/**
 * Projects model visibility controller class
 *
 * Controls the visibility of projects in a projects model, ensuring that among duplicate projects (with the same UUID) only one is visible based on a user-defined criterion
 *
 * @author Thomas Kroes
 */
class ProjectsModelVisibilityController : public QObject
{
    Q_OBJECT

public:

    /** Function type to assign visibility among duplicates */
    using VisibilityRuleFunction = std::function<QModelIndex(const QModelIndexList&, QStandardItemModel*)>;

    /**
     * Construct a visibility controller for the given projects model
     * @param projectsModel Pointer to the projects model to control
     * @param visibilityRuleFunction Function to assign visibility among duplicates
     * @param parent Pointer to parent QObject
     */
    ProjectsModelVisibilityController(AbstractProjectsModel* projectsModel, VisibilityRuleFunction visibilityRuleFunction = {}, QObject* parent = nullptr);

    /** Recompute visibility for all rows in the model */
    void recomputeAll();

private:

    /** Slot called when rows are inserted */
    void onRowsInserted(const QModelIndex& parent, int first, int last);

    /** Slot called after rows are removed */
    void onRowsRemoved(const QModelIndex& parent, int /*first*/, int /*last*/);

    /** Slot called when data changes */
    void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles);

private:

    /** Apply group visibility for the given rows */
    void applyGroupVisibility(const QModelIndexList& rows);

    /**
     * Set visibility of the given index to \p on
     * @param index Model index to set visibility for
     * @param on Boolean determining whether to set visibility on or off
     */
    void setVisibility(const QModelIndex& index, bool on);

    template<class F>
    void walk(const QModelIndex& parent, F&& f)
    {
        const auto numberOfRows = _projectsModel->rowCount(parent);

        for (int rowIndex = 0; rowIndex < numberOfRows; ++rowIndex) {
            QModelIndex rowIx = _projectsModel->index(rowIndex, 0, parent);

            f(rowIx);

        	if (_projectsModel->rowCount(rowIx) > 0)
                walk(rowIx, f);
        }
    }

private:
    AbstractProjectsModel*  _projectsModel = nullptr;   /** Pointer to the controlled projects model */
    VisibilityRuleFunction  _visibilityRuleFunction;    /** Function to choose the winning row among duplicates */
    bool                    _isUpdating = false;        /** Flag indicating whether an update is in progress to prevent recursive updates */
};

}
