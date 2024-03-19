// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StretchAction.h"

namespace mv::gui {

StretchAction::StretchAction(QObject* parent, const QString& title, std::int32_t stretch /*= 1*/) :
    WidgetAction(parent, title)
{
    setStretch(stretch);
}

void StretchAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicStretchAction = dynamic_cast<StretchAction*>(publicAction);

    Q_ASSERT(publicStretchAction != nullptr);

    if (publicStretchAction == nullptr)
        return;

    connect(this, &StretchAction::stretchChanged, publicStretchAction, &StretchAction::setStretch);
    connect(publicStretchAction, &StretchAction::stretchChanged, this, &StretchAction::setStretch);

    setStretch(publicStretchAction->getStretch());

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void StretchAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicStretchAction = dynamic_cast<StretchAction*>(getPublicAction());

    Q_ASSERT(publicStretchAction != nullptr);

    if (publicStretchAction == nullptr)
        return;

    disconnect(this, &StretchAction::stretchChanged, publicStretchAction, &StretchAction::setStretch);
    disconnect(publicStretchAction, &StretchAction::stretchChanged, this, &StretchAction::setStretch);

    WidgetAction::disconnectFromPublicAction(recursive);
}

void StretchAction::fromVariantMap(const mv::VariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);
}

mv::VariantMap StretchAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    return variantMap;
}

}
