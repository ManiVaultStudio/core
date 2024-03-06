// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/GroupAction.h"

#include "models/TasksFilterModel.h"

#include <QGridLayout>

namespace mv::gui {

/**
 * Tasks list action class
 *
 * Action class for managing tasks
 *
 * Note: This action is developed for internal use (not meant to be used in third-party plugins)
 * 
 * @author Thomas Kroes
 */
class TasksListAction : public GroupAction
{
    Q_OBJECT

public:

    /** Widget action for listing the tasks vertically */
    class Widget : public WidgetActionWidget
    {
    private:
        using WidgetsMap = QMap<Task*, QVector<QWidget*>>;

    protected:

        /**
         * Construct with pointer to \p parent widget, owning \p tasksListAction and \p widgetFlags
         * @param parent Pointer to parent widget
         * @param tasksListAction Pointer to owning tasks list action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        Widget(QWidget* parent, TasksListAction* tasksListAction, const std::int32_t& widgetFlags);

    private:

        /** Invoked when the number of tasks changes */
        void updateLayout();

        /** Cleans the tasks layout */
        void cleanLayout();

    protected:
        TasksListAction*    _tasksListAction;   /** Pointer to owning tasks list action */
        QGridLayout         _layout;            /** Tasks layout */
        WidgetsMap          _widgetsMap;        /** Maps task to allocated widget */

        friend class TasksListAction;
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
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE TasksListAction(QObject* parent, const QString& title);

    /**
     * Get tasks model
     * @return Pointer to the tasks model
     */
    AbstractTasksModel* getModel();

    /**
     * Get tasks filter model
     * @return Pointer to the tasks filter model
     */
    TasksFilterModel* getFilterModel();

    /**
     * Initialize the action with pointers to tasks \p model and \p filterModel
     * @param model Pointer to the tasks model
     * @param filterModel Pointer to the tasks filter model
     * @param itemTypeName Item type name string
     * @param widgetConfigurationFunction Configuration function to call after the widget has been configured by the tasks action
     * @param mayLoadTasksPlugin When set to true, adds a trigger action to the toolbar for loading the tasks plugin
     */
    void initialize(AbstractTasksModel* model, TasksFilterModel* filterModel, const QString& itemTypeName, WidgetConfigurationFunction widgetConfigurationFunction = WidgetConfigurationFunction(), bool mayLoadTasksPlugin = true);

private:
    AbstractTasksModel*             _model;                         /** Tasks model */
    TasksFilterModel*               _filterModel;                   /** Filter model for tasks model */
    WidgetConfigurationFunction     _widgetConfigurationFunction;   /** Configuration function to call after the widget has been configured by the tasks action */
    bool                            _mayLoadTasksPlugin;            /** When set to true, adds a trigger action to the toolbar for loading the tasks plugin */
    TriggerAction                   _loadTasksPluginAction;         /** Triggers loading the tasks plugin */
};

}

Q_DECLARE_METATYPE(mv::gui::TasksListAction)

inline const auto tasksListActionMetaTypeId = qRegisterMetaType<mv::gui::TasksListAction*>("mv::gui::TasksListAction");
