// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "WidgetAction.h"
#include "WidgetActionToolButtonMenu.h"

#include <QToolButton>
#include <QPushButton>

namespace mv::gui {

class WidgetAction;

/**
 * Widget action tool button 
 *
 * Specialized tool button which show an action widget as a popup
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT WidgetActionToolButton : public QToolButton
{
    Q_OBJECT

public:

    /**
     * Create with pointer to \p parent widget and owning \p action
     * @param parent Pointer to parent widget
     * @param action Pointer to owning action
     */
    WidgetActionToolButton(QWidget* parent, WidgetAction* action);

    /**
     * Construct with pointer to \parent widget, pointer to owning \p action and \p widgetConfigurationFunction
     * @param parent Pointer to parent widget
     * @param action Pointer to owning action
     * @param widgetConfigurationFunction This function is called right after the popup action widget is created
     */
    WidgetActionToolButton(QWidget* parent, WidgetAction* action, WidgetConfigurationFunction widgetConfigurationFunction);

    /**
     * Override paint event for custom painting
     * @param paintEvent Pointer to paint event
     */
    void paintEvent(QPaintEvent* paintEvent) override;

    /**
     * Get action
     * @return Pointer to action (maybe nullptr)
     */
    WidgetAction* getAction() const;

    /**
     * Set the action to \p action (ownership is not transferred)
     * @param action Pointer to action
     */
    void setAction(WidgetAction* action);

    /**
     * Get widget configuration function
     * @return Function that is called right after a widget action widget is created
     */
    WidgetConfigurationFunction getWidgetConfigurationFunction();

    /**
     * Get show indicator
     * @return Boolean determining whether to show the indicator or not
     */
    bool getShowIndicator() const {
        return _showIndicator;
    }

    /**
     * Set show indicator to \p showIndicator
     * @param showIndicator Boolean determining whether to show the indicator or not
     */
    void setShowIndicator(bool showIndicator) {
        _showIndicator = showIndicator;

        update();
    }

    /**
     * Get alignment of the indicator
     * @return Alignment of the indicator
     */
    Qt::Alignment getIndicatorAlignment() const {
        return _indicatorAlignment;
    }

    /**
     * Set alignment of the indicator to \p indicatorAlignment
     * @param indicatorAlignment Alignment of the indicator
     */
    void setIndicatorAlignment(Qt::Alignment indicatorAlignment) {
        _indicatorAlignment = indicatorAlignment;

        update();
    }

    WidgetActionToolButtonMenu& getMenu() {
        return _menu;
    }

private:

    /**
     * Startup initialization
     * @param action Action to display as a popup widget of a tool button
     */
    void initialize(WidgetAction* action);

signals:

    /**
     * Signals that the current action changed from \p previousAction to \p currentAction
     * @param previousAction Pointer to previous action (maybe nullptr)
     * @param currentAction Pointer to current action (maybe nullptr)
     */
    void actionChanged(WidgetAction* previousAction, WidgetAction* currentAction);

private:
    WidgetAction*                   _action;                        /** Pointer to action */
    WidgetConfigurationFunction     _widgetConfigurationFunction;   /** Function that is called right after a widget action widget is created */
    bool                            _showIndicator;                 /** Whether to show the indicator or not */
    Qt::Alignment                   _indicatorAlignment;            /** Alignment of the indicator */
    WidgetActionToolButtonMenu      _menu;                          /** Menu with custom action content */
       
    /** Work around improper QToolButton sizing issue */
    static QPushButton* sizeHintPushButton;
};

}
