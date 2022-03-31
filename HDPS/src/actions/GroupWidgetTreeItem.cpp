#include "GroupWidgetTreeItem.h"
#include "GroupSectionTreeItem.h"
#include "GroupsAction.h"
#include "GroupAction.h"

#include <QDebug>
#include <QResizeEvent>
#include <QScrollBar>
#include <QCoreApplication>

//#define GROUP_WIDGET_TREE_ITEM_VERBOSE

namespace hdps {

namespace gui {

GroupWidgetTreeItem::GroupWidgetTreeItem(GroupSectionTreeItem* groupSectionTreeItem, GroupAction* groupAction) :
    QTreeWidgetItem(groupSectionTreeItem),
    _groupSectionTreeItem(groupSectionTreeItem),
    _groupAction(groupAction),
    _containerWidget(new QWidget(treeWidget())),
    _containerLayout(new QVBoxLayout()),
    _groupWidget(groupAction->createWidget(treeWidget())),
    _sizeSynchronizer(this)
{
    groupSectionTreeItem->addChild(this);

    _containerWidget->setFocusPolicy(Qt::NoFocus);
    _groupWidget->setFocusPolicy(Qt::NoFocus);
    _containerWidget->setLayout(_containerLayout);

    _groupWidget->setFixedWidth(treeWidget()->width());

    _containerLayout->setMargin(0);
    _containerLayout->setSizeConstraint(QLayout::SetFixedSize);
    _containerLayout->addWidget(_groupWidget);

    treeWidget()->setItemWidget(this, 0, _containerWidget);

    QCoreApplication::processEvents();

    setSizeHint(0, _groupWidget->sizeHint());
}

GroupWidgetTreeItem::~GroupWidgetTreeItem()
{
#ifdef GROUP_WIDGET_TREE_ITEM_VERBOSE
    qDebug() << QString("Destructing %1 group widget item").arg(_groupAction->getSettingsPath());
#endif
}

GroupSectionTreeItem* GroupWidgetTreeItem::getGroupSectionTreeItem()
{
    return _groupSectionTreeItem;
}

std::int32_t GroupWidgetTreeItem::getGroupSectionWidth() const
{
    return _groupSectionTreeItem->getPushButton().sizeHint().width();
}

QWidget* GroupWidgetTreeItem::getGroupWidget()
{
    return _groupWidget;
}

GroupWidgetTreeItem::SizeSynchronizer::SizeSynchronizer(GroupWidgetTreeItem* groupTreeItem) :
    QObject(),
    _groupTreeItem(groupTreeItem)
{
    // Synchronize when the section tree push button and/or the group widget changes size
    _groupTreeItem->getGroupSectionTreeItem()->getPushButton().installEventFilter(this);
    _groupTreeItem->getGroupWidget()->installEventFilter(this);
}

bool GroupWidgetTreeItem::SizeSynchronizer::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            // Synchronize group widget width with section push button width
            if (target == &_groupTreeItem->getGroupSectionTreeItem()->getPushButton())
                _groupTreeItem->getGroupWidget()->setFixedWidth(static_cast<QResizeEvent*>(event)->size().width());

            // Synchronize tree item size hint
            if (target == _groupTreeItem->getGroupWidget())
                _groupTreeItem->setSizeHint(0, QSize(_groupTreeItem->getGroupSectionWidth(), _groupTreeItem->getGroupWidget()->sizeHint().height()));

            auto treeWidget = dynamic_cast<GroupsAction::TreeWidget*>(_groupTreeItem->treeWidget());

            treeWidget->updateGeometries();

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}

}
}
