// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DecimalAction.h"

#include <QHBoxLayout>

using namespace mv::util;

namespace mv::gui {

DecimalAction::DecimalAction(QObject * parent, const QString& title, float minimum /*= INIT_MIN*/, float maximum /*= INIT_MAX*/, float value /*= INIT_VALUE*/, std::uint32_t numberOfDecimals /*= INIT_NUMBER_OF_DECIMALS*/) :
    NumericalAction<float>(parent, title, minimum, maximum, value, numberOfDecimals),
    _singleStep(0.1f)
{
    _valueChanged               = [this]() -> void { emit valueChanged(_value); };
    _minimumChanged             = [this]() -> void { emit minimumChanged(_minimum); };
    _maximumChanged             = [this]() -> void { emit maximumChanged(_maximum); };
    _prefixChanged              = [this]() -> void { emit prefixChanged(_prefix); };
    _suffixChanged              = [this]() -> void { emit suffixChanged(_suffix); };
    _numberOfDecimalsChanged    = [this]() -> void { emit numberOfDecimalsChanged(_numberOfDecimals); };

    initialize(minimum, maximum, value, numberOfDecimals);
}

void DecimalAction::initialize(float minimum, float maximum, float value, std::uint32_t numberOfDecimals /*= INIT_NUMBER_OF_DECIMALS*/)
{
    _minimum            = std::min(minimum, _maximum);
    _maximum            = std::max(maximum, _minimum);
    _value              = std::max(_minimum, std::min(value, _maximum));
    _numberOfDecimals   = numberOfDecimals;

    _minimumChanged();
    _maximumChanged();
    _valueChanged();

    if (_numberOfDecimalsChanged)
        _numberOfDecimalsChanged();
}

float DecimalAction::getSingleStep() const
{
    return _singleStep;
}

void DecimalAction::setSingleStep(float singleStep)
{
    if (singleStep == _singleStep)
        return;

    _singleStep = std::max(0.0f, singleStep);

    emit singleStepChanged(_singleStep);
}

WidgetAction* DecimalAction::getPublicCopy() const
{
    if (auto publicCopy = dynamic_cast<NumericalAction<float>*>(WidgetAction::getPublicCopy())) {
        publicCopy->setRange(getRange());
        return publicCopy;
    }

    return nullptr;
}

void DecimalAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicDecimalAction = dynamic_cast<DecimalAction*>(publicAction);

    Q_ASSERT(publicDecimalAction != nullptr);

    if (publicDecimalAction == nullptr)
        return;

    connect(this, &DecimalAction::valueChanged, publicDecimalAction, [publicDecimalAction](float value) -> void {
        publicDecimalAction->setValue(value);
    });

    connect(publicDecimalAction, &DecimalAction::valueChanged, this, [this](float value) -> void {
        setValue(value);
    });

    setValue(publicDecimalAction->getValue());

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void DecimalAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicDecimalAction = dynamic_cast<DecimalAction*>(getPublicAction());

    Q_ASSERT(publicDecimalAction != nullptr);

    if (publicDecimalAction == nullptr)
        return;

    disconnect(this, &DecimalAction::valueChanged, publicDecimalAction, nullptr);
    disconnect(publicDecimalAction, &DecimalAction::valueChanged, this, nullptr);

    WidgetAction::disconnectFromPublicAction(recursive);
}

void DecimalAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");

    setValue(static_cast<float>(variantMap["Value"].toDouble()));

    if (isPublic()) {
        if (variantMap.contains("Minimum"))
			setMinimum(static_cast<float>(variantMap["Minimum"].toDouble()));

        if (variantMap.contains("Maximum"))
            setMaximum(static_cast<float>(variantMap["Maximum"].toDouble()));
    }
}

QVariantMap DecimalAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "Value", QVariant::fromValue(static_cast<double>(getValue())) }
    });

    if (isPublic()) {
        variantMap.insert({
			{ "Minimum", QVariant::fromValue(static_cast<double>(getMinimum())) },
            { "Maximum", QVariant::fromValue(static_cast<double>(getMaximum())) }
        });
    }

    return variantMap;
}

DecimalAction::SpinBoxWidget::SpinBoxWidget(QWidget* parent, DecimalAction* decimalAction) :
    QDoubleSpinBox(parent)
{
    setObjectName("SpinBox");

    connect(this, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, decimalAction](double value) {
        decimalAction->setValue(value);
    });

    const auto valueString = [](double value, std::uint32_t decimals) -> QString {
        return QString::number(value, 'f', decimals);
    };

    const auto setToolTips = [this, decimalAction, valueString]() {
        setToolTip(QString("%1: %2%3").arg(decimalAction->text(), valueString(decimalAction->getValue(), decimalAction->getNumberOfDecimals()), decimalAction->getSuffix()));
    };

    const auto onUpdateValue = [this, decimalAction, setToolTips]() {
        const auto actionValue = decimalAction->getValue();

        if (actionValue == value())
            return;
        
        QSignalBlocker blocker(this);

        setValue(actionValue);

        setToolTips();
    };

    const auto onUpdateValueRange = [this, decimalAction]() {
        QSignalBlocker blocker(this);

        setRange(decimalAction->getMinimum(), decimalAction->getMaximum());
    };

    const auto onUpdatePrefix = [this, decimalAction, setToolTips]() {
        QSignalBlocker blocker(this);

        setPrefix(decimalAction->getPrefix());

        setToolTips();
    };

    const auto onUpdateSuffix = [this, decimalAction, setToolTips]() {
        QSignalBlocker blocker(this);

        setSuffix(decimalAction->getSuffix());

        setToolTips();
    };

    const auto onUpdateDecimals = [this, decimalAction]() {
        QSignalBlocker blocker(this);

        setDecimals(decimalAction->getNumberOfDecimals());
    };

    const auto onUpdateSingleStep = [this, decimalAction]() {
        QSignalBlocker blocker(this);

        setSingleStep(decimalAction->getSingleStep());
    };

    connect(decimalAction, &DecimalAction::minimumChanged, this, onUpdateValueRange);
    connect(decimalAction, &DecimalAction::maximumChanged, this, onUpdateValueRange);
    connect(decimalAction, &DecimalAction::prefixChanged, this, onUpdatePrefix);
    connect(decimalAction, &DecimalAction::suffixChanged, this, onUpdateSuffix);
    connect(decimalAction, &DecimalAction::numberOfDecimalsChanged, this, onUpdateDecimals);
    connect(decimalAction, &DecimalAction::singleStepChanged, this, onUpdateSingleStep);
    connect(decimalAction, &DecimalAction::valueChanged, this, onUpdateValue);

    onUpdateValueRange();
    onUpdateValue();
    onUpdatePrefix();
    onUpdateSuffix();
    onUpdateDecimals();
    onUpdateSingleStep();
    setToolTips();
}

DecimalAction::SliderWidget::SliderWidget(QWidget* parent, DecimalAction* decimalAction) :
    QSlider(Qt::Horizontal, parent)
{
    setObjectName("Slider");
    setRange(std::numeric_limits<std::int16_t>::lowest(), std::numeric_limits<std::int16_t>::max());

    const auto sliderIntervalLength = static_cast<float>(maximum() - minimum());

    const auto onUpdateAction = [this, decimalAction, sliderIntervalLength]() -> void {
        const auto sliderValueNormalized    = static_cast<float>(value() - minimum()) / sliderIntervalLength;
        decimalAction->setValue(decimalAction->getMinimum() + (sliderValueNormalized * decimalAction->getIntervalLength()));
    };

    connect(this, &QSlider::valueChanged, this, [this, decimalAction, onUpdateAction](int value) {
        if (!decimalAction->getUpdateDuringDrag())
            return;

        onUpdateAction();
    });

    connect(this, &QSlider::sliderReleased, this, [this, decimalAction, onUpdateAction]() {
        if (decimalAction->getUpdateDuringDrag())
            return;

        onUpdateAction();
    });

    const auto valueString = [](double value, std::uint32_t decimals) -> QString {
        return QString::number(value, 'f', decimals);
    };

    const auto setToolTips = [this, decimalAction, valueString]() {
        const auto toolTip = QString("%1: %2%3").arg(decimalAction->text(), valueString(decimalAction->getValue(), decimalAction->getNumberOfDecimals()), decimalAction->getSuffix());
        
        setToolTip(toolTip);
    };

    const auto onUpdateValue = [this, decimalAction, sliderIntervalLength, setToolTips]() {
        const auto sliderValue = minimum() + (decimalAction->getNormalized() * sliderIntervalLength);

        QSignalBlocker blocker(this);

        setValue(sliderValue);

        setToolTips();
    };

    connect(decimalAction, &DecimalAction::valueChanged, this, [this, decimalAction, onUpdateValue](float value) {
        onUpdateValue();
    });

    onUpdateValue();
    setToolTips();
}

DecimalAction::LineEditWidget::LineEditWidget(QWidget* parent, DecimalAction* decimalAction) :
    QLineEdit(parent),
    _validator()
{
    setObjectName("LineEdit");

    _validator.setNotation(QDoubleValidator::StandardNotation);

    setValidator(&_validator);

    const auto updateValidator = [this, decimalAction]() -> void {
        _validator.setBottom(decimalAction->getMinimum());
        _validator.setTop(decimalAction->getMaximum());
    };

    updateValidator();

    connect(decimalAction, &DecimalAction::minimumChanged, this, updateValidator);
    connect(decimalAction, &DecimalAction::maximumChanged, this, updateValidator);

    const auto updateText = [this, decimalAction]() -> void {
        QSignalBlocker blocker(this);

        setText(decimalAction->getPrefix() + ": " + QString::number(decimalAction->getValue(), 'f', decimalAction->getNumberOfDecimals()));
    };

    updateText();

    connect(decimalAction, &DecimalAction::valueChanged, this, updateText);
    connect(decimalAction, &DecimalAction::prefixChanged, this, updateText);

    connect(this, &QLineEdit::textChanged, this, [decimalAction](const QString& text) -> void {
        decimalAction->setValue(text.toFloat());
    });
}

QWidget* DecimalAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
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
