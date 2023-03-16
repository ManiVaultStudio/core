#pragma once

#include "GroupAction.h"
#include "ToolbarActionItem.h"

namespace hdps::gui {

/**
 * Toolbar action class
 *
 * Base toolbar action for displaying actions
 *
 * @author Thomas Kroes
 */
//class ToolbarAction : public WidgetAction
//{
//    Q_OBJECT
//
//public:
//
//    using AutoExpandPriorities  = QVector<std::int32_t>;
//    using ActionItems           = QMap<const WidgetAction*, ToolbarActionItem*>;
//
//public:
//
//    /**
//     * Constructor
//     * @param parent Pointer to parent object
//     * @param title Title of the action
//     */
//    ToolbarAction(QObject* parent, const QString& title = "Toolbar");
//
//    /**
//     * Get type string
//     * @return Widget action type in string format
//     */
//    QString getTypeString() const override;
//
//    /**
//     * Get group action
//     * @return Reference to group action
//     */
//    virtual GroupAction& getGroupAction() final;
//
//    /**
//     * Get action items for the toolbar
//     * @return List of action items
//     */
//    virtual ActionItems& getActionItems();
//
//public: // Actions management
//
//    /**
//     * Add \p action to the group
//     * @param action Pointer to action to add
//     * @param autoExpandPriority Priority with which the action should be auto-expanded
//     */
//    virtual void addAction(const WidgetAction* action, const std::int32_t& autoExpandPriority = -1) final;
//
//    /**
//     * Remove \p action from the group
//     * @param action Pointer to action to add
//     */
//    virtual void removeAction(const WidgetAction* action) final;
//
//    /**
//     * Get const actions
//     * @return Const actions
//     */
//    virtual ConstWidgetActions getActions() final;
//
//    /**
//     * Set \p action auto expand priority to \p autoExpandPriority
//     * @param action Pointer to action for which to set the auto expand priority
//     * @param autoExpandPriority Priority with which the action should be auto-expanded
//     */
//    virtual void setActionAutoExpandPriority(const WidgetAction* action, const std::int32_t& autoExpandPriority) final;
//
//signals:
//
//    /**
//     * Signals that the action widgets changed to \p actionWidgets
//     * @param actionWidgets New action widgets
//     */
//    void actionWidgetsChanged(ActionItems& actionWidgets);
//
//private:
//    GroupAction     _groupAction;   /** Group action which holds the actions */
//    ActionItems     _actionItems;   /** Action items */
//};

}