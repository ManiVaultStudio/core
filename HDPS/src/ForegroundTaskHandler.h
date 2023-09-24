// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskHandler.h"

#include "actions/TasksAction.h"
#include "actions/ToggleAction.h"

#include <QToolButton>
#include <QMenu>

class QMainWindow;

namespace hdps {

/**
 * Foreground task handler class
 *
 * Contains a bespoke tool button for the application that displays running foreground tasks in a popup.
 *
 * @author Thomas Kroes
 */
class ForegroundTaskHandler final : public AbstractTaskHandler
{
public:

    /* Custom tool button class with a popup widget attached to it which show the foreground task(s) */
    class StatusBarButton : public QToolButton {
    public:

        /**
         * Construct with \p parent widget
         * @param parent Pointer to parent widget
         */
        StatusBarButton(QWidget* parent = nullptr);

        /**
         * Override paint event to customize control drawing
         * @param paintEvent Pointer to paint event that occurred
         */
        void paintEvent(QPaintEvent* paintEvent);

    public: // Action getters

        gui::ToggleAction& getSeeThroughAction() { return _seeThroughAction; }

    private:
        QMenu               _menu;                  /** Popup menu attached to the tool button */
        gui::ToggleAction   _seeThroughAction;      /** Toggle see-through on and off */
    };

protected:

    /** Popup widget which show the foreground tasks, it opens and closes depending on availability of foreground tasks */
    class PopupWidget : public QWidget {
    public:

        /**
         * Construct with pointer to the owning \p foregroundTaskHandler \p statusBarButton and \p parent widget
         * @param foregroundTaskHandler Pointer to the owning foreground task handler
         * @param statusBarButton Pointer to associated status bar button
         * @param parent Pointer to parent widget
         */
        PopupWidget(ForegroundTaskHandler* foregroundTaskHandler, StatusBarButton* statusBarButton, QWidget* parent = nullptr);

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

    private:

        /** Overlays the tasks icon with a badge which reflects the number of foreground tasks */
        void updateStatusBarButtonIcon();
        
        /**
         * Get pointer to the main window
         * @return Pointer to the application main window
         */
        QMainWindow* getMainWindow();

        /** Synchronize the position with the position of the status bar button */
        void synchronizeWithStatusBarButton();

        /** Removes all existing task widget layout items */
        void cleanLayout();

        /** Invoked when the number of foreground changes */
        void numberOfForegroundTasksChanged();

    private:
        ForegroundTaskHandler*  _foregroundTaskHandler;     /** Pointer to the owning foreground task handler */
        StatusBarButton*        _statusBarButton;           /** Pointer to the status bar button to which the popup widget is attached */
        gui::TasksAction        _tasksAction;               /** Tasks action which will be configured to show running foreground tasks */
        QPixmap                 _tasksIconPixmap;           /** Tasks icon pixmap underlay (count badge will be drawn on top) */
        QMap<Task*, QWidget*>   _widgetsMap;                /** Maps task to allocated widget */
        QTimer                  _minimumDurationTimer;      /** Wait for a small amount of time before showing the UI */

        static const QSize iconPixmapSize;
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

    StatusBarButton& getStatusBarButton() { return _statusBarButton; }

private:
    StatusBarButton     _statusBarButton;       /** Status bar button which can be added to a status bar (has a popup widget which shows the foreground tasks) */
    PopupWidget         _popupWidget;           /** Popup widget which show the foreground tasks */
};

}
