// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/WidgetAction.h>

#include <widgets/OverlayWidget.h>

#include <QToolButton>
#include <QMenu>

/**
 * Status bar action class
 *
 * Base status bar action class with popup functionality
 *
 * @author Thomas Kroes
 */
class StatusBarAction : public mv::gui::WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for display of a status bar action */
    class Widget : public mv::gui::WidgetActionWidget {
    private:

        class ToolButton : public QToolButton {
        public:
            ToolButton(QWidget* parent, StatusBarAction* statusBarAction);

        private:
            StatusBarAction* _statusBarAction;      /** Pointer to owning status bar action */
        };

    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param statusBarAction Pointer to owning status bar action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        Widget(QWidget* parent, StatusBarAction* statusBarAction, const std::int32_t& widgetFlags);

    protected:
        StatusBarAction*    _statusBarAction;   /** Pointer to owning status bar action */
        ToolButton          _toolButton;        /** Customized tool button */
        QMenu               _toolButtonMenu;    /** Tool button menu */

        friend class StatusBarAction;
    };

protected:

    /**
     * Get widget representation of the status bar action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    StatusBarAction(QObject* parent, const QString& title);

protected:

    /**
     * Set bar action to \p barAction
     * @param barAction Pointer to bar action
     */
    void setBarAction(WidgetAction* barAction);

    /**
     * Set popup action to \p popupAction
     * @param popupAction Pointer to popup action
     */
    void setPopupAction(WidgetAction* popupAction);

    /**
     * Get bar action
     * @return Pointer to bar action
     */
    WidgetAction* getBarAction();

    /**
     * Get popup action
     * @return Pointer to popup action (maybe nullptr)
     */
    WidgetAction* getPopupAction();

signals:

    /**
     * Signals that the popup action changed from \p previousPopupAction to \p popupAction
     * @param previousPopupAction Pointer to previous action (maybe nullptr)
     * @param popupAction Pointer to current action (maybe nullptr)
     */
    void popupActionChanged(WidgetAction* previousPopupAction, WidgetAction* popupAction);

private:
    WidgetAction*   _barAction;       /** Pointer to bar action */
    WidgetAction*   _popupAction;     /** Pointer to popup action (maybe nullptr) */
};
