// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TaskAction.h"

#include <QMenu>
#include <QHBoxLayout>

namespace hdps::gui {

TaskAction::TaskAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent, title)
{
    setDefaultWidgetFlags(WidgetFlag::Default);
    setConnectionPermissionsToForceNone(true);
}

QWidget* TaskAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    if (dynamic_cast<QMenu*>(parent))
        return QWidgetAction::createWidget(parent);

    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    //layout->setContentsMargins(0, 0, 0, 0);

    //if (widgetFlags & WidgetFlag::CheckBox)
    //    layout->addWidget(new ToggleAction::CheckBoxWidget(parent, this));

    //if (widgetFlags & WidgetFlag::PushButton)
    //    layout->addWidget(new ToggleAction::PushButtonWidget(parent, this, widgetFlags));

    widget->setLayout(layout);

    return widget;
}

}
