// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QMenu>

namespace mv::gui {

class WidgetAction;
class WidgetActionToolButton;

/**
 * Widget action tool button menu
 *
 * Specialized tool button menu class which show an action widget as a popup
 *
 * Note: This action is developed for internal use (not meant to be used explicitly in third-party plugins)
 *
 * @author Thomas Kroes
 */
class WidgetActionToolButtonMenu : public QMenu
{
private:

    class DeferredWidgetAction : public QWidgetAction
    {
    private:

        /** Widget class which initializes with an empty vertical box layout and populates when the menu is about to be shown */
        class ActionWidget : public QWidget
        {
        public:

            /**
             * Create with reference to \p widgetActionToolButton
             * @param widgetActionToolButton Reference to widget action tool button
             */
            ActionWidget(QWidget* parent, WidgetActionToolButton& widgetActionToolButton);

            void initialize();
            QSize sizeHint() const override;
        private:
            WidgetActionToolButton&     _widgetActionToolButton;        /** ----TODO---- */
            QWidget*                    _widget;
        };

    public:

        /**
         * Create with reference to \p widgetActionToolButton
         * @param widgetActionToolButton Reference to widget action tool button
         */
        DeferredWidgetAction(WidgetActionToolButton& widgetActionToolButton);

        /**
         * Create widget with pointer to \p parent widget
         * @param parent Pointer to parent widget
         * @return Pointer to created widget
         */
        QWidget* createWidget(QWidget* parent) override;

        ActionWidget& getActionWidget() { return _actionWidget; }

        WidgetActionToolButton& getWidgetActionToolButton() { return _widgetActionToolButton; }

    private:
        WidgetActionToolButton&     _widgetActionToolButton;        /** ----TODO---- */
        ActionWidget                _actionWidget;
    };

public:

    /**
     * Create with reference to owning \p widgetActionToolButton and pointer to \p action
     * @param widgetActionToolButton Reference to owning widget action tool button
     */
    WidgetActionToolButtonMenu(WidgetActionToolButton& widgetActionToolButton);

    /**
     * Get widget configuration function
     * @return Function that is called right after a widget action widget is created
     */
    WidgetConfigurationFunction getWidgetConfigurationFunction();

    QWidget* getWidget() { return _widget; }

    DeferredWidgetAction& getDeferredWidgetAction() { return _widgetAction; }
private:

    /**
     * Startup initialization
     * @param action Action to display as a popup widget of a tool button
     */
    void initialize(WidgetAction* action);

private:
    WidgetActionToolButton&         _widgetActionToolButton;        /** ----TODO---- */
    DeferredWidgetAction            _widgetAction;                  /** ----TODO---- */
    QWidget* _widget;
    WidgetConfigurationFunction     _widgetConfigurationFunction;   /** Function that is called right after a widget action widget is created */
};

}
