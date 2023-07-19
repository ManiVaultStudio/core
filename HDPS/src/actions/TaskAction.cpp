// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TaskAction.h"
#include "Application.h"

namespace hdps::gui {

TaskAction::TaskAction(QObject* parent, const QString& title /*= ""*/) :
    GroupAction(parent, title),
    _progressAction(this, "Progress"),
    _killTaskAction(this, "Kill")
{
    setShowLabels(false);
    setDefaultWidgetFlags(GroupAction::Horizontal | WidgetFlag::Default);
    setConnectionPermissionsToForceNone(true);

    _progressAction.setStretch(1);

    _killTaskAction.setIcon(Application::getIconFont("FontAwesome").getIcon("times"));
    _killTaskAction.setDefaultWidgetFlags(TriggerAction::Icon);

    addAction(&_progressAction);
    addAction(&_killTaskAction);
}

}
