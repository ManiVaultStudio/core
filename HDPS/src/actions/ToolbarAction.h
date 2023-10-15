// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GroupAction.h"
#include "ToolbarActionItem.h"

namespace mv::gui {

/**
 * Toolbar action class
 *
 * Base toolbar action for displaying actions
 *
 * @author Thomas Kroes
 */
class ToolbarAction : public WidgetAction
{
    Q_OBJECT

public:

    using AutoExpandPriorities  = QVector<std::int32_t>;
    using ActionItems           = QVector<ToolbarActionItem*>;
    using ActionItemsMap        = QMap<const WidgetAction*, ToolbarActionItem*>;

public:

    /**
     * Construct with \p parent and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param alignment Item alignment
     */
    Q_INVOKABLE ToolbarAction(QObject* parent, const QString& title, const Qt::AlignmentFlag& alignment = Qt::AlignmentFlag::AlignLeft);

protected:

    /**
     * Get group action
     * @return Reference to group action
     */
    virtual GroupAction& getGroupAction() final;

    /**
     * Get action items for the toolbar
     * @return Vector of action items
     */
    virtual ActionItems& getActionItems();

public: // Actions management

    /**
     * Add \p action to the group
     * @param action Pointer to action to add
     * @param autoExpandPriority Priority with which the action should be auto-expanded
     * @param widgetFlags Action widget flags (default flags if -1)
     */
    virtual void addAction(WidgetAction* action, const std::int32_t& autoExpandPriority = -1, std::int32_t widgetFlags = -1) final;

    /**
     * Remove \p action from the group
     * @param action Pointer to action to add
     */
    virtual void removeAction(WidgetAction* action) final;

    /**
     * Get actions
     * @return Vector of pointers to actions
     */
    virtual WidgetActions getActions() final;

    /**
     * Set \p action auto expand priority to \p autoExpandPriority
     * @param action Pointer to action for which to set the auto expand priority
     * @param autoExpandPriority Priority with which the action should be auto-expanded
     */
    virtual void setActionAutoExpandPriority(const WidgetAction* action, const std::int32_t& autoExpandPriority) final;

private:

    /** Invalidates the layout (might be prohibited by the ) */
    void invalidateLayout();

signals:

    /**
     * Signals that the action widgets changed to \p actionWidgets
     * @param actionWidgets New action widgets
     */
    void actionWidgetsChanged(ActionItemsMap& actionWidgets);

    /** Signals that the layout has become invalid due to changes in auto expand priorities or change in widget size */
    void layoutInvalidated();

private:
    GroupAction         _groupAction;       /** Group action which holds the actions */
    ActionItems         _actionItems;       /** Action items */
    ActionItemsMap      _actionItemsMap;    /** Maps action pointer to action item pointer */

protected:
    static constexpr std::int32_t CONTENTS_MARGIN = 4;      /** Content margins around the toolbar */
};

}

Q_DECLARE_METATYPE(mv::gui::ToolbarAction)

inline const auto toolbarActionMetaTypeId = qRegisterMetaType<mv::gui::ToolbarAction*>("mv::gui::ToolbarAction");