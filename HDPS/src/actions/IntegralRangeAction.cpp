// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "IntegralRangeAction.h"
#include "Application.h"
#include "CoreInterface.h"

#include <QDebug>
#include <QHBoxLayout>

namespace hdps::gui {

#if (__cplusplus < 201703L)   // definition needed for pre C++17 gcc and clang
    constexpr std::int32_t IntegralRangeAction::INIT_LIMIT_MIN;
    constexpr std::int32_t IntegralRangeAction::INIT_LIMIT_MAX;
    constexpr std::int32_t IntegralRangeAction::INIT_RANGE_MIN;
    constexpr std::int32_t IntegralRangeAction::INIT_RANGE_MAX;
#endif

IntegralRangeAction::IntegralRangeAction(QObject* parent, const QString& title, const util::NumericalRange<std::int32_t>& limits /*= util::NumericalRange<std::int32_t>(INIT_LIMIT_MIN, INIT_LIMIT_MAX)*/, const util::NumericalRange<std::int32_t>& range /*= util::NumericalRange<std::int32_t>(INIT_RANGE_MIN, INIT_RANGE_MAX)*/) :
    NumericalRangeAction(parent, title, limits, range)
{
    _limitsChanged  = [this]() -> void { emit limitsChanged(getLimits()); };
    _rangeChanged   = [this]() -> void { emit rangeChanged(getRange()); };

    connect(&getRangeMinAction(), &IntegralAction::valueChanged, this, [this](const std::int32_t& value) -> void {
        if (value >= _rangeMaxAction.getValue())
            _rangeMaxAction.setValue(value);

        emit rangeChanged({ _rangeMinAction.getValue(), _rangeMaxAction.getValue() });
    });

    connect(&getRangeMaxAction(), &IntegralAction::valueChanged, this, [this](const std::int32_t& value) -> void {
        if (value <= _rangeMinAction.getValue())
            _rangeMinAction.setValue(value);

        emit rangeChanged({ _rangeMinAction.getValue(), _rangeMaxAction.getValue() });
    });
}

QWidget* IntegralRangeAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if (widgetFlags & WidgetFlag::MinimumSpinBox)
        layout->addWidget(_rangeMinAction.createWidget(widget, IntegralAction::SpinBox), 1);

    if (widgetFlags & WidgetFlag::MinimumLineEdit)
        layout->addWidget(_rangeMinAction.createWidget(widget, IntegralAction::LineEdit));

    if (widgetFlags & WidgetFlag::Slider) {
        auto slidersLayout = new QHBoxLayout();

        slidersLayout->addWidget(_rangeMinAction.createWidget(widget, IntegralAction::Slider), 2);
        slidersLayout->addWidget(_rangeMaxAction.createWidget(widget, IntegralAction::Slider), 2);

        layout->addLayout(slidersLayout);
    }

    if (widgetFlags & WidgetFlag::MaximumSpinBox)
        layout->addWidget(_rangeMaxAction.createWidget(widget, IntegralAction::SpinBox), 1);

    if (widgetFlags & WidgetFlag::MaximumLineEdit)
        layout->addWidget(_rangeMaxAction.createWidget(widget, IntegralAction::LineEdit));

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
        actions().connectPrivateActionToPublicAction(&_rangeMinAction, &publicIntegralRangeAction->getRangeMinAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_rangeMaxAction, &publicIntegralRangeAction->getRangeMaxAction(), recursive);
    }

    NumericalRangeAction::connectToPublicAction(publicAction, recursive);
}

void IntegralRangeAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        actions().disconnectPrivateActionFromPublicAction(&_rangeMinAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_rangeMaxAction, recursive);
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
