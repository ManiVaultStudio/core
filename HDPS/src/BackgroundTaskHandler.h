// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskHandler.h"

#include "actions/WidgetAction.h"

namespace hdps {

/**
 * Background task handler class
 *
 * Interact with tasks in a main window status bar popup window.
 *
 * @author Thomas Kroes
 */
class BackgroundTaskHandler final : public AbstractTaskHandler
{
protected:

    /**
     * Foreground tasks action class
     *
     * Tasks action for showing foreground tasks in a customized popup interface.
     *
     * @author Thomas Kroes
     */
    class StatusBarAction : public gui::WidgetAction
    {
        class StatusBarButton : public QToolButton {
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
            StatusBarButton(QWidget* parent = nullptr);

            /**
             * Get widget representation of the foreground tasks action
             * @param parent Pointer to parent widget
             * @param widgetFlags Widget flags for the configuration of the widget (type)
             * Paint event
             * @param paintEvent Pointer to paint event
             */
            QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
                return new Widget(parent, this, widgetFlags);
            };
            void paintEvent(QPaintEvent* paintEvent);

        public: // Action getters

            gui::ToggleAction& getSeeThroughAction() { return _seeThroughAction; }

        private:
            QMenu               _menu;                  /** Popup menu attached to the tool button */
            gui::ToggleAction   _seeThroughAction;
        };

public:

    /**
    * Construct task handler with \p parent object
    * @param parent Pointer to parent object
    * @param task Pointer to task to handle
    */
    BackgroundTaskHandler(QObject* parent, Task* task);

    /** Initializes the handler */
    void init() override;

private:
    friend class QMainWindow;
};

}
