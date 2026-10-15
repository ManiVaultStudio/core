// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetActionViewWidget.h"
#include "WidgetActionToolButton.h"

#include "WidgetAction.h"

#include <QHBoxLayout>

namespace mv::gui {

/**
 * Widget action collapsed widget class
 * 
 * Displays a tool button that shows the widget in a popup
 *
 * Note: This action is developed for internal use only
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT WidgetActionCollapsedWidget final : public WidgetActionViewWidget
{
public:

    /**
     * Construct with pointer to \parent widget and pointer to owning \p action
     * @param parent Parent widget
     * @param action Pointer to the widget action that will be displayed in a popup
     */
    WidgetActionCollapsedWidget(QWidget* parent, WidgetAction* action);

    /**
     * Construct with pointer to \parent widget, pointer to owning \p action and \p widgetConfigurationFunction
     * @param parent Parent widget
     * @param action Pointer to the widget action that will be displayed in a popup
     * @param widgetConfigurationFunction This function is called right after the popup action widget is created
     */
    WidgetActionCollapsedWidget(QWidget* parent, WidgetAction* action, WidgetConfigurationFunction widgetConfigurationFunction);

    /**
     * Set the action to \p action
     * @param widgetAction Pointer to action
     */
    void setAction(WidgetAction* action) override;

    /**
     * Get tool button
     * @return Get reference to the tool button
     */
    WidgetActionToolButton& getToolButton() {
        return _toolButton;
    }

private:

    /**
     * IsStartup initialization
     * @param action Action to display as a popup widget of a tool button
     */
    void initialize(WidgetAction* action);

private:
    QHBoxLayout             _layout;        /** Layout */
    WidgetActionToolButton  _toolButton;    /** Tool button for the popup */

    friend class WidgetAction;
};

}
