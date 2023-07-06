// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DecimalRangeAction.h"
#include "Application.h"
#include "CoreInterface.h"

#include <QDebug>
#include <QHBoxLayout>

namespace hdps::gui {

#if (__cplusplus < 201703L)   // definition needed for pre C++17 gcc and clang
    constexpr float  DecimalRangeAction::INIT_LIMIT_MIN;
    constexpr float  DecimalRangeAction::INIT_LIMIT_MAX;
    constexpr float  DecimalRangeAction::INIT_RANGE_MIN;
    constexpr float  DecimalRangeAction::INIT_RANGE_MAX;
    constexpr float  DecimalRangeAction::INIT_NUMBER_OF_DECIMALS;
#endif

DecimalRangeAction::DecimalRangeAction(QObject* parent, const QString& title, const util::NumericalRange<float>& limits /*= util::NumericalRange<float>(INIT_LIMIT_MIN, INIT_LIMIT_MAX)*/, const util::NumericalRange<float>& range /*= util::NumericalRange<float>(INIT_RANGE_MIN, INIT_RANGE_MAX)*/, std::int32_t numberOfDecimals /*= INIT_NUMBER_OF_DECIMALS*/) :
    NumericalRangeAction(parent, title, limits, range)
{
    _limitsChanged  = [this]() -> void { emit limitsChanged(getLimits()); };
    _rangeChanged   = [this]() -> void { emit rangeChanged(getRange()); };

    getRangeMinAction().setNumberOfDecimals(numberOfDecimals);
    getRangeMaxAction().setNumberOfDecimals(numberOfDecimals);

    connect(&getRangeMinAction(), &DecimalAction::valueChanged, this, [this](const float& value) -> void {
        if (value >= _rangeMaxAction.getValue())
            _rangeMaxAction.setValue(value);

        emit rangeChanged({ _rangeMinAction.getValue(), _rangeMaxAction.getValue() });
    });

    connect(&getRangeMaxAction(), &DecimalAction::valueChanged, this, [this](const float& value) -> void {
        if (value <= _rangeMinAction.getValue())
            _rangeMinAction.setValue(value);

        emit rangeChanged({ _rangeMinAction.getValue(), _rangeMaxAction.getValue() });
    });
}

QWidget* DecimalRangeAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if (widgetFlags & WidgetFlag::MinimumSpinBox)
        layout->addWidget(_rangeMinAction.createWidget(widget, DecimalAction::SpinBox), 1);

    if (widgetFlags & WidgetFlag::MinimumLineEdit)
        layout->addWidget(_rangeMinAction.createWidget(widget, DecimalAction::LineEdit));

    if (widgetFlags & WidgetFlag::Slider) {
        auto slidersLayout = new QHBoxLayout();

        slidersLayout->addWidget(_rangeMinAction.createWidget(widget, DecimalAction::Slider), 2);
        slidersLayout->addWidget(_rangeMaxAction.createWidget(widget, DecimalAction::Slider), 2);

        layout->addLayout(slidersLayout);
    }

    if (widgetFlags & WidgetFlag::MaximumSpinBox)
        layout->addWidget(_rangeMaxAction.createWidget(widget, DecimalAction::SpinBox), 1);

    if (widgetFlags & WidgetFlag::MaximumLineEdit)
        layout->addWidget(_rangeMaxAction.createWidget(widget, DecimalAction::LineEdit));

    widget->setLayout(layout);

    return widget;
}

void DecimalRangeAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicDecimalRangeAction = dynamic_cast<DecimalRangeAction*>(publicAction);

    Q_ASSERT(publicDecimalRangeAction != nullptr);

    if (publicDecimalRangeAction == nullptr)
        return;

    if (recursive) {
        actions().connectPrivateActionToPublicAction(&_rangeMinAction, &publicDecimalRangeAction->getRangeMinAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_rangeMaxAction, &publicDecimalRangeAction->getRangeMaxAction(), recursive);
    }

    NumericalRangeAction::connectToPublicAction(publicAction, recursive);
}

void DecimalRangeAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        actions().disconnectPrivateActionFromPublicAction(&_rangeMinAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_rangeMaxAction, recursive);
    }
    
    NumericalRangeAction::disconnectFromPublicAction(recursive);
}

DecimalRangeAction::DecimalRangeWidget::DecimalRangeWidget(QWidget* parent, DecimalRangeAction* decimalRangeAction, const std::int32_t& widgetFlags /*= 0*/) :
    WidgetActionWidget(parent, decimalRangeAction, widgetFlags)
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(decimalRangeAction->getRangeMinAction().createWidget(this, DecimalAction::SpinBox));
    layout->addWidget(decimalRangeAction->getRangeMaxAction().createWidget(this, DecimalAction::SpinBox));

    setLayout(layout);
}

}
