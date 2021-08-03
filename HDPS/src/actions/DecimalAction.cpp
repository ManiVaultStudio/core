#include "DecimalAction.h"

#include "../Application.h"

#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QSlider>

namespace hdps {

namespace gui {

DecimalAction::DecimalAction(QObject * parent, const QString& title, const double& minimum /*= MIN_VALUE*/, const double& maximum /*= MAX_VALUE*/, const double& value /*= VALUE*/, const double& defaultValue /*= DEFAULT_VALUE*/, const std::int32_t& decimals /*= DECIMALS*/) :
    WidgetAction(parent),
    _value(),
    _defaultValue(),
    _minimum(std::numeric_limits<double>::max()),
    _maximum(std::numeric_limits<double>::min()),
    _suffix(),
    _decimals(),
    _updateDuringDrag(true)
{
    setText(title);
    initialize(minimum, maximum, value, defaultValue, decimals);
}

void DecimalAction::initialize(const double& minimum /*= MIN_VALUE*/, const double& maximum /*= MAX_VALUE*/, const double& value /*= VALUE*/, const double& defaultValue /*= DEFAULT_VALUE*/, const std::int32_t& decimals /*= DECIMALS*/)
{
    setMinimum(minimum);
    setMaximum(maximum);
    setValue(value);
    setDefaultValue(defaultValue);
    setDecimals(decimals);
}

double DecimalAction::getValue() const
{
    return _value;
}

void DecimalAction::setValue(const double& value)
{
    if (value == _value)
        return;

    _value = std::max(_minimum, std::min(value, _maximum));

    emit valueChanged(_value);
}

double DecimalAction::getDefaultValue() const
{
    return _defaultValue;
}

void DecimalAction::setDefaultValue(const double& defaultValue)
{
    if (defaultValue == _defaultValue)
        return;

    _defaultValue = std::max(_minimum, std::min(defaultValue, _maximum));

    emit defaultValueChanged(_defaultValue);
}

bool DecimalAction::canReset() const
{
    return _value != _defaultValue;
}

void DecimalAction::reset()
{
    setValue(_defaultValue);
}

double DecimalAction::getMinimum() const
{
    return _minimum;
}

void DecimalAction::setMinimum(const double& minimum)
{
    if (minimum == _minimum)
        return;

    _minimum = std::min(minimum, _maximum);

    emit minimumChanged(_minimum);
}

double DecimalAction::getMaximum() const
{
    return _maximum;
}

void DecimalAction::setMaximum(const double& maximum)
{
    if (maximum == _maximum)
        return;

    _maximum = std::max(maximum, _minimum);

    emit maximumChanged(_maximum);
}

void DecimalAction::setRange(const double& minimum, const double& maximum)
{
    setMinimum(minimum);
    setMaximum(maximum);
}

QString DecimalAction::getSuffix() const
{
    return _suffix;
}

void DecimalAction::setSuffix(const QString& suffix)
{
    if (suffix == _suffix)
        return;

    _suffix = suffix;

    emit suffixChanged(_suffix);
}

std::uint32_t DecimalAction::getDecimals() const
{
    return _decimals;
}

void DecimalAction::setDecimals(const std::uint32_t& decimals)
{
    if (decimals == _decimals)
        return;

    _decimals = decimals;

    emit decimalsChanged(_decimals);
}

bool DecimalAction::getUpdateDuringDrag() const
{
    return _updateDuringDrag;
}

void DecimalAction::setUpdateDuringDrag(const bool& updateDuringDrag)
{
    if (updateDuringDrag == _updateDuringDrag)
        return;

    _updateDuringDrag = updateDuringDrag;
}

bool DecimalAction::isAtMinimum() const
{
    return _value == _minimum;
}

bool DecimalAction::isAtMaximum() const
{
    return _value == _maximum;
}

hdps::gui::DecimalAction::SpinBoxWidget* DecimalAction::createSpinBoxWidget(QWidget* parent)
{
    return new SpinBoxWidget(parent, this);
}

hdps::gui::DecimalAction::SliderWidget* DecimalAction::createSliderWidget(QWidget* parent)
{
    return new SliderWidget(parent, this);
}

QWidget* DecimalAction::getWidget(QWidget* parent, const Widget::State& state /*= Widget::State::Standard*/)
{
    auto widget = new QWidget(parent);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->addWidget(new SpinBoxWidget(parent, this), 1);
    layout->addWidget(new SliderWidget(parent, this), 2);

    widget->setLayout(layout);

    return widget;
}

DecimalAction::SpinBoxWidget::SpinBoxWidget(QWidget* parent, DecimalAction* decimalAction) :
    WidgetAction::Widget(parent, decimalAction, Widget::State::Standard),
    _doubleSpinBox(new QDoubleSpinBox())
{
    _doubleSpinBox->setObjectName("DoubleSpinBox");

    auto layout = new QHBoxLayout();

    setLayout(layout);

    layout->setMargin(0);
    layout->addWidget(_doubleSpinBox);

    connect(_doubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, decimalAction](double value) {
        decimalAction->setValue(value);
    });

    const auto valueString = [](const double& value, const std::uint32_t& decimals) -> QString {
        return QString::number(value, 'f', decimals);
    };

    const auto setToolTips = [this, decimalAction, valueString]() {
        _doubleSpinBox->setToolTip(QString("%1: %2%3").arg(decimalAction->text(), valueString(decimalAction->getValue(), decimalAction->getDecimals()), decimalAction->getSuffix()));
    };

    const auto onUpdateValue = [this, decimalAction, setToolTips]() {
        const auto value = decimalAction->getValue();

        if (value == _doubleSpinBox->value())
            return;
        
        _doubleSpinBox->setValue(value);
        setToolTips();
    };

    const auto onUpdateValueRange = [this, decimalAction]() {
        QSignalBlocker spinBoxBlocker(_doubleSpinBox);

        _doubleSpinBox->setMinimum(decimalAction->getMinimum());
        _doubleSpinBox->setMaximum(decimalAction->getMaximum());
    };

    connect(decimalAction, &DecimalAction::minimumChanged, this, [this, decimalAction, onUpdateValueRange](const double& minimum) {
        onUpdateValueRange();
    });

    connect(decimalAction, &DecimalAction::maximumChanged, this, [this, decimalAction, onUpdateValueRange](const double& maximum) {
        onUpdateValueRange();
    });

    const auto onUpdateSuffix = [this, decimalAction, setToolTips]() {
        if (!_doubleSpinBox)
            return;

        QSignalBlocker doubleSpinBoxBlocker(_doubleSpinBox);

        _doubleSpinBox->setSuffix(decimalAction->getSuffix());

        setToolTips();
    };

    connect(decimalAction, &DecimalAction::suffixChanged, this, [this, decimalAction, onUpdateSuffix](const QString& suffix) {
        onUpdateSuffix();
    });

    const auto onUpdateDecimals = [this, decimalAction]() {
        if (!_doubleSpinBox)
            return;

        QSignalBlocker doubleSpinBoxBlocker(_doubleSpinBox);

        _doubleSpinBox->setDecimals(decimalAction->getDecimals());
    };

    connect(decimalAction, &DecimalAction::decimalsChanged, this, [this, decimalAction, onUpdateDecimals](const std::int32_t& decimals) {
        onUpdateDecimals();
    });

    connect(decimalAction, &DecimalAction::valueChanged, this, [this, decimalAction, onUpdateValue](const double& value) {
        onUpdateValue();
    });

    onUpdateValueRange();
    onUpdateValue();
    onUpdateSuffix();
    onUpdateDecimals();
    setToolTips();
}

DecimalAction::SliderWidget::SliderWidget(QWidget* parent, DecimalAction* decimalAction) :
    WidgetAction::Widget(parent, decimalAction, Widget::State::Standard),
    _slider(new QSlider(Qt::Horizontal))
{
    _slider->setObjectName("Slider");

    auto layout = new QHBoxLayout();

    setLayout(layout);

    layout->setMargin(0);
    layout->addWidget(_slider);

    const auto onUpdateAction = [this, decimalAction]() -> void {
        decimalAction->setValue(static_cast<double>(_slider->value()) / static_cast<double>(SLIDER_MULTIPLIER));
    };

    connect(_slider, &QSlider::valueChanged, this, [this, decimalAction, onUpdateAction](int value) {
        if (!decimalAction->getUpdateDuringDrag())
            return;

        onUpdateAction();
    });

    connect(_slider, &QSlider::sliderReleased, this, [this, decimalAction, onUpdateAction]() {
        if (decimalAction->getUpdateDuringDrag())
            return;

        onUpdateAction();
    });

    const auto valueString = [](const double& value, const std::uint32_t& decimals) -> QString {
        return QString::number(value, 'f', decimals);
    };

    const auto setToolTips = [this, decimalAction, valueString]() {
        const auto toolTip = QString("%1: %2%3").arg(decimalAction->text(), valueString(decimalAction->getValue(), decimalAction->getDecimals()), decimalAction->getSuffix());

        if (_slider)
            _slider->setToolTip(toolTip);
    };

    const auto onUpdateValue = [this, decimalAction, setToolTips]() {
        const auto value        = decimalAction->getValue();
        const auto sliderValue  = value * static_cast<double>(SLIDER_MULTIPLIER);

        if (sliderValue == _slider->value())
            return;

        _slider->setValue(sliderValue);

        setToolTips();
    };

    const auto onUpdateValueRange = [this, decimalAction]() {
        QSignalBlocker sliderBlocker(_slider);

        _slider->setMinimum(decimalAction->getMinimum() * SLIDER_MULTIPLIER);
        _slider->setMaximum(decimalAction->getMaximum() * SLIDER_MULTIPLIER);
    };

    connect(decimalAction, &DecimalAction::minimumChanged, this, [this, decimalAction, onUpdateValueRange](const double& minimum) {
        onUpdateValueRange();
    });

    connect(decimalAction, &DecimalAction::maximumChanged, this, [this, decimalAction, onUpdateValueRange](const double& maximum) {
        onUpdateValueRange();
    });

    connect(decimalAction, &DecimalAction::valueChanged, this, [this, decimalAction, onUpdateValue](const double& value) {
        onUpdateValue();
    });

    onUpdateValueRange();
    onUpdateValue();
    setToolTips();
}

}
}
