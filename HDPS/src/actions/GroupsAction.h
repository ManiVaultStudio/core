#pragma once

#include "GroupAction.h"
#include "WidgetActionOptions.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

class QWidget;
class QTreeWidget;
class QTreeWidgetItem;

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

    /** Group widget class for widget action groups */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param groupsAction Pointer to groups action
         */
        Widget(QWidget* parent, GroupsAction* groupsAction);

        friend class GroupsAction;
    };

    /**
     * Get widget representation of the groups action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    GroupsAction(QObject* parent);

    /**
     * Respond to target events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override final;

    /**
     * Adds a vector of group actions
     * @param groupActions Vector of pointers to group actions
     */
    void set(const GroupActions& groupActions);

    /** Get group actions */
    const GroupActions& getGroupActions() const;

signals:

    /** Signals that the groups have changed */
    void changed(const GroupActions& groupActions);

    /**
     * Signals that a group got expanded
     * @param groupAction Group action that got expanded
     */
    void expanded(const GroupAction* groupAction);

    /**
     * Signals that a group got collapsed
     * @param groupAction Group action that got collapsed
     */
    void collapsed(const GroupAction* groupAction);

protected:
    GroupActions    _groupActions;          /** Pointers to group actions */
};

}
}
