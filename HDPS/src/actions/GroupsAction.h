#pragma once

#include "GroupAction.h"

#include <QPushButton>

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
         * Section push button class
         *
         * Push button for expanding/collapsing items in an accordion widget for group actions
         *
         * @author Thomas Kroes
         */
        class SectionPushButton : public QPushButton {
        public:

            /**
             * Constructor
             * @param treeWidgetItem Pointer to tree widget item
             * @param widgetActionGroup Pointer to widget action group
             * @param text Section name
             * @param parent Pointer to parent widget
             */
            SectionPushButton(QTreeWidgetItem* treeWidgetItem, GroupAction* groupAction, const QString& text, QWidget* parent = nullptr);

        protected:
            GroupAction*        _widgetActionGroup;     /** Pointer to widget action group */
            QTreeWidgetItem*    _treeWidgetItem;        /** Pointer to widget action group */
        };

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
     * @param state State of the widget (for stateful widgets)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags, const hdps::gui::WidgetActionWidget::State& state = hdps::gui::WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    GroupsAction(QObject* parent);

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

protected:
    GroupActions   _groupActions;      /** Pointers to group actions */
};

}
}
