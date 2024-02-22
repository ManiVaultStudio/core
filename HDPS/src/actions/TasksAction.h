// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/WidgetAction.h"

#include "models/TasksFilterModel.h"

#include "widgets/HierarchyWidget.h"

#include <QPixmap>

namespace mv::gui {

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

    /** Describes the widget flags */
    enum WidgetFlag {
        Tree    = 0x00001,      /** Includes a widget that show tasks in a tree */
        Popup   = 0x00002       /** Includes a widget that is optimized for display in a popup */
    };

public:

    /** Tasks action tree widget */
    class TreeWidget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param tasksAction Pointer to tasks action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        TreeWidget(QWidget* parent, TasksAction* tasksAction, const std::int32_t& widgetFlags);

    private:

        /** Updates the tree view in response to changes in the tasks filter model */
        void updateTreeView();

    private:
        TasksAction*        _tasksAction;   /** Pointer to owning tasks action */
        HierarchyWidget     _tasksWidget;   /** Show the tasks in a hierarchy widget */

        friend class TasksAction;
    };

    /** Tasks action popup widget */
    class PopupWidget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param tasksAction Pointer to tasks action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        PopupWidget(QWidget* parent, TasksAction* tasksAction, const std::int32_t& widgetFlags);

    private:

        void cleanLayout();
        void updateLayout();

    private:
        TasksAction*                    _tasksAction;   /** Pointer to owning tasks action */
        QMap<Task*, QVector<QWidget*>>  _widgetsMap;    /** Maps task to allocated widget */

        friend class TasksAction;
    };

protected:

    /**
     * Get widget representation of the color action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE TasksAction(QObject* parent, const QString& title);

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

    /**
     * Open any progress editor which has not been opened yet in the filtered tasks view recursively
     * @param itemView Item view to set the persistent editors for
     * @param parent Parent model index
     */
    void openPersistentProgressEditorsRecursively(QAbstractItemView& itemView, const QModelIndex& parent = QModelIndex());

    /**
     * Close any progress editor in the filtered tasks view recursively
     * @param itemView Item view to set the persistent editors for
     * @param parent Parent model index
     */
    void closePersistentProgressEditorsRecursively(QAbstractItemView& itemView, const QModelIndex& parent = QModelIndex());

    /**
     * get whether any aggregate tasks are displayed
     * @return Boolean determining whether there are any aggregate tasks are displayed
     */
    bool hasAgregateTasks() const;

signals:

    /**
     * Signals that auto hide kill column changed to 
     * @param autoHideKillCollumn Boolean determining whether the kill column is automatically hidden when there are no killable tasks
     */
    void autoHideKillCollumnChanged(bool autoHideKillCollumn);

private:
    TasksFilterModel    _tasksFilterModel;      /** Filter model for tasks */
    QPixmap             _tasksIconPixmap;       /** Tasks icon pixmap underlay (count badge will be drawn on top) */
    std::int32_t        _rowHeight;             /** Row height in pixels */
    std::int32_t        _progressColumnMargin;  /** Progress column margin */
    bool                _autoHideKillCollumn;   /** When true, the kill column is automatically hidden when there are no killable tasks */

    static const QSize tasksIconPixmapSize;
};

}

Q_DECLARE_METATYPE(mv::gui::TasksAction)

inline const auto tasksActionMetaTypeId = qRegisterMetaType<mv::gui::TasksAction*>("mv::gui::TasksAction");
