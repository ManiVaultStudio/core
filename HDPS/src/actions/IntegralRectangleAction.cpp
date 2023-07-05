// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "IntegralRectangleAction.h"
#include "CoreInterface.h"

namespace hdps::gui {

IntegralRectangleAction::IntegralRectangleAction(QObject* parent, const QString& title, const QRect& rectangle /*= QRect()*/) :
    RectangleAction<QRect, IntegralRangeAction>(parent, title, rectangle)
{
    _rectangleChanged = [this]() -> void { emit rectangleChanged(getRectangle()); };
}

void IntegralRectangleAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicIntegralRectangleAction = dynamic_cast<IntegralRectangleAction*>(publicAction);

    Q_ASSERT(publicIntegralRectangleAction != nullptr);

    if (publicIntegralRectangleAction == nullptr)
        return;

    if (recursive) {
        actions().connectPrivateActionToPublicAction(&getRangeAction(Axis::X), &publicIntegralRectangleAction->getRangeAction(Axis::X), recursive);
        actions().connectPrivateActionToPublicAction(&getRangeAction(Axis::Y), &publicIntegralRectangleAction->getRangeAction(Axis::Y), recursive);
    }

    RectangleAction<QRect, IntegralRangeAction>::connectToPublicAction(publicAction, recursive);
}

void IntegralRectangleAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        actions().disconnectPrivateActionFromPublicAction(&getRangeAction(Axis::X), recursive);
        actions().disconnectPrivateActionFromPublicAction(&getRangeAction(Axis::Y), recursive);
    }

    RectangleAction<QRect, IntegralRangeAction>::disconnectFromPublicAction(recursive);
}

void IntegralRectangleAction::fromVariantMap(const QVariantMap& variantMap)
{
    RectangleAction<QRect, IntegralRangeAction>::fromVariantMap(variantMap);

    getRangeAction(Axis::X).fromParentVariantMap(variantMap);
    getRangeAction(Axis::Y).fromParentVariantMap(variantMap);
}

QVariantMap IntegralRectangleAction::toVariantMap() const
{
    auto variantMap = RectangleAction<QRect, IntegralRangeAction>::toVariantMap();

    getRangeAction(Axis::X).insertIntoVariantMap(variantMap);
    getRangeAction(Axis::Y).insertIntoVariantMap(variantMap);

    return variantMap;
}

}