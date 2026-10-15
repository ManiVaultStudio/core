// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "GroupWidgetTreeItem.h"
#include "GroupSectionTreeItem.h"
#include "GroupsAction.h"
#include "GroupAction.h"

#include <QDebug>
#include <QResizeEvent>
#include <QScrollBar>
#include <QCoreApplication>

#ifdef _DEBUG
    //#define GROUP_WIDGET_TREE_ITEM_VERBOSE
#endif

namespace mv::gui {

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

    _containerLayout->setContentsMargins(0, 0, 0, 0);
    _containerLayout->setSizeConstraint(QLayout::SetFixedSize);
    _containerLayout->addWidget(_groupWidget);

    treeWidget()->setItemWidget(this, 0, _containerWidget);

    QCoreApplication::processEvents();

    setSizeHint(0, _groupWidget->sizeHint());
}

GroupWidgetTreeItem::~GroupWidgetTreeItem()
{
#ifdef GROUP_WIDGET_TREE_ITEM_VERBOSE
    qDebug() << __FUNCTION__ << _groupAction->text();
#endif

    if (_groupWidget)
        delete _groupWidget;
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

            /*
            //auto treeWidget = dynamic_cast<GroupsAction::TreeWidget*>(_groupTreeItem->treeWidget());
            //treeWidget->updateGeometries();
            */

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}

}
