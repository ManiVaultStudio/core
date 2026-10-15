// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/HorizontalGroupAction.h>
#include <actions/StringAction.h>
#include <actions/WidgetAction.h>
#include <actions/WidgetActionToolButton.h>

#include <widgets/OverlayWidget.h>

#include <QToolButton>
#include <QMenu>

namespace mv::gui {

/**
 * Status bar action class
 *
 * Base status bar action class with popup functionality
 *
 * There are two methods to add content to the status bar popup:
 * 1. By setting a popup action: setPopupAction(...)
 * 2. By adding menu actions: addMenuAction(...)
 *
 * The index controls the location w.r.t. the other status bar items:
 * - When negative, the item is inserted in between existing items(right-to-left)
 * - Wen zero, the item is appended to the existing items at the right
 * - Wen positive, the item is inserted in between existing items(left-to-right)
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT StatusBarAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for display of a status bar action */
    class CORE_EXPORT Widget : public mv::gui::WidgetActionWidget {
    private:

        class ToolButton : public mv::gui::WidgetActionToolButton {
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
        QWidget*            _popupWidget;       /** Current popup widget (maybe nullptr) */

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
     * Construct with \p parent object, \p title and possibly an \p icon
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param icon Icon
     */
    StatusBarAction(QObject* parent, const QString& title, const util::StyledIcon& icon = util::StyledIcon());

    /**
     * Construct with \p parent object, \p title and possibly a FontAwesome \p icon
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param icon FontAwesome icon name
     */
    StatusBarAction(QObject* parent, const QString& title, const QString& icon = "");

    /** Destructor for removing this action from StatusBarAction#statusBarActions */
    ~StatusBarAction() override;

    /**
     * Get registered status bar action
     * @return Status bar actions
     */
    static WidgetActions getStatusBarActions();

public:

    /**
     * Get bar group action
     * @return Reference to bar group action
     */
    HorizontalGroupAction& getBarGroupAction();

    /**
     * Get bar icon string action
     * @return Reference to bar icon string action
     */
    StringAction& getBarIconStringAction();

    /**
     * Get popup action
     * @return Pointer to popup action (maybe nullptr)
     */
    WidgetAction* getPopupAction();

    /**
     * Set popup action to \p popupAction
     * @param popupAction Pointer to popup action
     */
    void setPopupAction(WidgetAction* popupAction);

    /**
     * Add \p menuAction
     * @param menuAction Pointer to menu action
     */
    void addMenuAction(WidgetAction* menuAction);

    /**
     * Remove \p menuAction
     * @param menuAction Pointer to menu action to remove
     */
    void removeMenuAction(WidgetAction* menuAction);

    /**
     * Get menu actions
     * @return Pointers to menu actions
     */
    WidgetActions getMenuActions();

protected: // Show/hide

    /** Show the popup */
    void showPopup();

    /** Hide the popup */
    void hidePopup();

public: // Positioning

    /**
     * Get index (must be called prior to main window showing, otherwise it will not have an effect)
     * @return Position index
     */
    std::int32_t getIndex();

    /**
     * Set index to \p index
     * @param index Position index
     */
    void setIndex(std::int32_t index);

private:

    /**
     * Initialize the status bar action and applies the \p icon
     * @param icon Icon to use
     */
    void initialize(const QIcon& icon);

signals:

    /**
     * Signals that the popup action changed from \p previousPopupAction to \p popupAction
     * @param previousPopupAction Pointer to previous action (maybe nullptr)
     * @param popupAction Pointer to current action (maybe nullptr)
     */
    void popupActionChanged(WidgetAction* previousPopupAction, WidgetAction* popupAction);

    /**
     * Signals that the position \p index changed
     * @param index Position index
     */
    void indexChanged(std::int32_t index);

    /**
     * Signals that \p menuAction was added
     * @param menuAction Pointer to menu action that was added
     */
    void menuActionAdded(WidgetAction* menuAction);

    /**
     * Signals that \p menuAction is about to be removed
     * @param menuAction Pointer to menu action is about to be removed
     */
    void menuActionAboutToBeRemoved(WidgetAction* menuAction);

    /** Signals that a popup should be displayed */
    void requirePopupShow();

    /** Signals that a popup should be hidden */
    void requirePopupHide();

private:
    HorizontalGroupAction   _barGroupAction;    /** Bar group action */
    StringAction            _iconAction;        /** String action for showing a home icon with FontAwesome */
    WidgetAction*           _popupAction;       /** Pointer to popup action (maybe nullptr) */
    WidgetActions           _menuActions;       /** Menu actions for the popup */
    std::int32_t            _index;             /** Position in the status bar where the action is inserted (zero: append, negative; right-to-left, positive: left-to-right) */

    static WidgetActions statusBarActions;

    friend class WidgetActionToolButton;
};

}