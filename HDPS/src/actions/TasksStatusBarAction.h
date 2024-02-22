// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/WidgetAction.h"

#include "AbstractTasksModel.h"
#include "TasksFilterModel.h"
#include "TasksPopupWidget.h"

#include <QToolButton>
#include <QMenu>

namespace mv {
    class AbstractTasksModel;
}

namespace mv::gui {

/**
 * Background tasks status bar action class
 *
 * Tasks action class for display and interaction with background tasks.
 *
 * @author Thomas Kroes
 */
class TasksStatusBarAction : public WidgetAction
{
    Q_OBJECT

public:

    enum class PopupMode {
        Hover,
        Click,
        Manual,
        Automatic
    };

protected:

    class Widget : public WidgetActionWidget
    {
    protected:

        /* Custom tool button class with a popup widget attached to it which show the foreground task(s) */
        class ToolButton : public QToolButton {
        public:

            /**
             * Construct with pointer to owning \p tasksStatusBarAction \p parent widget
             * @param tasksStatusBarAction Pointer to owning tasks status bar action
             * @param parent Pointer to parent widget
             */
            ToolButton(TasksStatusBarAction* tasksStatusBarAction, QWidget* parent = nullptr);

            void paintEvent(QPaintEvent* paintEvent) override;

        private:
            TasksStatusBarAction* _tasksStatusBarAction;
        };

    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param tasksStatusBarAction Pointer to owning tasks status bar action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        Widget(QWidget* parent, TasksStatusBarAction* tasksStatusBarAction, std::int32_t widgetFlags);

        /**
         * Respond to \p target object events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override;

    private:
        TasksStatusBarAction*   _tasksStatusBarAction;
        ToolButton              _toolButton;
        TasksPopupWidget        _tasksPopupWidget;

        friend class TasksStatusBarAction;
    };

protected:

    /**
     * Get widget representation of the tasks status bar action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Construct with \p parent object and \p title
     * @param tasksModel Reference to underlying tasks model
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param taskScope Scope of the task
     */
    TasksStatusBarAction(AbstractTasksModel& tasksModel, QObject* parent, const QString& title, const QIcon& icon = QIcon(), const PopupMode& popupMode = PopupMode::Click, const Task::GuiScope& taskScope = Task::GuiScope::Foreground);

    /**
     * Get the popup mode
     * @return Popup mode
     */
    PopupMode getPopupMode() const;

    /**
     * Set popup mode to \p popupMode
     * @param popupMode Popup mode
     */
    void setPopupMode(const PopupMode& popupMode);

    /**
     * Get task scope
     * @return Task scope
     */
    Task::GuiScope getTaskScope() const;

    /**
     * Get popup menu
     * @return Reference to popup menu
     */
    QMenu& getMenu();

    /**
     * Get popup force hidden
     * @return Boolean determining whether popup force hidden is on or not
     */
    bool getPopupForceHidden() const;

    /**
     * Set popup force hidden to \p popupForceHidden
     * @param popupForceHidden Boolean determining whether popup force hidden is on or not
     */
    void setPopupForceHidden(bool popupForceHidden);

    /**
     * Get tasks model
     * @return Reference to tasks model
     */
    AbstractTasksModel& getTasksModel();

    /**
     * Get tasks filter model
     * @return Reference to tasks filter model
     */
    TasksFilterModel& getTasksFilterModel();

signals:

    /**
     * Signals that popup force hidden to \p popupForceHidden
     * @param popupForceHidden Boolean determining whether popup force hidden is on or not
     */
    void popupForceHiddenChanged(bool popupForceHidden);

private:
    AbstractTasksModel& _tasksModel;            /** Tasks model */
    TasksFilterModel    _tasksFilterModel;      /** Filter model of the tasks model */
    PopupMode           _popupMode;             /** Popup mode */
    Task::GuiScope      _taskGuiScope;          /** Tasks GUI scope filter */
    QMenu               _menu;                  /** Popup menu */
    bool                _popupForceHidden;      /** Boolean determining whether popup force hidden is on or not */
};

}
