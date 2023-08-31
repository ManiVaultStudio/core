// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/WidgetAction.h>

#include "TasksModel.h"
#include "TasksFilterModel.h"

#include <widgets/HierarchyWidget.h>

#include <QPixmap>

namespace hdps::gui {

/**
 * Tasks action class
 *
 * Action class for managing tasks
 *
 * @author Thomas Kroes
 */
class TasksAction final : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {
        Toolbar     = 0x00001,  /** The widget includes a toolbar for filtering etc. */
        Overlay     = 0x00002   /** The widget includes a toolbar for filtering etc. */
    };

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

        /** Resize sections when the model changes */
        void modelChanged();

    private:
        TasksAction*        _tasksAction;   /** Pointer to owning tasks action */
        HierarchyWidget     _tasksWidget;   /** Show the tasks in a hierarchy widget */

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
    Q_INVOKABLE TasksAction(QObject* parent, const QString& title);

    /**
     * Get tasks model
     * @return Reference to the task model
     */
    TasksModel& getTasksModel();

    /**
     * Get tasks filter model
     * @return Reference to the task filter model
     */
    TasksFilterModel& getTasksFilterModel();

    /**
     * Set row height to \p rowHeight
     * @param rowHeight Row height in pixels
     */
    void setRowHeight(std::int32_t rowHeight);

    /** Get row height */
    std::int32_t getRowHeight() const;

    /**
     * Set progress column margin to \p progressColumnMargin
     * @param progressColumnMargin Progress column margin
     */
    void setProgressColumnMargin(std::int32_t progressColumnMargin);

    /** Get progress column margin */
    std::int32_t getProgressColumnMargin() const;

    /**
     * Set auto hide kill column to \p autoHideKillColumn
     * @param autoHideKillColumn Boolean determining whether the kill column is automatically hidden when there are no killable tasks
     */
    void setAutoHideKillCollumn(bool autoHideKillColumn);

    /** Get auto hide kill column */
    bool getAutoHideKillCollumn() const;

private:

    /** Invoked when the filter model layout changes */
    void filterModelChanged();

signals:

    /**
     * Signals that auto hide kill column changed to 
     * @param autoHideKillCollumn Boolean determining whether the kill column is automatically hidden when there are no killable tasks
     */
    void autoHideKillCollumnChanged(bool autoHideKillCollumn);

private:
    TasksModel          _tasksModel;            /** Model with all tasks in the system */
    TasksFilterModel    _tasksFilterModel;      /** Filter model for tasks */
    QPixmap             _tasksIconPixmap;       /** Tasks icon pixmap underlay (count badge will be drawn on top) */
    std::int32_t        _rowHeight;             /** Row height in pixels */
    std::int32_t        _progressColumnMargin;  /** Progress column margin */
    bool                _autoHideKillCollumn;   /** When true, the kill column is automatically hidden when there are no killable tasks */

    static const QSize tasksIconPixmapSize;
};

}

Q_DECLARE_METATYPE(hdps::gui::TasksAction)

inline const auto tasksActionMetaTypeId = qRegisterMetaType<hdps::gui::TasksAction*>("hdps::gui::TasksAction");
