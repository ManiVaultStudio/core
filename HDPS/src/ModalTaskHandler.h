// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskHandler.h"

#include "models/TasksTreeModel.h"
#include "models/TasksFilterModel.h"

#include "actions/TasksAction.h"

#include <QDialog>
#include <QTimer>

namespace mv {

/**
 * Modal task handler class
 *
 * Takes care of handling modal tasks (i.e. shows them in a modal tasks dialog).
 *
 * @author Thomas Kroes
 */
class ModalTaskHandler final : public AbstractTaskHandler
{
protected:

    /** Dialog for interaction with modal tasks */
    class ModalTasksDialog final : public QDialog {
    public:

        /**
         * Construct with \p parent widget
         * @param modalTaskHandler Pointer to owning modal task handler
         * @param parent Pointer to parent widget
         */
        ModalTasksDialog(ModalTaskHandler* modalTaskHandler, QWidget* parent = nullptr);

    private:

        /** Update the window title and icon */
        void updateWindowTitleAndIcon();

    private:
        ModalTaskHandler*   _modalTaskHandler;      /** Pointer to owning modal task handler */
        gui::TasksAction    _tasksAction;           /** Tree action for displaying the task(s) */
        QWidget*            _tasksWidget;           /** Tasks widget created by the tasks action */
    };

public:

    /**
    * Construct task handler with \p parent object
    * @param parent Pointer to parent object
    */
    ModalTaskHandler(QObject* parent);

    /**
     * Get the tasks model
     * @return Reference to the tasks tree model
     */
    TasksTreeModel& getModel();

    /**
     * Get the tasks filter model
     * @return Reference to the tasks filter model
     */
    TasksFilterModel& getFilterModel();

private:

    /** Updates the visibility of the modal tasks dialog */
    void updateDialogVisibility();

private:
    TasksTreeModel      _model;                 /** Tasks tree model */
    TasksFilterModel    _filterModel;           /** Filter model for the tasks model */
    ModalTasksDialog    _modalTasksDialog;      /** Modal tasks dialog */
    QTimer              _minimumDurationTimer;  /** Wait for a small amount of time before showing the UI */
};

}
