#include "DoubleAction.h"

namespace hdps {

namespace gui {

DoubleAction::DoubleAction(QObject * parent, const QString& title, const double& minimum /*= DEFAULT_MIN_VALUE*/, const double& maximum /*= DEFAULT_MAX_VALUE*/, const double& value /*= DEFAULT_VALUE*/, const std::int32_t& decimals /*= DEFAULT_DECIMALS*/) :
    WidgetAction(parent),
    _value(0.0f),
    _minimum(0.0),
    _maximum(0.0),
    _suffix(),
    _decimals(1),
    _updateDuringDrag(true)
{
    setText(title);
    setMinimum(minimum);
    setMaximum(maximum);
    setValue(value);
    setDecimals(decimals);
}

QWidget* DoubleAction::createWidget(QWidget* parent)
{
    auto widget = new Widget(parent, this);

    //widget->show();

    return widget;
}

double DoubleAction::getValue() const
{
    return _value;
}

void DoubleAction::setValue(const double& value)
{
    if (value == _value)
        return;

    _value = std::max(_minimum, std::min(value, _maximum));

    emit valueChanged(_value);
}

float DoubleAction::getMinimum() const
{
    return _minimum;
}

void DoubleAction::setMinimum(const double& minimum)
{
    if (minimum == _minimum)
        return;

    _minimum = std::min(minimum, _maximum);

    emit minimumChanged(_minimum);
}

float DoubleAction::getMaximum() const
{
    return _maximum;
}

void DoubleAction::setMaximum(const double& maximum)
{
    if (maximum == _maximum)
        return;

    _maximum = std::max(maximum, _minimum);

    emit maximumChanged(_maximum);
}

void DoubleAction::setRange(const double& min, const double& max)
{
    setMinimum(min);
    setMaximum(max);
}

QString DoubleAction::getSuffix() const
{
    return _suffix;
}

void DoubleAction::setSuffix(const QString& suffix)
{
    if (suffix == _suffix)
        return;

    _suffix = suffix;

    emit suffixChanged(_suffix);
}

std::int32_t DoubleAction::getDecimals() const
{
    return _decimals;
}

void DoubleAction::setDecimals(const std::int32_t& decimals)
{
    if (decimals == _decimals)
        return;

    _decimals = decimals;

    emit decimalsChanged(_decimals);
}

bool DoubleAction::getUpdateDuringDrag() const
{
    return _updateDuringDrag;
}

void DoubleAction::setUpdateDuringDrag(const bool& updateDuringDrag)
{
    if (updateDuringDrag == _updateDuringDrag)
        return;

    _updateDuringDrag = updateDuringDrag;
}

DoubleAction::Widget::Widget(QWidget* parent, DoubleAction* doubleAction) :
    WidgetAction::Widget(parent, doubleAction),
    _layout(),
    _spinBox(),
    _slider(Qt::Horizontal)
{
    setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred));

    if (isChildOfMenu()) {
        _spinBox.setFixedWidth(60);
        _slider.setFixedWidth(100);
    }

    if (childOfToolbar()) {
        _spinBox.setFixedWidth(60);
        _slider.setFixedWidth(60);
    }

    _layout.setMargin(0);
    _layout.addWidget(&_spinBox);
    _layout.addWidget(&_slider);

    setLayout(&_layout);

    //_label.setVisible(childOfMenu());

    const auto setToolTips = [this, doubleAction]() {
        const auto toolTip = QString("%1: %2%3").arg(doubleAction->text(), QString::number(doubleAction->getValue(), 'f', doubleAction->getDecimals()), doubleAction->getSuffix());

        _spinBox.setToolTip(toolTip);
        _slider.setToolTip(toolTip);
    };

    connect(&_spinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, doubleAction](double value) {
        doubleAction->setValue(value);
    });

    const auto updateAction = [this, doubleAction]() -> void {
        doubleAction->setValue(static_cast<double>(_slider.value()) / static_cast<double>(SLIDER_MULTIPLIER));
    };

    connect(&_slider, &QSlider::valueChanged, this, [this, doubleAction, updateAction](int value) {
        if (!doubleAction->getUpdateDuringDrag())
            return;

        updateAction();
    });

    connect(&_slider, &QSlider::sliderReleased, this, [this, doubleAction, updateAction]() {
        if (doubleAction->getUpdateDuringDrag())
            return;

        updateAction();
    });

    const auto updateValue = [this, doubleAction, setToolTips]() {
        const auto value = doubleAction->getValue();

        if (value != _spinBox.value())
            _spinBox.setValue(value);

        if (value * SLIDER_MULTIPLIER != _slider.value())
            _slider.setValue(value * static_cast<double>(SLIDER_MULTIPLIER));

        setToolTips();
    };

    connect(doubleAction, &DoubleAction::valueChanged, this, [this, doubleAction, updateValue](const double& value) {
        updateValue();
    });

    const auto updateValueRange = [this, doubleAction]() {
        QSignalBlocker spinBoxBlocker(&_spinBox), sliderBlocker(&_slider);

        const auto minimum = doubleAction->getMinimum();

        _spinBox.setMinimum(minimum);
        _slider.setMinimum(minimum * SLIDER_MULTIPLIER);

        const auto maximum = doubleAction->getMaximum();

        _spinBox.setMaximum(maximum);
        _slider.setMaximum(maximum * SLIDER_MULTIPLIER);
    };

    connect(doubleAction, &DoubleAction::minimumChanged, this, [this, doubleAction, updateValueRange](const double& minimum) {
        updateValueRange();
    });

    connect(doubleAction, &DoubleAction::maximumChanged, this, [this, doubleAction, updateValueRange](const double& maximum) {
        updateValueRange();
    });

    const auto updateSuffix = [this, doubleAction, setToolTips]() {
        QSignalBlocker spinBoxBlocker(&_spinBox);

        _spinBox.setSuffix(doubleAction->getSuffix());

        setToolTips();
    };

    connect(doubleAction, &DoubleAction::suffixChanged, this, [this, doubleAction, updateSuffix](const QString& suffix) {
        updateSuffix();
    });

    const auto updateDecimals = [this, doubleAction]() {
        QSignalBlocker spinBoxBlocker(&_spinBox);

        _spinBox.setDecimals(doubleAction->getDecimals());
    };

    connect(doubleAction, &DoubleAction::decimalsChanged, this, [this, doubleAction, updateDecimals](const std::int32_t& decimals) {
        updateDecimals();
    });

    updateValueRange();
    updateValue();
    updateSuffix();
    updateDecimals();
    setToolTips();
}

}
}