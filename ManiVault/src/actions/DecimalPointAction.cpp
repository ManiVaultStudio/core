// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DecimalPointAction.h"

using namespace mv::util;

namespace mv::gui {

DecimalPointAction::DecimalPointAction(QObject* parent, const QString& title) :
    NumericalPointAction<float, DecimalAction>(parent, title, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max())
{
    for (int axisIndex = 0; axisIndex < static_cast<int>(Axis::Count); ++axisIndex) {
        getAction(static_cast<NumericalPointAction::Axis>(axisIndex)).setNumberOfDecimals(2);

        connect(&getAction(static_cast<NumericalPointAction::Axis>(axisIndex)), &DecimalAction::valueChanged, this, [this](float value) -> void {
            emit valueChanged(getX(), getY());
        });
    }
}

void DecimalPointAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicDecimalPointAction = dynamic_cast<DecimalPointAction*>(publicAction);

    Q_ASSERT(publicDecimalPointAction);

    if (!publicDecimalPointAction)
        return;

    connect(this, &DecimalPointAction::valueChanged, publicDecimalPointAction, [publicDecimalPointAction](float x, float y) -> void {
        publicDecimalPointAction->set(x, y);
    });

    connect(publicDecimalPointAction, &DecimalPointAction::valueChanged, this, [this](float x, float y) -> void {
        set(x, y);
    });

    set(publicDecimalPointAction->get());

    NumericalPointAction<float, DecimalAction>::connectToPublicAction(publicAction, recursive);
}

void DecimalPointAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicDecimalPointAction = dynamic_cast<DecimalPointAction*>(getPublicAction());

    Q_ASSERT(publicDecimalPointAction);

    if (!publicDecimalPointAction)
        return;

    disconnect(this, &DecimalPointAction::valueChanged, publicDecimalPointAction, nullptr);
    disconnect(publicDecimalPointAction, &DecimalPointAction::valueChanged, this, nullptr);

    NumericalPointAction<float, DecimalAction>::disconnectFromPublicAction(recursive);
}

}
