// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DecimalRangeAction.h"
#include "Application.h"
#include "CoreInterface.h"

#include <QDebug>
#include <QHBoxLayout>

namespace mv::gui {

DecimalRangeAction::DecimalRangeAction(QObject* parent, const QString& title, const util::NumericalRange<float>& limits /*= util::NumericalRange<float>(INIT_LIMIT_MIN, INIT_LIMIT_MAX)*/, const util::NumericalRange<float>& range /*= util::NumericalRange<float>(INIT_RANGE_MIN, INIT_RANGE_MAX)*/, std::int32_t numberOfDecimals /*= INIT_NUMBER_OF_DECIMALS*/) :
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

    getRangeMinAction().setNumberOfDecimals(numberOfDecimals);
    getRangeMaxAction().setNumberOfDecimals(numberOfDecimals);

    connect(&getRangeMinAction(), &DecimalAction::minimumChanged, this, _limitsChanged);
    connect(&getRangeMinAction(), &DecimalAction::maximumChanged, this, _limitsChanged);
    connect(&getRangeMaxAction(), &DecimalAction::minimumChanged, this, _limitsChanged);
    connect(&getRangeMaxAction(), &DecimalAction::maximumChanged, this, _limitsChanged);

    connect(&getRangeMinAction(), &DecimalAction::valueChanged, this, [this](const float& value) -> void {
        if (value >= getRangeMaxAction().getValue())
            getRangeMaxAction().setValue(value);

        _rangeChanged();
    });

    connect(&getRangeMaxAction(), &DecimalAction::valueChanged, this, [this](const float& value) -> void {
        if (value <= getRangeMinAction().getValue())
            getRangeMinAction().setValue(value);

        _rangeChanged();
    });
}

QWidget* DecimalRangeAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if (widgetFlags & WidgetFlag::MinimumSpinBox)
        layout->addWidget(getRangeMinAction().createWidget(widget, DecimalAction::SpinBox), 1);

    if (widgetFlags & WidgetFlag::MinimumLineEdit)
        layout->addWidget(getRangeMinAction().createWidget(widget, DecimalAction::LineEdit));

    if (widgetFlags & WidgetFlag::Slider) {
        auto slidersLayout = new QHBoxLayout();

        slidersLayout->addWidget(getRangeMinAction().createWidget(widget, DecimalAction::Slider), 2);
        slidersLayout->addWidget(getRangeMaxAction().createWidget(widget, DecimalAction::Slider), 2);

        layout->addLayout(slidersLayout);
    }

    if (widgetFlags & WidgetFlag::MaximumSpinBox)
        layout->addWidget(getRangeMaxAction().createWidget(widget, DecimalAction::SpinBox), 1);

    if (widgetFlags & WidgetFlag::MaximumLineEdit)
        layout->addWidget(getRangeMaxAction().createWidget(widget, DecimalAction::LineEdit));

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
        actions().connectPrivateActionToPublicAction(&getRangeMinAction(), &publicDecimalRangeAction->getRangeMinAction(), recursive);
        actions().connectPrivateActionToPublicAction(&getRangeMaxAction(), &publicDecimalRangeAction->getRangeMaxAction(), recursive);
    }

    NumericalRangeAction::connectToPublicAction(publicAction, recursive);
}

void DecimalRangeAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        actions().disconnectPrivateActionFromPublicAction(&getRangeMinAction(), recursive);
        actions().disconnectPrivateActionFromPublicAction(&getRangeMaxAction(), recursive);
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
