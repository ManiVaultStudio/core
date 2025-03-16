// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DecimalRectangleAction.h"
#include "CoreInterface.h"

namespace mv::gui {

DecimalRectangleAction::DecimalRectangleAction(QObject * parent, const QString& title) :
    RectangleAction<DecimalRangeAction>(parent, title)
{
    _rectangleChanged = [this]() -> void {
        if (isRectangleChangedCallBackBlocked())
            return;

        emit rectangleChanged(getLeft(), getRight(), getBottom(), getTop());
    };

    connect(&getRangeAction(Axis::X), &DecimalRangeAction::rangeChanged, this, [this]() -> void { _rectangleChanged(); });
    connect(&getRangeAction(Axis::Y), &DecimalRangeAction::rangeChanged, this, [this]() -> void { _rectangleChanged(); });
}

void DecimalRectangleAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicDecimalRectangleAction = dynamic_cast<DecimalRectangleAction*>(publicAction);

    Q_ASSERT(publicDecimalRectangleAction != nullptr);

    if (publicDecimalRectangleAction == nullptr)
        return;

    publicDecimalRectangleAction->setRectangle(getLeft(), getRight(), getBottom(), getTop());

    connect(this, &DecimalRectangleAction::rectangleChanged, publicDecimalRectangleAction, [this, publicDecimalRectangleAction](float left, float right, float bottom, float top) -> void {
        publicDecimalRectangleAction->setRectangle(left, right, bottom, top);
        });

    connect(publicDecimalRectangleAction, &DecimalRectangleAction::rectangleChanged, this, [this](float left, float right, float bottom, float top) -> void {
        setRectangle(left, right, bottom, top);
        });
    

    RectangleAction<DecimalRangeAction>::connectToPublicAction(publicAction, recursive);
}

void DecimalRectangleAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        actions().disconnectPrivateActionFromPublicAction(&getRangeAction(Axis::X), recursive);
        actions().disconnectPrivateActionFromPublicAction(&getRangeAction(Axis::Y), recursive);
    }

    RectangleAction<DecimalRangeAction>::disconnectFromPublicAction(recursive);
}

void DecimalRectangleAction::fromVariantMap(const QVariantMap& variantMap)
{
    RectangleAction<DecimalRangeAction>::fromVariantMap(variantMap);

    getRangeAction(Axis::X).fromParentVariantMap(variantMap);
    getRangeAction(Axis::Y).fromParentVariantMap(variantMap);
}

QVariantMap DecimalRectangleAction::toVariantMap() const
{
    auto variantMap = RectangleAction<DecimalRangeAction>::toVariantMap();

    getRangeAction(Axis::X).insertIntoVariantMap(variantMap);
    getRangeAction(Axis::Y).insertIntoVariantMap(variantMap);

    return variantMap;
}

}
