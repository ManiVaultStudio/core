// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "GlobalSettingsGroupAction.h"

#include <QString>

namespace mv
{

using namespace gui;

GlobalSettingsGroupAction::GlobalSettingsGroupAction(QObject* parent, const QString& title, bool expanded /*= true*/) :
    GroupAction(parent, title, expanded)
{
}

QString GlobalSettingsGroupAction::getSettingsPrefix() const
{
    return QString("GlobalSettings/%1/").arg(text());
}


void GlobalSettingsGroupAction::addAction(WidgetAction* action, std::int32_t widgetFlags /*= -1*/, WidgetConfigurationFunction widgetConfigurationFunction /*= WidgetConfigurationFunction()*/)
{
    Q_ASSERT(action != nullptr);

    if (!action)
        return;

    const auto actionName = getSettingsPrefix() + action->getSerializationName();

    auto parts = actionName.split(" ");

    for (int i = 0; i < parts.size(); ++i)
        parts[i].replace(0, 1, parts[i][0].toUpper());

    action->setSettingsPrefix(parts.join(""));

    GroupAction::addAction(action, widgetFlags, widgetConfigurationFunction);
}

}
