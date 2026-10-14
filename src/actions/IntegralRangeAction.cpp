// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "IntegralRangeAction.h"
#include "Application.h"
#include "CoreInterface.h"

#include <QDebug>
#include <QHBoxLayout>

namespace mv::gui {

IntegralRangeAction::IntegralRangeAction(QObject* parent, const QString& title, const util::NumericalRange<std::int32_t>& limits /*= util::NumericalRange<std::int32_t>(INIT_LIMIT_MIN, INIT_LIMIT_MAX)*/, const util::NumericalRange<std::int32_t>& range /*= util::NumericalRange<std::int32_t>(INIT_RANGE_MIN, INIT_RANGE_MAX)*/) :
    NumericalRangeAction(parent, title, limits, range)
{
    _limitsChanged = [this]() -> void {
        if (isLimitsChangedCallBackBlocked())
            return;

        emit limitsChanged(getLimits());
    };

    _rangeChanged = [this]() -> void {
        if (isRangeChangedCallBackBlocked())
            return;

        emit rangeChanged(getRange());
    };

    connect(&getRangeMinAction(), &IntegralAction::minimumChanged, this, _limitsChanged);
    connect(&getRangeMinAction(), &IntegralAction::maximumChanged, this, _limitsChanged);
    connect(&getRangeMaxAction(), &IntegralAction::minimumChanged, this, _limitsChanged);
    connect(&getRangeMaxAction(), &IntegralAction::maximumChanged, this, _limitsChanged);

    connect(&getRangeMinAction(), &IntegralAction::valueChanged, this, [this](const std::int32_t& value) -> void {
        if (value >= getRangeMaxAction().getValue())
            getRangeMaxAction().setValue(value);

        _rangeChanged();
    });

    connect(&getRangeMaxAction(), &IntegralAction::valueChanged, this, [this](const std::int32_t& value) -> void {
        if (value <= getRangeMinAction().getValue())
            getRangeMinAction().setValue(value);

        _rangeChanged();
    });
}

QWidget* IntegralRangeAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if (widgetFlags & WidgetFlag::MinimumSpinBox)
        layout->addWidget(getRangeMinAction().createWidget(widget, IntegralAction::SpinBox), 1);

    if (widgetFlags & WidgetFlag::MinimumLineEdit)
        layout->addWidget(getRangeMinAction().createWidget(widget, IntegralAction::LineEdit));

    if (widgetFlags & WidgetFlag::Slider) {
        auto slidersLayout = new QHBoxLayout();

        slidersLayout->addWidget(getRangeMinAction().createWidget(widget, IntegralAction::Slider), 2);
        slidersLayout->addWidget(getRangeMaxAction().createWidget(widget, IntegralAction::Slider), 2);

        layout->addLayout(slidersLayout);
    }

    if (widgetFlags & WidgetFlag::MaximumSpinBox)
        layout->addWidget(getRangeMaxAction().createWidget(widget, IntegralAction::SpinBox), 1);

    if (widgetFlags & WidgetFlag::MaximumLineEdit)
        layout->addWidget(getRangeMaxAction().createWidget(widget, IntegralAction::LineEdit));

    widget->setLayout(layout);

    return widget;
}

void IntegralRangeAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicIntegralRangeAction = dynamic_cast<IntegralRangeAction*>(publicAction);

    Q_ASSERT(publicIntegralRangeAction != nullptr);

    if (publicIntegralRangeAction == nullptr)
        return;

    if (recursive) {
        actions().connectPrivateActionToPublicAction(&getRangeMinAction(), &publicIntegralRangeAction->getRangeMinAction(), recursive);
        actions().connectPrivateActionToPublicAction(&getRangeMaxAction(), &publicIntegralRangeAction->getRangeMaxAction(), recursive);
    }

    NumericalRangeAction::connectToPublicAction(publicAction, recursive);
}

void IntegralRangeAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        actions().disconnectPrivateActionFromPublicAction(&getRangeMinAction(), recursive);
        actions().disconnectPrivateActionFromPublicAction(&getRangeMaxAction(), recursive);
    }
    
    NumericalRangeAction::disconnectFromPublicAction(recursive);
}

IntegralRangeAction::IntegralRangeWidget::IntegralRangeWidget(QWidget* parent, IntegralRangeAction* integralRangeAction, const std::int32_t& widgetFlags /*= 0*/) :
    WidgetActionWidget(parent, integralRangeAction, widgetFlags)
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(integralRangeAction->getRangeMinAction().createWidget(this, IntegralAction::SpinBox));
    layout->addWidget(integralRangeAction->getRangeMaxAction().createWidget(this, IntegralAction::SpinBox));

    setLayout(layout);
}

}
