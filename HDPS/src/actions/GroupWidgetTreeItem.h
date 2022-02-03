#pragma once

#include "WidgetAction.h"

#include <QTreeWidget>
#include <QVBoxLayout>

class QWidget;

namespace hdps {

namespace gui {

class GroupAction;

/**
 * Group widget tree item class
 * 
 * Tree item class for displaying the contents of a group action
 *
 * @author Thomas Kroes
 */
class GroupWidgetTreeItem : public QTreeWidgetItem
{
    /**
    * Group widget tree item class
    *
    * Tree item class for displaying the contents of a group action
    *
    * @author Thomas Kroes
    */
    class SizeSynchronizer : public QObject
    {
    public:
        SizeSynchronizer(GroupWidgetTreeItem* groupTreeItem);

        /**
         * Respond to target events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override final;

    protected:
        GroupWidgetTreeItem*  _groupTreeItem;
    };

public:
    GroupWidgetTreeItem(QTreeWidgetItem* parentTreeWidgetItem, GroupAction* groupAction);

    QWidget* getWidget();

protected:
    GroupAction*        _groupAction;
    QWidget             _containerWidget;
    QVBoxLayout         _containerLayout;
    QWidget*            _widget;
    SizeSynchronizer    _sizeSynchronizer;
};

}
}
