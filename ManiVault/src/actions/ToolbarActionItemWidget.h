// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "util/WidgetFader.h"

#include "ToolbarActionItem.h"

#include <QWidget>
#include <QPointer>

namespace mv::gui {

/**
 * Toolbar action item widget class
 *
 * @author Thomas Kroes
 */
class ToolbarActionItemWidget final : public QWidget
{
    Q_OBJECT

protected:

    /** State widget class (displays either collapsed or expanded action) */
    class StateWidget : public QWidget
    {
    public:

        /**
         * Construct with \p parent widget, pointer to \p action to display and \p state
         * @param parent Pointer to parent widget
         * @param action Pointer to action to display
         * @param widgetFlags Flags to use in widget creation
         * @param state State in which to display the \p action
         */
        StateWidget(QWidget* parent, WidgetAction* action, std::int32_t widgetFlags, const ToolbarActionItem::State& state);

        /**
         * Overload set visible to enabled fading
         * @param visible whether the state widget is visible or not
         */
        void setVisible(bool visible);

    private:
        //util::WidgetFader   _widgetFader;   /** For fading in/out the widget */
    };

protected:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param toolbarActionItem Reference to owning toolbar action item
     */
    ToolbarActionItemWidget(QWidget* parent, ToolbarActionItem& toolbarActionItem);

    /**
     * Respond to \p target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

private:

    /**
     * Synchronize widget size for \p state (updates the size in the action item)
     * @param state State to synchronize the widget size for
     */
    void synchronizeWidgetSize(const ToolbarActionItem::State& state);

private:
    ToolbarActionItem&  _toolbarActionItem;     /** Reference to owning toolbar action item */
    StateWidget         _collapsedWidget;       /** State widget for collapsed action */
    StateWidget         _expandedWidget;        /** State widget for expanded action */

    friend class ToolbarActionItem;
};

}
