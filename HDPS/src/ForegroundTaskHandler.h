// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskHandler.h"

#include "actions/TasksAction.h"

namespace hdps {

/**
 * Foreground task handler class
 *
 * Contains a specialized tasks action that displays running foreground tasks.
 *
 * @author Thomas Kroes
 */
class ForegroundTaskHandler final : public AbstractTaskHandler
{
public:

    /**
     * Foreground tasks action class
     *
     * Tasks action for showing foreground tasks in a customized popup interface.
     *
     * @author Thomas Kroes
     */
    class ForegroundTasksAction : public gui::WidgetAction
    {
    public:

        /** Widget class for foreground tasks action */
        class Widget : public gui::WidgetActionWidget {
        public:

            /**
             * Constructor
             * @param parent Pointer to parent widget
             * @param foregroundTasksAction Pointer to foreground tasks action
             * @param widgetFlags Widget flags for the configuration of the widget (type)
             */
            Widget(QWidget* parent, ForegroundTasksAction* foregroundTasksAction, const std::int32_t& widgetFlags);
        };

        /**
         * Get widget representation of the foreground tasks action
         * @param parent Pointer to parent widget
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
            return new Widget(parent, this, widgetFlags);
        };

    public:

        /**
         * Initialize with pointer to \p parent object and \p title
         * @param parent Pointer to parent object
         * @param title Action title
         */
        ForegroundTasksAction(QObject* parent, const QString& title);

    protected: // Action getters

        gui::TasksAction& getTasksAction() { return _tasksAction; }

    private:
        gui::TasksAction    _tasksAction;       /** Tasks action which will be configured to show running foreground tasks */
    };

public:

    /**
    * Construct task handler with \p parent object
    * @param parent Pointer to parent object
    */
    ForegroundTaskHandler(QObject* parent);

    /** Initializes the handler */
    void init() override;

public: // Action getters

    ForegroundTasksAction& getForegroundTasksAction() { return _foregroundTasksAction; }

private:
    ForegroundTasksAction    _foregroundTasksAction;   /** Tasks action for showing foreground tasks */
};

}
