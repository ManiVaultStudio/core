// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QWidget>

#include "actions/TasksAction.h"

class QMainWindow;

namespace hdps::gui {

class TasksStatusBarAction;

/**
 * Tasks popup widget class 
 *
 * Shows tasks in a popup window.
 * 
 * The popup widget can be attached to an anchor widget, or it can act standalone.
 *
 * @author Thomas Kroes
 */
class TasksPopupWidget : public QWidget {
public:

    /**
     * Construct with pointer to the \p anchorWidget, a \p minimumDuration and \p parent widget
     * @param anchorWidget If set, the popup widget will stick to the upper left corner of the anchor widget (right-aligned)
     * @param parent Pointer to parent widget
     * @param minimumDuration Only show the popup when there is at least one task that last longer that this duration
     */
    TasksPopupWidget(gui::TasksStatusBarAction& tasksStatusBarAction, QWidget* anchorWidget = nullptr, QWidget* parent = nullptr, std::uint32_t minimumDuration = 250);

    /**
     * Respond to target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

    /**
     * Override size hint to make widget shrink
     * @return Size hint
     */
    QSize sizeHint() const override;

    void setIcon(const QIcon& icon);

protected:

    /** Update the contents of the popup (automatically invoked when the number of tasks changes) */
    void updateContents();

private:

    /** Overlays the icon with a badge which reflects the number of tasks */
    void updateIcon();
    
    /** Synchronize the position with the position of the anchor widget (if one is set) */
    void synchronizeWithAnchorWidget();

    /**
     * Get pointer to the main window
     * @return Pointer to the application main window
     */
    QMainWindow* getMainWindow();

    /** Removes all existing task widget layout items */
    void cleanLayout();

private:
    gui::TasksStatusBarAction&      _tasksStatusBarAction;      /** Tasks status bar action which contains the tasks to be displayed */
    QWidget*                        _anchorWidget;              /** If set, the popup widget will stick to the upper left corner of the anchor widget (right-aligned) */
    QPixmap                         _tasksIconPixmap;           /** Tasks icon pixmap underlay (count badge will be drawn on top) */
    QMap<Task*, QVector<QWidget*>>  _widgetsMap;                /** Maps task to allocated widget */
    QTimer                          _minimumDurationTimer;      /** Wait for a small amount of time before showing the UI */

    static const QSize iconPixmapSize;

    friend class TasksStatusBarAction;
};

}
