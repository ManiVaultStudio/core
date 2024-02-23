// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetActionViewWidget.h"

#include <QHBoxLayout>
#include <QToolButton>

namespace mv::gui {

class WidgetAction;

/**
 * Widget action collapsed widget class
 * 
 * Displays a tool button that shows the widget in a popup
 *
 * Note: This action is primarily developed for internal use (not meant to be used explicitly in third-party plugins)
 * 
 * @author Thomas Kroes
 */
class WidgetActionCollapsedWidget : public WidgetActionViewWidget
{
protected:

    /** Tool button class with custom paint behavior */
    class ToolButton : public QToolButton {
    public:

        /**
         * Create with pointer to owning \p action and \p parent widget
         * @param action Pointer to owning action
         * @param parent Pointer to parent widget
         */
        ToolButton(WidgetAction* action, QWidget* parent = nullptr);

        /**
         * Paint event
         * @param paintEvent Pointer to paint event
         */
        void paintEvent(QPaintEvent* paintEvent);

    private:
        WidgetAction*   _action;    /** Pointer to owning action */
    };

public:

    /**
     * Construct with pointer to \parent widget and pointer to owning \p action
     * @param parent Parent widget
     * @param action Pointer to the widget action that will be displayed in a popup
     */
    WidgetActionCollapsedWidget(QWidget* parent, WidgetAction* action);

    /**
     * Construct with pointer to \parent widget and pointer to owning \p action
     * @param parent Parent widget
     * @param action Pointer to the widget action that will be displayed in a popup
     * @param widgetConfigurationFunction This function is called right after the action widget is created
     */
    WidgetActionCollapsedWidget(QWidget* parent, WidgetAction* action, WidgetConfigurationFunction widgetConfigurationFunction);

    /**
     * Set the source action
     * @param widgetAction Pointer to source action
     */
    void setAction(WidgetAction* action) override;

    /** Get reference to the tool button */
    ToolButton& getToolButton() { return _toolButton; }

    /**
     * Get widget configuration function
     * @return Function that is called right after a widget action widget is created
     */
    WidgetConfigurationFunction getWidgetConfigurationFunction();

private:

    /**
     * Startup initialization
     * @param action Action to display as a popup widget of a tool button
     */
    void initialize(WidgetAction* action);

private:
    QHBoxLayout                     _layout;                        /** Layout */
    ToolButton                      _toolButton;                    /** Tool button for the popup */
    WidgetConfigurationFunction     _widgetConfigurationFunction;   /** Function that is called right after a widget action widget is created */

    friend class WidgetAction;
};

}
