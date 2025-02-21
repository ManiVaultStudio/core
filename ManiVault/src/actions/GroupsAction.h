// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GroupAction.h"
#include "StringAction.h"
#include "TriggerAction.h"

#include "Task.h"

#include <QStyledItemDelegate>
#include <QTreeWidget>

class QWidget;

namespace mv::gui {

class GroupSectionTreeItem;

/**
 * Groups action class
 *
 * Contains a list of group actions and displays them in an accordion widget
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT GroupsAction : public WidgetAction
{
    Q_OBJECT

public:
    using GroupActions = QVector<GroupAction*>;

public:

    /** Describes the widget configurations */
    enum WidgetFlag {
        Filtering   = 0x00001,      /** Widget contains UI for filtering group actions */
        Expansion   = 0x00002,      /** Widget contains UI to expand/collapse all actions */

        Default = Filtering | Expansion
    };

public:

    /**
     * Tree widget class
     * Implemented solely to be able to update geometries by hand
     */
    class CORE_EXPORT TreeWidget : public QTreeWidget
    {
    public:

        /** Update item geometries */
        void updateGeometries()
        {
            QTreeWidget::updateGeometries();
        }
    };

    /** Group widget class for widget action groups */
    class CORE_EXPORT Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param groupsAction Pointer to groups action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        Widget(QWidget* parent, GroupsAction* groupsAction, const std::int32_t& widgetFlags);

    public:

        /**
         * Get action that contains filtered actions
         * @return Reference to filtered actions action
         */
        GroupAction& getFilteredActionsAction() {
            return _filteredActionsAction;
        }

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

        /**
         * Show group action
         * @param groupAction Pointer to the group action
         */
        void showGroupAction(GroupAction* groupAction);

        /**
         * Hide group action
         * @param groupAction Pointer to the group action
         */
        void hideGroupAction(GroupAction* groupAction);
        
    protected:
        GroupsAction*                                   _groupsAction;                  /** Pointer to groups action */
        QVBoxLayout                                     _layout;                        /** Main layout */
        GroupAction                                     _filteredActionsAction;         /** Group action for filtered actions */
        QWidget                                         _toolbarWidget;                 /** Toolbar widget */
        QHBoxLayout                                     _toolbarLayout;                 /** Toolbar layout */
        StringAction                                    _filterAction;                  /** Filter action */
        TriggerAction                                   _expandAllAction;               /** Expand all datasets action */
        TriggerAction                                   _collapseAllAction;             /** Collapse all datasets action */
        TreeWidget                                      _treeWidget;                    /** Tree widget for display of the groups */
        QMap<GroupAction*, GroupSectionTreeItem*>       _groupSectionTreeItems;         /** Maps group action pointer to group section tree item pointer */

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
     * @param title Title of the groups action
     */
    Q_INVOKABLE GroupsAction(QObject* parent, const QString& title);

    ~GroupsAction();

public: // Adding/removing group action(s)

    /**
     * Appends a group action
     * @param groupAction Group action to add
     * @param visible Whether the group action is visible or not
     */
    void addGroupAction(GroupAction* groupAction, bool visible = true);

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
     * Reset group actions to default
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
     * @param visibleOnly Visible group actions only
     * @return Vector of booleans, indicating whether a group is expanded or collapsed
     */
    QVector<bool> getExpansions(bool visibleOnly = true) const;

    /**
     * Get number of expanded group actions
     * @param visibleOnly Visible group actions only
     * @return Number of expanded group actions
     */
    std::int32_t getNumberOfExpandedGroupActions(bool visibleOnly = true) const;

    /**
     * Get number of visible group actions
     * @return Number of visible group actions
     */
    std::int32_t getNumberOfVisibleGroupActions() const;

public: // Visibility

    /**
     * Hide group action
     * @param groupAction Pointer to group action
     * @param visible Whether the group action is visible
     */
    void setGroupActionVisibility(GroupAction* groupAction, bool visible);

    /**
     * Show group action
     * @param groupAction Pointer to group action
     */
    void showGroupAction(GroupAction* groupAction);

    /**
     * Hide all group actions
     */
    void showAllGroupActions();

    /**
     * Hide group action
     * @param groupAction Pointer to group action
     */
    void hideGroupAction(GroupAction* groupAction);

    /**
     * Hide all group actions
     */
    void hideAllGroupActions();

    /**
     * Get whether the group action is visible
     * @return Whether the group action is visible
     */
    bool isGroupActionVisible(GroupAction* groupAction) const;

    /**
     * Get whether the group action is hidden
     * @return Whether the group action is hidden
     */
    bool isGroupActionHidden(GroupAction* groupAction) const;

public: // Miscellaneous

    /**
     * Get update task
     * @return Task for reporting update progress
     */
    Task& getUpdateTask();

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

    /**
     * Signals that a group action got shown
     * @param groupAction Group action that got shown
     */
    void groupActionShown(GroupAction* groupAction);

    /**
     * Signals that a group action got hidden
     * @param groupAction Group action that got hidden
     */
    void groupActionHidden(GroupAction* groupAction);

protected:
    GroupActions                _groupActions;      /** Pointers to group actions */
    QMap<GroupAction*, bool>    _visibility;        /** Group action visibility */
    Task                        _updateTask;        /** Task for reporting update progress */
};

}

Q_DECLARE_METATYPE(mv::gui::GroupsAction)

inline const auto groupsActionMetaTypeId = qRegisterMetaType<mv::gui::GroupsAction*>("mv::gui::GroupsAction");
