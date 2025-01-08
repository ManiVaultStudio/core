// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "VerticalToolbarAction.h"

namespace mv::gui {

VerticalToolbarAction::VerticalToolbarAction(QObject* parent, const QString& title, const Qt::AlignmentFlag& alignment /*= Qt::AlignmentFlag::AlignTo*/) :
    ToolbarAction(parent, title)
{
    getGroupAction().setDefaultWidgetFlags(GroupAction::Vertical);
}

VerticalToolbarAction::Widget::Widget(QWidget* parent, VerticalToolbarAction* verticalToolbarAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, verticalToolbarAction, widgetFlags),
    _verticalToolbarAction(verticalToolbarAction),
    _layout(new QVBoxLayout()),
    _toolbarLayout(new QVBoxLayout()),
    _toolbarWidget()
{
    _toolbarLayout->setContentsMargins(ToolbarAction::CONTENTS_MARGIN, ToolbarAction::CONTENTS_MARGIN, ToolbarAction::CONTENTS_MARGIN, ToolbarAction::CONTENTS_MARGIN);
    _toolbarLayout->setAlignment(Qt::AlignLeft);

    _toolbarWidget.setLayout(_toolbarLayout);
    
    _layout->setSizeConstraint(QLayout::SetFixedSize);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setAlignment(Qt::AlignLeft);
    _layout->addWidget(&_toolbarWidget);
    _layout->addStretch(1);
    
    setLayout(_layout);

    connect(_verticalToolbarAction, &ToolbarAction::actionWidgetsChanged, this, &VerticalToolbarAction::Widget::setActionWidgets);

    setActionWidgets();
}

void VerticalToolbarAction::Widget::setActionWidgets()
{
    QLayoutItem* layoutItem;

    while ((layoutItem = _toolbarLayout->takeAt(0)) != nullptr) {
        delete layoutItem->widget();
        delete layoutItem;
    }

    for (auto actionItem : _verticalToolbarAction->getActionItems())
        _toolbarLayout->addWidget(actionItem->createWidget(&_toolbarWidget));
}

}
