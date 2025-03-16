// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "IntegralAction.h"

#include <QHBoxLayout>

using namespace mv::util;

namespace mv::gui {

IntegralAction::IntegralAction(QObject * parent, const QString& title, std::int32_t minimum /*= INIT_MIN*/, std::int32_t maximum /*= INIT_MAX*/, std::int32_t value /*= INIT_VALUE*/) :
    NumericalAction<std::int32_t>(parent, title, minimum, maximum, value)
{
    _valueChanged           = [this]() -> void { emit valueChanged(_value); };
    _minimumChanged         = [this]() -> void { emit minimumChanged(_minimum); };
    _maximumChanged         = [this]() -> void { emit maximumChanged(_maximum); };
    _prefixChanged          = [this]() -> void { emit prefixChanged(_prefix); };
    _suffixChanged          = [this]() -> void { emit suffixChanged(_suffix); };

    initialize(minimum, maximum, value);
}

void IntegralAction::initialize(std::int32_t minimum, std::int32_t maximum, std::int32_t value)
{
    _minimum        = std::min(minimum, _maximum);
    _maximum        = std::max(maximum, _minimum);
    _value          = std::max(_minimum, std::min(value, _maximum));

    _minimumChanged();
    _maximumChanged();
    _valueChanged();
}

void IntegralAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicIntegralAction = dynamic_cast<IntegralAction*>(publicAction);

    Q_ASSERT(publicIntegralAction != nullptr);

    if (publicIntegralAction == nullptr)
        return;

    connect(this, &IntegralAction::valueChanged, publicIntegralAction, [this, publicIntegralAction](const std::int32_t& value) -> void {
        publicIntegralAction->setValue(value, _staySilent);
    });

    connect(publicIntegralAction, &IntegralAction::valueChanged, [this](const std::int32_t& value) -> void {
        setValue(value, _staySilent);
    });

    setValue(publicIntegralAction->getValue());

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void IntegralAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicIntegralAction = dynamic_cast<IntegralAction*>(getPublicAction());

    Q_ASSERT(publicIntegralAction != nullptr);

    if (publicIntegralAction == nullptr)
        return;

    disconnect(this, &IntegralAction::valueChanged, publicIntegralAction, nullptr);
    disconnect(publicIntegralAction, &IntegralAction::valueChanged, this, nullptr);

    WidgetAction::disconnectFromPublicAction(recursive);
}

void IntegralAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");

    setValue(variantMap["Value"].toInt());
}

QVariantMap IntegralAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "Value", QVariant::fromValue(getValue()) }
    });

    return variantMap;
}

IntegralAction::SpinBoxWidget::SpinBoxWidget(QWidget* parent, IntegralAction* integralAction) :
    QSpinBox(parent)
{
    setAcceptDrops(true);
    setObjectName("SpinBox");

    connect(this, qOverload<int>(&QSpinBox::valueChanged), this, [this, integralAction](int value) {
        integralAction->setValue(value);
    });

    const auto valueString = [](std::int32_t value) -> QString {
        return QString::number(value);
    };

    const auto setToolTips = [this, integralAction, valueString]() {
        setToolTip(QString("%1: %2%3").arg(integralAction->text(), valueString(integralAction->getValue()), integralAction->getSuffix()));
    };

    const auto onUpdateValue = [this, integralAction, setToolTips]() {
        const auto actionValue = integralAction->getValue();

        if (actionValue == value())
            return;

        QSignalBlocker doubleSpinBoxBlocker(this);

        setValue(actionValue);

        setToolTips();
    };

    const auto onUpdateValueRange = [this, integralAction]() {
        QSignalBlocker blocker(this);

        setRange(integralAction->getMinimum(), integralAction->getMaximum());
    };

    connect(integralAction, &IntegralAction::minimumChanged, this, [this, integralAction, onUpdateValueRange](const std::int32_t& minimum) {
        onUpdateValueRange();
    });

    connect(integralAction, &IntegralAction::maximumChanged, this, [this, integralAction, onUpdateValueRange](const std::int32_t& maximum) {
        onUpdateValueRange();
    });

    const auto onUpdatePrefix = [this, integralAction, setToolTips]() {
        QSignalBlocker blocker(this);

        setPrefix(integralAction->getPrefix());

        setToolTips();
    };

    const auto onUpdateSuffix = [this, integralAction, setToolTips]() {
        QSignalBlocker blocker(this);

        setSuffix(integralAction->getSuffix());

        setToolTips();
    };

    connect(integralAction, &IntegralAction::prefixChanged, this, onUpdatePrefix);
    connect(integralAction, &IntegralAction::suffixChanged, this, onUpdateSuffix);

    connect(integralAction, &IntegralAction::valueChanged, this, [this, integralAction, onUpdateValue](std::int32_t value) {
        onUpdateValue();
    });

    onUpdateValueRange();
    onUpdateValue();
    onUpdatePrefix();
    onUpdateSuffix();
    setToolTips();
}

IntegralAction::SliderWidget::SliderWidget(QWidget* parent, IntegralAction* integralAction) :
    QSlider(Qt::Horizontal, parent)
{
    setAcceptDrops(true);
    setObjectName("Slider");

    connect(this, &QSlider::valueChanged, this, [this, integralAction](int value) {
        if (!integralAction->getUpdateDuringDrag())
            return;

        integralAction->setValue(value);
    });

    connect(this, &QSlider::sliderReleased, this, [this, integralAction]() {
        if (integralAction->getUpdateDuringDrag())
            return;

        integralAction->setValue(value());
    });

    const auto valueString = [](std::int32_t value) -> QString {
        return QString::number(value);
    };

    const auto setToolTips = [this, integralAction, valueString]() {
        setToolTip(QString("%1: %2%3").arg(integralAction->text(), valueString(integralAction->getValue()), integralAction->getSuffix()));
    };

    const auto onUpdateValue = [this, integralAction, setToolTips]() {
        const auto actionValue = integralAction->getValue();

        if (actionValue == value())
            return;

        QSignalBlocker blocker(this);
        
        setValue(actionValue);

        setToolTips();
    };

    const auto onUpdateValueRange = [this, integralAction]() {
        QSignalBlocker blocker(this);

        setRange(integralAction->getMinimum(), integralAction->getMaximum());
    };

    connect(integralAction, &IntegralAction::minimumChanged, this, [this, integralAction, onUpdateValueRange](const std::int32_t& minimum) {
        onUpdateValueRange();
    });

    connect(integralAction, &IntegralAction::maximumChanged, this, [this, integralAction, onUpdateValueRange](const std::int32_t& maximum) {
        onUpdateValueRange();
    });

    connect(integralAction, &IntegralAction::valueChanged, this, [this, integralAction, onUpdateValue](const std::int32_t& value) {
        onUpdateValue();
    });

    onUpdateValueRange();
    onUpdateValue();
    setToolTips();
}

IntegralAction::LineEditWidget::LineEditWidget(QWidget* parent, IntegralAction* integralAction) :
    QLineEdit(parent),
    _validator()
{
    setObjectName("LineEdit");

    setValidator(&_validator);

    const auto updateValidator = [this, integralAction]() -> void {
        _validator.setBottom(integralAction->getMinimum());
        _validator.setTop(integralAction->getMaximum());
    };

    updateValidator();

    connect(integralAction, &IntegralAction::minimumChanged, this, updateValidator);
    connect(integralAction, &IntegralAction::maximumChanged, this, updateValidator);

    const auto updateText = [this, integralAction]() -> void {
        QSignalBlocker blocker(this);

        setText(integralAction->getPrefix() + QString::number(integralAction->getValue()));
    };

    updateText();

    connect(integralAction, &IntegralAction::valueChanged, this, updateText);
    connect(integralAction, &IntegralAction::prefixChanged, this, updateText);

    connect(this, &QLineEdit::textChanged, this, [integralAction](const QString& text) -> void {
        integralAction->setValue(text.toInt());
    });
}

QWidget* IntegralAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if (widgetFlags & WidgetFlag::SpinBox)
        layout->addWidget(new SpinBoxWidget(parent, this), 1);

    if (widgetFlags & WidgetFlag::Slider)
        layout->addWidget(new SliderWidget(parent, this), 2);

    if (widgetFlags & WidgetFlag::LineEdit)
        layout->addWidget(new LineEditWidget(parent, this), 2);

    widget->setLayout(layout);

    return widget;
}

}
