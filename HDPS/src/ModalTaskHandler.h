// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskHandler.h"

#include "actions/GroupAction.h"

#include <QDialog>

namespace hdps {

/**
 * Modal task handler class
 *
 * Interact with tasks in a modal tasks dialog.
 *
 * @author Thomas Kroes
 */
class ModalTaskHandler final : public AbstractTaskHandler
{
protected:

    /** Dialog for interaction with modal tasks */
    class ModalTasksDialog : QDialog {
    public:

        using ModalTasks = QVector<Task*>;

    public:

        /**
         * Construct with \p parent widget
         * @param parent Pointer to parent widget
         */
        ModalTasksDialog(QWidget* parent = nullptr);

    private:

        /** Executed when the number of tasks or the status of a task changes */
        void tasksChanged();

    private:
        ModalTasks          _modalTasks;            /** Modal tasks */
        gui::GroupAction    _tasksGroupAction;      /** Groups modal tasks */
    };

public:

    /**
    * Construct task handler with \p parent object
    * @param parent Pointer to parent object
    */
    ModalTaskHandler(QObject* parent);

    /** Initializes the handler */
    void init() override;
   
private:
    static ModalTasksDialog     modalTasksDialog;   /** Modal tasks dialog */
};

}
