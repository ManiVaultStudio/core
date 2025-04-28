// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "IntegralPointAction.h"

using namespace mv::util;

namespace mv::gui {

IntegralPointAction::IntegralPointAction(QObject* parent, const QString& title) :
    NumericalPointAction<std::int32_t, IntegralAction>(parent, title, std::numeric_limits<std::int32_t>::lowest(), std::numeric_limits<std::int32_t>::max())
{
    for (int axisIndex = 0; axisIndex < static_cast<int>(Axis::Count); ++axisIndex) {
        connect(&getAction(static_cast<NumericalPointAction::Axis>(axisIndex)), &IntegralAction::valueChanged, this, [this](std::int32_t value) -> void {
            emit valueChanged(getX(), getY());
        });
    }
}

void IntegralPointAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicIntegralPointAction = dynamic_cast<IntegralPointAction*>(publicAction);

    Q_ASSERT(publicIntegralPointAction);

    if (!publicIntegralPointAction)
        return;

    connect(this, &IntegralPointAction::valueChanged, publicIntegralPointAction, [publicIntegralPointAction](std::int32_t x, std::int32_t y) -> void {
        publicIntegralPointAction->set(x, y);
	});

    connect(publicIntegralPointAction, &IntegralPointAction::valueChanged, this, [this](std::int32_t x, std::int32_t y) -> void {
        set(x, y);
    });

    set(publicIntegralPointAction->get());

    NumericalPointAction<std::int32_t, IntegralAction>::connectToPublicAction(publicAction, recursive);
}

void IntegralPointAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicIntegralPointAction = dynamic_cast<IntegralPointAction*>(getPublicAction());

    Q_ASSERT(publicIntegralPointAction);

    if (!publicIntegralPointAction)
        return;

    disconnect(this, &IntegralPointAction::valueChanged, publicIntegralPointAction, nullptr);
    disconnect(publicIntegralPointAction, &IntegralPointAction::valueChanged, this, nullptr);

    NumericalPointAction<std::int32_t, IntegralAction>::disconnectFromPublicAction(recursive);
}

}
