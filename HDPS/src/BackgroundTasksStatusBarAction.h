// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/HorizontalGroupAction.h"
#include "actions/TaskAction.h"

#include "TasksPopupWidget.h"

#include <QToolButton>

namespace hdps {
    class BackgroundTaskHandler;
}

namespace hdps::gui {

/**
 * Background tasks status bar action class
 *
 * Tasks action class for display and interaction with background tasks.
 *
 * @author Thomas Kroes
 */
class BackgroundTasksStatusBarAction : public HorizontalGroupAction
{
protected:

    class DetailsAction : public WidgetAction
    {
    protected:

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
        };

    public:

        /**
         * Construct with \p parent object and \p title
         * @param parent Pointer to parent object
         * @param title Title of the action
         */
        DetailsAction(QObject* parent, const QString& title);

    protected:

        /**
         * Get widget representation of the color action
         * @param parent Pointer to parent widget
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;
    };

protected:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    BackgroundTasksStatusBarAction(QObject* parent, const QString& title);

public: // Action getters

    gui::TaskAction& getTaskAction() { return _taskAction; }

private:
    BackgroundTaskHandler*  _backgroundTaskHandler;
    TaskAction              _taskAction;
    DetailsAction           _detailsAction;

    friend class hdps::BackgroundTaskHandler;
};

}
