#pragma once

#include "WidgetAction.h"

namespace hdps::gui {

/**
 * Group action class
 *
 * Contains zero or more child actions and either displays them horizontally or vertically.
 * 
 * @author Thomas Kroes
 */
class GroupAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {
        Horizontal  = 0x00001,      /** The child actions are arranged horizontally in the group */
        Vertical    = 0x00002,      /** The child actions are arranged vertically in the group */

        Default = Horizontal
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Group title
     * @param expanded Whether the group is initially expanded/collapsed
     */
    GroupAction(QObject* parent, const QString& title, const bool& expanded = false);

    /** Set expanded/collapsed */
    void setExpanded(const bool& expanded);

    /** Expand the group */
    void expand();

    /** Collapse the group */
    void collapse();

    /** Toggle the group */
    void toggle();

    /** Get whether the group is expanded */
    bool isExpanded() const;

    /** Get whether the group is collapsed */
    bool isCollapsed() const;

    /** Gets the group read-only */
    bool isReadOnly() const;

    /**
     * Sets the group read-only
     * @param readOnly Whether the group is read-only
     */
    void setReadOnly(const bool& readOnly);

    /** Gets whether the group should show labels or not */
    bool getShowLabels() const;

    /**
     * Sets whether the group should show labels or not
     * @param showLabels Whether the group should show labels or not
     */
    void setShowLabels(bool showLabels);

public: // Actions management

    /**
     * Add \p action to the group
     * @param action Pointer to action to add
     */
    void addAction(const WidgetAction* action);

    /**
     * Remove \p action from the group
     * @param action Pointer to action to add
     */
    void removeAction(const WidgetAction* action);

    /**
     * Get const actions
     * @return Const actions
     */
    ConstWidgetActions getActions();

    /**
     * Set actions to \p actions
     * @param actions Widget actions
     */
    void setActions(const ConstWidgetActions& actions);

private:

    /** Sort added actions based on their sort index */
    virtual void sortActions() final;

signals:

    /**
     * Signals that the actions changed to \p actions
     * @param actions Current actions in the group
     */
    void actionsChanged(const ConstWidgetActions& actions);

    /** Signals that the group got expanded */
    void expanded();

    /** Signals that the group got collapsed */
    void collapsed();

    /**
     * Signals that the group read-only status changed to \p readOnly
     * @param readOnly Read-only status
     */
    void readOnlyChanged(const bool& readOnly);

    /**
     * Signals that the group show labels option changed to \p showLabels
     * @param showLabels Whether label are visible or not
     */
    void showLabelsChanged(const bool& showLabels);

private:
    bool                _expanded;      /** Whether or not the group is expanded */
    bool                _readOnly;      /** Whether or not the group is read-only */
    ConstWidgetActions  _actions;       /** Widget actions in the group */
    bool                _showLabels;    /** Whether to show labels or not */
};

}
