// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskHandler.h"
#include "TasksFilterModel.h"

#include "actions/TasksAction.h"

#include <QDialog>
#include <QTimer>

namespace hdps {

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

        /** Invoked when the number of modal tasks changes */
        void numberOfModalTasksChanged();

        /** Removes all existing task widget layout items */
        void cleanLayout();

    private:
        ModalTaskHandler*       _modalTaskHandler;  /** Pointer to owning modal task handler */
        QMap<Task*, QWidget*>   _widgetsMap;        /** Maps task to allocated widget */
    };

public:

    /**
    * Construct task handler with \p parent object
    * @param parent Pointer to parent object
    */
    ModalTaskHandler(QObject* parent);

    /**
     * Get status bar action
     * @return Pointer to status bar widget action
     */
    gui::WidgetAction* getStatusBarAction() override { return nullptr; }
    
    /**
     * Get tasks filter model
     * @return Reference to the tasks filter model which filters the tasks tree model instance
     */
    TasksFilterModel& getTasksFilterModel();

private:

    /** Updates the visibility of the modal tasks dialog */
    void updateDialogVisibility();

private:
    TasksFilterModel    _tasksFilterModel;
    ModalTasksDialog    _modalTasksDialog;      /** Modal tasks dialog */
    QTimer              _minimumDurationTimer;  /** Wait for a small amount of time before showing the UI */
};

}
