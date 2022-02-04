#pragma once

#include "GroupAction.h"
#include "StringAction.h"
#include "TriggerAction.h"

class QWidget;

namespace hdps {

namespace gui {

/**
 * Groups action class
 *
 * Contains a list of group actions and displays them in an accordion widget
 *
 * @author Thomas Kroes
 */
class GroupsAction : public WidgetAction
{
    Q_OBJECT

public:
    using GroupActions = QVector<GroupAction*>;

public:

    /** Describes the widget configurations */
    enum WidgetFlag {
        Filtering       = 0x00001,      /** Widget contains UI for filtering group actions */
        Expansion       = 0x00002,      /** Widget contains UI to expand/collapse all actions */
        Serialization   = 0x00004,      /** Widget includes UI for serialization */

        Default = Filtering | Expansion | Serialization
    };

public:

    class TreeWidget : public QTreeWidget
    {
    public:
        void updateGeometries()
        {
            QTreeWidget::updateGeometries();
        }
    };

    /** Group widget class for widget action groups */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param groupsAction Pointer to groups action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        Widget(QWidget* parent, GroupsAction* groupsAction, const std::int32_t& widgetFlags);

    protected: // Internals

        /**
         * Create toolbar for interaction with the groups
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        void createToolbar(const std::int32_t& widgetFlags);

        /**
         * Create tree widget for display of the groups
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        void createTreeWidget(const std::int32_t& widgetFlags);

        /** Updates the state of the toolbar */
        void updateToolbar();

        /** Updates the property filtering */
        void updateFiltering();

    private:

        /**
         * Add a group action to the view
         * @param groupAction Pointer to the group action
         */
        void addGroupAction(GroupAction* groupAction);

        /**
         * Removes a group action from the view
         * @param groupAction Pointer to the group action
         */
        void removeGroupAction(GroupAction* groupAction);

    protected:
        GroupsAction*       _groupsAction;              /** Pointer to groups action */
        QVBoxLayout         _layout;                    /** Main layout */
        GroupAction         _filteredActionsAction;     /** Group action for filtered actions */
        QWidget             _toolbarWidget;             /** Toolbar widget */
        QHBoxLayout         _toolbarLayout;             /** Toolbar layout */
        StringAction        _filterAction;              /** Filter action */
        TriggerAction       _expandAllAction;           /** Expand all datasets action */
        TriggerAction       _collapseAllAction;         /** Collapse all datasets action */
        TriggerAction       _loadDefaultAction;         /** Load default action */
        TriggerAction       _saveDefaultAction;         /** Save default action */
        TriggerAction       _factoryDefaultAction;      /** Restore factory default action */
        TreeWidget          _treeWidget;                /** Tree widget for display of the groups */

        friend class GroupsAction;
    };

    /**
     * Get widget representation of the groups action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    GroupsAction(QObject* parent);

public: // Adding/removing group action(s)

    /**
     * Appends a group action
     * @param groupAction Group action to add
     */
    void addGroupAction(GroupAction* groupAction);

    /**
     * Removes a group action
     * @param groupAction Group action to remove
     */
    void removeGroupAction(GroupAction* groupAction);

    /**
     * Set group actions (overwrites existing group actions)
     * @param groupActions Vector of pointers to group actions
     */
    void setGroupActions(const GroupActions& groupActions);

    /**
     * Reset group actions
     */
    void resetGroupActions();

    /** Get group actions */
    const GroupActions& getGroupActions() const;

public: // Expand/collapse group(s)

    /**
     * Get whether one (or more) groups can be expanded
     * @return Whether one (or more) groups can be expanded
     */
    bool canExpandAll() const;

    /**
     * Expand all group actions
     */
    void expandAll();

    /**
     * Get whether one (or more) groups can be expanded
     * @return Whether one (or more) groups can be expanded
     */
    bool canCollapseAll() const;

    /**
     * Collapse all group actions
     */
    void collapseAll();

    /**
     * Get groups expansion states
     * @return Vector of booleans, indicating whether a group is expanded or collapsed
     */
    QVector<bool> getExpansions() const;

    /**
     * Get number of expanded group actions
     * @return Number of expanded group actions
     */
    std::int32_t getNumberOfExpandedGroupActions() const;

signals:

    /**
     * Signals that a group action was added
     * @param groupAction Group action that was added
     */
    void groupActionAdded(GroupAction* groupAction);

    /**
     * Signals that a group action was removed
     * @param groupAction Group action that was removed
     */
    void groupActionRemoved(GroupAction* groupAction);

    /**
     * Signals that a group action got expanded
     * @param groupAction Group action that got expanded
     */
    void groupActionExpanded(GroupAction* groupAction);

    /**
     * Signals that a group action got collapsed
     * @param groupAction Group action that got collapsed
     */
    void groupActionCollapsed(GroupAction* groupAction);

protected:
    GroupActions    _groupActions;      /** Pointers to group actions */
};

}
}
