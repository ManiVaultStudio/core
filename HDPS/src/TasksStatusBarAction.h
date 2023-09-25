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

namespace hdps {
    class AbstractTasksModel;
}

namespace hdps::gui {

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
     */
    TasksStatusBarAction(AbstractTasksModel& tasksModel, QObject* parent, const QString& title, const PopupMode& popupMode = PopupMode::Click);

    PopupMode getPopupMode() const;

    void setPopupMode(const PopupMode& popupMode);

    QMenu& getMenu();

    bool getPopupForceHidden() const;
    void setPopupForceHidden(bool popupForceHidden);

    AbstractTasksModel& getTasksModel();
    TasksFilterModel& getTasksFilterModel();

signals:

    void popupForceHiddenChanged(bool popupForceHidden);

private:
    AbstractTasksModel& _tasksModel;
    TasksFilterModel    _tasksFilterModel;
    PopupMode           _popupMode;
    QMenu               _menu;
    bool                _popupForceHidden;
};

}
