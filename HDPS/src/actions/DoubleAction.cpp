#include "DoubleAction.h"

#include "../Application.h"

#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QSlider>

namespace hdps {

namespace gui {

DoubleAction::DoubleAction(QObject * parent, const QString& title, const double& minimum /*= MIN_VALUE*/, const double& maximum /*= MAX_VALUE*/, const double& value /*= VALUE*/, const double& defaultValue /*= DEFAULT_VALUE*/, const std::int32_t& decimals /*= DECIMALS*/) :
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
    setMinimum(minimum);
    setMaximum(maximum);
    setValue(value);
    setDefaultValue(defaultValue);
    setDecimals(decimals);
}

QWidget* DoubleAction::createWidget(QWidget* parent)
{
    return new Widget(parent, this);
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

double DoubleAction::getDefaultValue() const
{
    return _defaultValue;
}

void DoubleAction::setDefaultValue(const double& defaultValue)
{
    if (defaultValue == _defaultValue)
        return;

    _defaultValue = std::max(_minimum, std::min(defaultValue, _maximum));

    emit defaultValueChanged(_defaultValue);
}

bool DoubleAction::canReset() const
{
    return _value != _defaultValue;
}

void DoubleAction::reset()
{
    setValue(_defaultValue);
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

std::uint32_t DoubleAction::getDecimals() const
{
    return _decimals;
}

void DoubleAction::setDecimals(const std::uint32_t& decimals)
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

DoubleAction::Widget::Widget(QWidget* parent, DoubleAction* doubleAction, const Configuration& configuration /*= Configuration::All*/) :
    WidgetAction::Widget(parent, doubleAction),
    _valueDoubleSpinBox(nullptr),
    _valueSlider(nullptr),
    _resetPushButton(nullptr)
{
    setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred));

    auto layout = new QHBoxLayout();

    setLayout(layout);

    layout->setMargin(0);

    if (configuration & Configuration::SpinBox) {
        _valueDoubleSpinBox = new QDoubleSpinBox();
        
        _valueDoubleSpinBox->setFixedWidth(60);

        layout->addWidget(_valueDoubleSpinBox);

        connect(_valueDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, doubleAction](double value) {
            doubleAction->setValue(value);
        });
    }

    if (configuration & Configuration::Slider) {
        _valueSlider = new QSlider(Qt::Horizontal);
        
        layout->addWidget(_valueSlider);

        const auto onUpdateAction = [this, doubleAction]() -> void {
            doubleAction->setValue(static_cast<double>(_valueSlider->value()) / static_cast<double>(SLIDER_MULTIPLIER));
        };

        connect(_valueSlider, &QSlider::valueChanged, this, [this, doubleAction, onUpdateAction](int value) {
            if (!doubleAction->getUpdateDuringDrag())
                return;

            onUpdateAction();
        });

        connect(_valueSlider, &QSlider::sliderReleased, this, [this, doubleAction, onUpdateAction]() {
            if (doubleAction->getUpdateDuringDrag())
                return;

            onUpdateAction();
        });
    }

    if (configuration & Configuration::Reset) {
        _resetPushButton = new QPushButton();

        layout->addWidget(_resetPushButton);

        _resetPushButton->setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("undo"));

        connect(_resetPushButton, &QPushButton::clicked, this, [this, doubleAction]() {
            doubleAction->reset();
        });
    }

    const auto valueString = [](const double& value, const std::uint32_t& decimals) -> QString {
        return QString::number(value, 'f', decimals);
    };

    const auto setToolTips = [this, doubleAction, valueString]() {
        const auto toolTip = QString("%1: %2%3").arg(doubleAction->text(), valueString(doubleAction->getValue(), doubleAction->getDecimals()), doubleAction->getSuffix());

        if (_valueDoubleSpinBox)
            _valueDoubleSpinBox->setToolTip(toolTip);

        if (_valueSlider)
            _valueSlider->setToolTip(toolTip);
    };

    const auto onUpdateValue = [this, doubleAction, setToolTips]() {
        const auto value = doubleAction->getValue();

        if (_valueDoubleSpinBox && value != _valueDoubleSpinBox->value())
            _valueDoubleSpinBox->setValue(value);

        if (_valueSlider && value * SLIDER_MULTIPLIER != _valueSlider->value())
            _valueSlider->setValue(value * static_cast<double>(SLIDER_MULTIPLIER));

        if (_resetPushButton)
            _resetPushButton->setEnabled(doubleAction->canReset());

        setToolTips();
    };

    const auto onUpdateValueRange = [this, doubleAction]() {
        QSignalBlocker spinBoxBlocker(_valueDoubleSpinBox), sliderBlocker(_valueSlider);

        const auto minimum = doubleAction->getMinimum();
        const auto maximum = doubleAction->getMaximum();

        if (_valueDoubleSpinBox) {
            _valueDoubleSpinBox->setMinimum(minimum);
            _valueDoubleSpinBox->setMaximum(maximum);
        }
        
        if (_valueSlider) {
            _valueSlider->setMinimum(minimum * SLIDER_MULTIPLIER);
            _valueSlider->setMaximum(maximum * SLIDER_MULTIPLIER);
        }
    };

    connect(doubleAction, &DoubleAction::minimumChanged, this, [this, doubleAction, onUpdateValueRange](const double& minimum) {
        onUpdateValueRange();
    });

    connect(doubleAction, &DoubleAction::maximumChanged, this, [this, doubleAction, onUpdateValueRange](const double& maximum) {
        onUpdateValueRange();
    });

    const auto onUpdateSuffix = [this, doubleAction, setToolTips]() {
        if (!_valueDoubleSpinBox)
            return;

        QSignalBlocker doubleSpinBoxBlocker(_valueDoubleSpinBox);

        _valueDoubleSpinBox->setSuffix(doubleAction->getSuffix());

        setToolTips();
    };

    connect(doubleAction, &DoubleAction::suffixChanged, this, [this, doubleAction, onUpdateSuffix](const QString& suffix) {
        onUpdateSuffix();
    });

    const auto onUpdateDecimals = [this, doubleAction]() {
        if (!_valueDoubleSpinBox)
            return;

        QSignalBlocker doubleSpinBoxBlocker(_valueDoubleSpinBox);

        _valueDoubleSpinBox->setDecimals(doubleAction->getDecimals());
    };

    connect(doubleAction, &DoubleAction::decimalsChanged, this, [this, doubleAction, onUpdateDecimals](const std::int32_t& decimals) {
        onUpdateDecimals();
    });
    
    const auto onUpdateDefaultValue = [this, valueString, doubleAction]() -> void {
        if (!_resetPushButton)
            return;

        _resetPushButton->setToolTip(QString("Reset %1 to: %2").arg(doubleAction->text(), valueString(doubleAction->getDefaultValue(), doubleAction->getDecimals())));
    };

    connect(doubleAction, &DoubleAction::valueChanged, this, [this, doubleAction, onUpdateValue](const double& value) {
        onUpdateValue();
    });

    connect(doubleAction, &DoubleAction::defaultValueChanged, this, [this, onUpdateDefaultValue](const double& value) {
        onUpdateDefaultValue();
    });

    onUpdateValueRange();
    onUpdateValue();
    onUpdateDefaultValue();
    onUpdateSuffix();
    onUpdateDecimals();
    setToolTips();
}

}
}