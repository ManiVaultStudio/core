#include "IntegralAction.h"

#include "../Application.h"

#include <QHBoxLayout>
#include <QSpinBox>
#include <QSlider>
#include <QMenu>

namespace hdps {

namespace gui {

IntegralAction::IntegralAction(QObject * parent, const QString& title, const std::int32_t& minimum /*= MIN_VALUE*/, const std::int32_t& maximum /*= MAX_VALUE*/, const std::int32_t& value /*= VALUE*/, const std::int32_t& defaultValue /*= DEFAULT_VALUE*/) :
    WidgetAction(parent),
    _value(),
    _defaultValue(),
    _minimum(std::numeric_limits<std::int32_t>::min()),
    _maximum(std::numeric_limits<std::int32_t>::max()),
    _suffix(),
    _updateDuringDrag(true)
{
    setText(title);
    setMinimum(minimum);
    setMaximum(maximum);
    setValue(value);
    setDefaultValue(defaultValue);
}

std::int32_t IntegralAction::getValue() const
{
    return _value;
}

void IntegralAction::setValue(const std::int32_t& value)
{
    if (value == _value)
        return;

    _value = std::max(_minimum, std::min(value, _maximum));

    emit valueChanged(_value);
}

std::int32_t IntegralAction::getDefaultValue() const
{
    return _defaultValue;
}

void IntegralAction::setDefaultValue(const std::int32_t& defaultValue)
{
    if (defaultValue == _defaultValue)
        return;

    _defaultValue = std::max(_minimum, std::min(defaultValue, _maximum));

    emit defaultValueChanged(_defaultValue);
}

bool IntegralAction::canReset() const
{
    return _value != _defaultValue;
}

void IntegralAction::reset()
{
    setValue(_defaultValue);
}

std::int32_t IntegralAction::getMinimum() const
{
    return _minimum;
}

void IntegralAction::setMinimum(const std::int32_t& minimum)
{
    if (minimum == _minimum)
        return;

    _minimum = std::min(minimum, _maximum);

    emit minimumChanged(_minimum);
}

std::int32_t IntegralAction::getMaximum() const
{
    return _maximum;
}

void IntegralAction::setMaximum(const std::int32_t& maximum)
{
    if (maximum == _maximum)
        return;

    _maximum = std::max(maximum, _minimum);

    emit maximumChanged(_maximum);
}

void IntegralAction::setRange(const std::int32_t& min, const std::int32_t& max)
{
    setMinimum(min);
    setMaximum(max);
}

QString IntegralAction::getSuffix() const
{
    return _suffix;
}

void IntegralAction::setSuffix(const QString& suffix)
{
    if (suffix == _suffix)
        return;

    _suffix = suffix;

    emit suffixChanged(_suffix);
}

bool IntegralAction::getUpdateDuringDrag() const
{
    return _updateDuringDrag;
}

void IntegralAction::setUpdateDuringDrag(const bool& updateDuringDrag)
{
    if (updateDuringDrag == _updateDuringDrag)
        return;

    _updateDuringDrag = updateDuringDrag;
}

bool IntegralAction::isAtMinimum() const
{
    return _value == _minimum;
}

bool IntegralAction::isAtMaximum() const
{
    return _value == _maximum;
}

IntegralAction::SpinBoxWidget::SpinBoxWidget(QWidget* parent, IntegralAction* integralAction) :
    WidgetAction::Widget(parent, integralAction, Widget::State::Standard),
    _spinBox(new QSpinBox())
{
    _spinBox->setObjectName("SpinBox");

    auto layout = new QHBoxLayout();

    setLayout(layout);

    layout->setMargin(0);
    layout->addWidget(_spinBox);

    connect(_spinBox, qOverload<int>(&QSpinBox::valueChanged), this, [this, integralAction](int value) {
        integralAction->setValue(value);
    });

    const auto valueString = [](const std::int32_t& value) -> QString {
        return QString::number(value);
    };

    const auto setToolTips = [this, integralAction, valueString]() {
        _spinBox->setToolTip(QString("%1: %2%3").arg(integralAction->text(), valueString(integralAction->getValue()), integralAction->getSuffix()));
    };

    const auto onUpdateValue = [this, integralAction, setToolTips]() {
        const auto value = integralAction->getValue();

        if (value != _spinBox->value())
            _spinBox->setValue(value);

        setToolTips();
    };

    const auto onUpdateValueRange = [this, integralAction]() {
        QSignalBlocker spinBoxBlocker(_spinBox);

        _spinBox->setMinimum(integralAction->getMinimum());
        _spinBox->setMaximum(integralAction->getMaximum());
    };

    connect(integralAction, &IntegralAction::minimumChanged, this, [this, integralAction, onUpdateValueRange](const std::int32_t& minimum) {
        onUpdateValueRange();
    });

    connect(integralAction, &IntegralAction::maximumChanged, this, [this, integralAction, onUpdateValueRange](const std::int32_t& maximum) {
        onUpdateValueRange();
    });

    const auto onUpdateSuffix = [this, integralAction, setToolTips]() {
        QSignalBlocker spinBoxBlocker(_spinBox);

        _spinBox->setSuffix(integralAction->getSuffix());

        setToolTips();
    };

    connect(integralAction, &IntegralAction::suffixChanged, this, [this, integralAction, onUpdateSuffix](const QString& suffix) {
        onUpdateSuffix();
    });

    connect(integralAction, &IntegralAction::valueChanged, this, [this, integralAction, onUpdateValue](const std::int32_t& value) {
        onUpdateValue();
    });

    onUpdateValueRange();
    onUpdateValue();
    onUpdateSuffix();
    setToolTips();
}

IntegralAction::SliderWidget::SliderWidget(QWidget* parent, IntegralAction* integralAction) :
    WidgetAction::Widget(parent, integralAction, Widget::State::Standard),
    _slider(new QSlider(Qt::Horizontal))
{
    _slider->setObjectName("Slider");

    auto layout = new QHBoxLayout();

    setLayout(layout);

    layout->setMargin(0);
    layout->addWidget(_slider);

    connect(_slider, &QSlider::valueChanged, this, [this, integralAction](int value) {
        if (!integralAction->getUpdateDuringDrag())
            return;

        integralAction->setValue(value);
    });

    connect(_slider, &QSlider::sliderReleased, this, [this, integralAction]() {
        if (integralAction->getUpdateDuringDrag())
            return;

        integralAction->setValue(_slider->value());
    });

    const auto valueString = [](const std::int32_t& value) -> QString {
        return QString::number(value);
    };

    const auto setToolTips = [this, integralAction, valueString]() {
        _slider->setToolTip(QString("%1: %2%3").arg(integralAction->text(), valueString(integralAction->getValue()), integralAction->getSuffix()));
    };

    const auto onUpdateValue = [this, integralAction, setToolTips]() {
        const auto value = integralAction->getValue();

        if (value != _slider->value())
            _slider->setValue(value);

        setToolTips();
    };

    const auto onUpdateValueRange = [this, integralAction]() {
        QSignalBlocker sliderBlocker(_slider);

        _slider->setMinimum(integralAction->getMinimum());
        _slider->setMaximum(integralAction->getMaximum());
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

QWidget* IntegralAction::getWidget(QWidget* parent, const Widget::State& state /*= Widget::State::Standard*/)
{
    auto widget = new QWidget(parent);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->addWidget(new SpinBoxWidget(parent, this), 1);
    layout->addWidget(new SliderWidget(parent, this), 2);

    widget->setLayout(layout);

    return widget;
}

hdps::gui::IntegralAction::SpinBoxWidget* IntegralAction::createSpinBoxWidget(QWidget* parent)
{
    return new SpinBoxWidget(parent, this);
}

hdps::gui::IntegralAction::SliderWidget* IntegralAction::createSliderWidget(QWidget* parent)
{
    return new SliderWidget(parent, this);
}

}
}