#pragma once

#include "WidgetActionOptions.h"

#include <QTreeWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

namespace hdps {

namespace gui {

class GroupAction;
class GroupWidgetTreeItem;

/**
 * Group section tree item class
 *
 * Tree item class for displaying a section push button with which the group contents can be expanded/collapsed
 *
 * @author Thomas Kroes
 */
class GroupSectionTreeItem : public QTreeWidgetItem
{
public:

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
         * @param parent Pointer to parent widget
         */
        SectionPushButton(QTreeWidgetItem* treeWidgetItem, GroupAction* groupAction, QWidget* parent = nullptr);

        /**
         * Respond to target events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override final;

    protected:
        GroupAction*            _widgetActionGroup;         /** Pointer to widget action group */
        QTreeWidgetItem*        _parentTreeWidgetItem;      /** Pointer to parent tree widget item */
        QTreeWidgetItem*        _groupTreeWidgetItem;       /** Pointer to group tree widget item */
        QWidget*                _groupWidget;               /** Pointer to group widget */
        QWidget                 _overlayWidget;             /** Overlay widget for buttons etc. */
        QHBoxLayout             _overlayLayout;             /** Overlay layout */
        QLabel                  _iconLabel;                 /** Left-aligned icon label */
        QLabel                  _settingsLabel;             /** Right-aligned settings label */
        WidgetActionOptions     _widgetActionOptions;       /** Widget action options */
    };

public:

    /**
     * Constructor
     * @param treeWidget Pointer to tree widget
     * @param groupAction Pointer to group action (used for toggling)
     */
    GroupSectionTreeItem(QTreeWidget* treeWidget, GroupAction* groupAction);

protected:
    GroupAction*            _groupAction;               /** Pointer to group action (used for toggling) */
    SectionPushButton       _pushButton;                /** Push button for toggling expansion */
    GroupWidgetTreeItem*    _groupWidgetTreeItem;       /** Pointer to child group widget tree item */
};

}
}
