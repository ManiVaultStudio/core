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

    /** Customized widget action class which defers layout population until the popup menu is about to be shown */
    class DeferredLoadWidgetAction : public QWidgetAction
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
            ActionWidget(WidgetActionToolButton& widgetActionToolButton);

            /** Populate the widget with the current action widget */
            void initialize();

            /**
             * Get size hint
             * @return Size hint
             */
            QSize sizeHint() const override;

        private:
            WidgetActionToolButton&     _widgetActionToolButton;    /** Reference to the widget action tool button */
            QWidget*                    _widget;                    /** Pointer to the create widget */
        };

    public:

        /**
         * Create with reference to \p widgetActionToolButton
         * @param widgetActionToolButton Reference to widget action tool button
         */
        DeferredLoadWidgetAction(WidgetActionToolButton& widgetActionToolButton);

        /**
         * Create widget with pointer to \p parent widget
         * @param parent Pointer to parent widget
         * @return Pointer to created widget
         */
        QWidget* createWidget(QWidget* parent) override;

        /**
         * Get action widget
         * @return Reference to action widget
         */
        ActionWidget& getActionWidget() {
            return _actionWidget;
        }

        /**
         * Get widget action tool button
         * @return Reference to owning widget action tool button
         */
        WidgetActionToolButton& getWidgetActionToolButton() {
            return _widgetActionToolButton;
        }

    private:
        WidgetActionToolButton&     _widgetActionToolButton;    /** Reference to owning widget action tool button */
        ActionWidget                _actionWidget;              /** Action widget */
    };

public:

    /**
     * Create with reference to owning \p widgetActionToolButton
     * @param widgetActionToolButton Reference to owning widget action tool button
     */
    WidgetActionToolButtonMenu(WidgetActionToolButton& widgetActionToolButton);

    /**
     * Get widget configuration function
     * @return Function that is called right after a widget action widget is created
     */
    WidgetConfigurationFunction getWidgetConfigurationFunction();

    /**
     * Get deferred load widget action
     * @return Reference to deferred load widget action
     */
    DeferredLoadWidgetAction& getDeferredLoadWidgetAction() {
        return _deferredLoadWidgetAction;
    }

    /**
     * Get ignore close event
     * @return Boolean determining whether to ignore the close event
     */
    bool getIgnoreCloseEvent() const;

    /**
     * Set ignore close event to \p ignoreCloseEvent
     * @param ignoreCloseEvent Boolean determining whether to ignore the close event
     */
    void setIgnoreCloseEvent(bool ignoreCloseEvent);

    /**
     * Invoked when the menu is required to be closed
     * @param event Pointer to close event
     */
    void closeEvent(QCloseEvent* event) override;

private:
    WidgetActionToolButton&         _widgetActionToolButton;        /** Reference to owning widget action tool button */
    DeferredLoadWidgetAction        _deferredLoadWidgetAction;      /** Widget class which initializes with an empty vertical box layout and populates when the menu is about to be shown */
    WidgetConfigurationFunction     _widgetConfigurationFunction;   /** Function that is called right after a widget action widget is created */
    bool                            _ignoreCloseEvent;              /** Boolean determining whether to ignore the close event */
};

}
