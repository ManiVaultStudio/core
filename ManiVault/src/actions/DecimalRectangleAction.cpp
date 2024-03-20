// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DecimalRectangleAction.h"
#include "CoreInterface.h"

namespace mv::gui {

DecimalRectangleAction::DecimalRectangleAction(QObject * parent, const QString& title, const QRectF& rectangle /*= QRectF()*/) :
    RectangleAction<QRectF, DecimalRangeAction>(parent, title, rectangle)
{
    _rectangleChanged = [this]() -> void { emit rectangleChanged(getRectangle()); };
}

void DecimalRectangleAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicDecimalRectangleAction = dynamic_cast<DecimalRectangleAction*>(publicAction);

    Q_ASSERT(publicDecimalRectangleAction != nullptr);

    if (publicDecimalRectangleAction == nullptr)
        return;

    if (recursive) {
        actions().connectPrivateActionToPublicAction(&getRangeAction(Axis::X), &publicDecimalRectangleAction->getRangeAction(Axis::X), recursive);
        actions().connectPrivateActionToPublicAction(&getRangeAction(Axis::Y), &publicDecimalRectangleAction->getRangeAction(Axis::Y), recursive);
    }

    RectangleAction<QRectF, DecimalRangeAction>::connectToPublicAction(publicAction, recursive);
}

void DecimalRectangleAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        actions().disconnectPrivateActionFromPublicAction(&getRangeAction(Axis::X), recursive);
        actions().disconnectPrivateActionFromPublicAction(&getRangeAction(Axis::Y), recursive);
    }

    RectangleAction<QRectF, DecimalRangeAction>::disconnectFromPublicAction(recursive);
}

void DecimalRectangleAction::fromVariantMap(const mv::VariantMap& variantMap)
{
    RectangleAction<QRectF, DecimalRangeAction>::fromVariantMap(variantMap);

    getRangeAction(Axis::X).fromParentVariantMap(variantMap);
    getRangeAction(Axis::Y).fromParentVariantMap(variantMap);
}

mv::VariantMap DecimalRectangleAction::toVariantMap() const
{
    auto variantMap = RectangleAction<QRectF, DecimalRangeAction>::toVariantMap();

    getRangeAction(Axis::X).insertIntoVariantMap(variantMap);
    getRangeAction(Axis::Y).insertIntoVariantMap(variantMap);

    return variantMap;
}

}
