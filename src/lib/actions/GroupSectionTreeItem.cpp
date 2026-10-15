// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "GroupSectionTreeItem.h"
#include "GroupWidgetTreeItem.h"
#include "GroupAction.h"

#include "util/StyledIcon.h"

#include <QResizeEvent>
#include <QOperatingSystemVersion>

//#define GROUP_SECTION_TREE_ITEM_VERBOSE

using namespace mv::util;

namespace mv::gui {

GroupSectionTreeItem::GroupSectionTreeItem(QTreeWidget* treeWidget, GroupAction* groupAction) :
    _groupAction(groupAction),
    _pushButton(new PushButton(this, groupAction)),
    _groupWidgetTreeItem(nullptr)
{
    setSizeHint(0, QSize(0, 24));

    treeWidget->addTopLevelItem(this);
    treeWidget->setItemWidget(this, 0, _pushButton);

    _groupWidgetTreeItem = new GroupWidgetTreeItem(this, groupAction);
}

GroupSectionTreeItem::~GroupSectionTreeItem()
{
#ifdef GROUP_SECTION_TREE_ITEM_VERBOSE
    qDebug() << QString("Destructing %1 group tree item").arg(_groupAction->getSettingsPath());
#endif

    takeChildren();

    delete _groupWidgetTreeItem;
}

GroupSectionTreeItem::PushButton& GroupSectionTreeItem::getPushButton() const
{
    return *_pushButton;
}

GroupAction* GroupSectionTreeItem::getGroupAction() const
{
    return _groupAction;
}

GroupSectionTreeItem::PushButton::PushButton(QTreeWidgetItem* treeWidgetItem, GroupAction* groupAction, QWidget* parent /*= nullptr*/) :
    QPushButton(groupAction->text(), parent),
    _widgetActionGroup(groupAction),
    _groupTreeWidgetItem(nullptr),
    _parentTreeWidgetItem(treeWidgetItem),
    _groupWidget(nullptr),
    _overlayWidget(this)
{
    auto isMacOS = QOperatingSystemVersion::currentType() == QOperatingSystemVersion::MacOS;
    
    // on macOS the buttons look extremely squished and foreign with fixed height
    if (!isMacOS) {
        setFixedHeight(22);
    }

    _overlayWidget.setLayout(&_overlayLayout);
    _overlayWidget.raise();

    _overlayLayout.setContentsMargins(8, 4, 8, 4);
    _overlayLayout.addWidget(&_iconLabel);
    _overlayLayout.addStretch(1);

    _iconLabel.setAlignment(Qt::AlignCenter);

    if (isMacOS) {
        _iconLabel.setFont(util::StyledIcon::getIconFont());
    } else {
        _iconLabel.setFont(util::StyledIcon::getIconFont(7));
    }

    // Install event filter to synchronize overlay widget size with push button size
    installEventFilter(this);

    // Toggle the section expansion when the section push button is clicked
    connect(this, &QPushButton::clicked, this, [this]() {
        _widgetActionGroup->toggle();
    });

    // Update the state of the push button when the group action changes
    const auto update = [this]() -> void {
        if (_widgetActionGroup->isExpanded())
            _parentTreeWidgetItem->setExpanded(true);
        else
            _parentTreeWidgetItem->setExpanded(false);

        // Assign the icon characters
        _iconLabel.setText(StyledIcon::getIconCharacter(_widgetActionGroup->isExpanded() ? "angle-down" : "angle-right"));
    };

    const auto updateText = [this, groupAction]() -> void {
        setText(groupAction->text());
    };

    connect(groupAction, &WidgetAction::changed, this, updateText);

    // Update when the group action is expanded or collapsed
    connect(_widgetActionGroup, &GroupAction::expanded, this, update);
    connect(_widgetActionGroup, &GroupAction::collapsed, this, update);

    update();
}

bool GroupSectionTreeItem::PushButton::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            if (target == this)
                _overlayWidget.setFixedSize(dynamic_cast<QResizeEvent*>(event)->size());

            break;
        }

        default:
            break;
    }

    return QPushButton::eventFilter(target, event);
}

}
