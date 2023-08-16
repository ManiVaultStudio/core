// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskHandler.h"

#include "actions/TasksAction.h"

#include <QDialog>

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

        /**
         * Override popup minimum size hint
         * @return Minimum size hint
         */
        QSize minimumSizeHint() const override {
            return QSize(800, 10);
        }

        /**
         * Override popup size hint
         * @return Size hint
         */
        QSize sizeHint() const override {
            return minimumSizeHint();
        }

    private:

        /** Invoked when the number of modal tasks changes */
        void numberOfModalTasksChanged();

    private:
        ModalTaskHandler* _modalTaskHandler;        /** Pointer to owning modal task handler */
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

    /** Create modal tasks dialog (initializes ModalTaskHandler#_modalTasksDialog if it is not a nullptr) */
    void createDialog();

    /** Destroys the modal tasks dialog */
    void destroyDialog();

    /**
     * Determine whether there is a modal tasks dialog live or not
     * @return Boolean determining whether there is a modal tasks dialog live or not
     */
    bool hasDialog() const;

public: // Action getters    

    /** Get tasks action for displaying the model tasks */
    gui::TasksAction& getTasksAction() { return _tasksAction; };

private:
    gui::TasksAction                    _tasksAction;           /** Tasks action */
    std::unique_ptr<ModalTasksDialog>   _modalTasksDialog;      /** Modal tasks dialog */
};

}
