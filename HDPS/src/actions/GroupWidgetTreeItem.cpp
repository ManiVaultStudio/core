#include "GroupWidgetTreeItem.h"
#include "GroupAction.h"

#include <QDebug>
#include <QResizeEvent>

namespace hdps {

namespace gui {

GroupWidgetTreeItem::GroupWidgetTreeItem(QTreeWidgetItem* parentTreeWidgetItem, GroupAction* groupAction) :
    QTreeWidgetItem(parentTreeWidgetItem),
    _groupAction(groupAction),
    _containerWidget(),
    _containerLayout(),
    _widget(groupAction->createWidget(treeWidget())),
    _sizeSynchronizer(this)
{
    parentTreeWidgetItem->addChild(this);

    _containerWidget.setLayout(&_containerLayout);
    _widget->setFixedWidth(treeWidget()->width());

    _containerLayout.setMargin(0);
    _containerLayout.setSizeConstraint(QLayout::SetFixedSize);
    _containerLayout.addWidget(_widget);



    treeWidget()->setItemWidget(this, 0, &_containerWidget);

    QCoreApplication::processEvents();

    setSizeHint(0, _widget->sizeHint());
}

QWidget* GroupWidgetTreeItem::getWidget()
{
    return _widget;
}

GroupWidgetTreeItem::SizeSynchronizer::SizeSynchronizer(GroupWidgetTreeItem* groupTreeItem) :
    QObject(),
    _groupTreeItem(groupTreeItem)
{
    _groupTreeItem->getWidget()->installEventFilter(this);
}

bool GroupWidgetTreeItem::SizeSynchronizer::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            _groupTreeItem->setSizeHint(0, _groupTreeItem->getWidget()->sizeHint());

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}

}
}
