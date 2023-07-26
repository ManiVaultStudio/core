// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/WidgetAction.h>

#include <TasksModel.h>

#include <widgets/HierarchyWidget.h>

namespace hdps::gui {

/**
 * Tasks action class
 *
 * Action class for managing tasks
 *
 * @author Thomas Kroes
 */
class TasksAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Tasks action widget */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param tasksAction Pointer to tasks action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        Widget(QWidget* parent, TasksAction* tasksAction, const std::int32_t& widgetFlags);

    private:
        HierarchyWidget  _tasksWidget;   /** Show the tasks in a hierarchy widget */

        friend class TasksAction;
    };

protected:

    /**
     * Get widget representation of the color action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    }

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE TasksAction(QObject* parent, const QString& title = "");

    /**
     * Get tasks model
     * @return Reference to the task model
     */
    TasksModel& getTasksModel();

private:
    TasksModel    _tasksModel;    /** Model with all tasks in the system */
};

}

Q_DECLARE_METATYPE(hdps::gui::TasksAction)

inline const auto tasksActionMetaTypeId = qRegisterMetaType<hdps::gui::TasksAction*>("hdps::gui::TasksAction");
