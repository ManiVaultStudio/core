#include "DecimalAction.h"

#include "../Application.h"

#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QPushButton>
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
    setMinimum(minimum);
    setMaximum(maximum);
    setValue(value);
    setDefaultValue(defaultValue);
    setDecimals(decimals);
}

QWidget* DecimalAction::createWidget(QWidget* parent)
{
    return new Widget(parent, this);
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

float DecimalAction::getMinimum() const
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

float DecimalAction::getMaximum() const
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

void DecimalAction::setRange(const double& min, const double& max)
{
    setMinimum(min);
    setMaximum(max);
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

DecimalAction::Widget::Widget(QWidget* parent, DecimalAction* decimalAction, const Configuration& configuration /*= Configuration::All*/) :
    WidgetAction::Widget(parent, decimalAction),
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

        connect(_valueDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, decimalAction](double value) {
            decimalAction->setValue(value);
        });
    }

    if (configuration & Configuration::Slider) {
        _valueSlider = new QSlider(Qt::Horizontal);
        
        layout->addWidget(_valueSlider);

        const auto onUpdateAction = [this, decimalAction]() -> void {
            decimalAction->setValue(static_cast<double>(_valueSlider->value()) / static_cast<double>(SLIDER_MULTIPLIER));
        };

        connect(_valueSlider, &QSlider::valueChanged, this, [this, decimalAction, onUpdateAction](int value) {
            if (!decimalAction->getUpdateDuringDrag())
                return;

            onUpdateAction();
        });

        connect(_valueSlider, &QSlider::sliderReleased, this, [this, decimalAction, onUpdateAction]() {
            if (decimalAction->getUpdateDuringDrag())
                return;

            onUpdateAction();
        });
    }

    if (configuration & Configuration::Reset) {
        _resetPushButton = new QPushButton();

        layout->addWidget(_resetPushButton);

        _resetPushButton->setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("undo"));

        connect(_resetPushButton, &QPushButton::clicked, this, [this, decimalAction]() {
            decimalAction->reset();
        });
    }

    const auto valueString = [](const double& value, const std::uint32_t& decimals) -> QString {
        return QString::number(value, 'f', decimals);
    };

    const auto setToolTips = [this, decimalAction, valueString]() {
        const auto toolTip = QString("%1: %2%3").arg(decimalAction->text(), valueString(decimalAction->getValue(), decimalAction->getDecimals()), decimalAction->getSuffix());

        if (_valueDoubleSpinBox)
            _valueDoubleSpinBox->setToolTip(toolTip);

        if (_valueSlider)
            _valueSlider->setToolTip(toolTip);
    };

    const auto onUpdateValue = [this, decimalAction, setToolTips]() {
        const auto value = decimalAction->getValue();

        if (_valueDoubleSpinBox && value != _valueDoubleSpinBox->value())
            _valueDoubleSpinBox->setValue(value);

        if (_valueSlider && value * SLIDER_MULTIPLIER != _valueSlider->value())
            _valueSlider->setValue(value * static_cast<double>(SLIDER_MULTIPLIER));

        if (_resetPushButton)
            _resetPushButton->setEnabled(decimalAction->canReset());

        setToolTips();
    };

    const auto onUpdateValueRange = [this, decimalAction]() {
        QSignalBlocker spinBoxBlocker(_valueDoubleSpinBox), sliderBlocker(_valueSlider);

        const auto minimum = decimalAction->getMinimum();
        const auto maximum = decimalAction->getMaximum();

        if (_valueDoubleSpinBox) {
            _valueDoubleSpinBox->setMinimum(minimum);
            _valueDoubleSpinBox->setMaximum(maximum);
        }
        
        if (_valueSlider) {
            _valueSlider->setMinimum(minimum * SLIDER_MULTIPLIER);
            _valueSlider->setMaximum(maximum * SLIDER_MULTIPLIER);
        }
    };

    connect(decimalAction, &DecimalAction::minimumChanged, this, [this, decimalAction, onUpdateValueRange](const double& minimum) {
        onUpdateValueRange();
    });

    connect(decimalAction, &DecimalAction::maximumChanged, this, [this, decimalAction, onUpdateValueRange](const double& maximum) {
        onUpdateValueRange();
    });

    const auto onUpdateSuffix = [this, decimalAction, setToolTips]() {
        if (!_valueDoubleSpinBox)
            return;

        QSignalBlocker doubleSpinBoxBlocker(_valueDoubleSpinBox);

        _valueDoubleSpinBox->setSuffix(decimalAction->getSuffix());

        setToolTips();
    };

    connect(decimalAction, &DecimalAction::suffixChanged, this, [this, decimalAction, onUpdateSuffix](const QString& suffix) {
        onUpdateSuffix();
    });

    const auto onUpdateDecimals = [this, decimalAction]() {
        if (!_valueDoubleSpinBox)
            return;

        QSignalBlocker doubleSpinBoxBlocker(_valueDoubleSpinBox);

        _valueDoubleSpinBox->setDecimals(decimalAction->getDecimals());
    };

    connect(decimalAction, &DecimalAction::decimalsChanged, this, [this, decimalAction, onUpdateDecimals](const std::int32_t& decimals) {
        onUpdateDecimals();
    });
    
    const auto onUpdateDefaultValue = [this, valueString, decimalAction]() -> void {
        if (!_resetPushButton)
            return;

        _resetPushButton->setToolTip(QString("Reset %1 to: %2").arg(decimalAction->text(), valueString(decimalAction->getDefaultValue(), decimalAction->getDecimals())));
    };

    connect(decimalAction, &DecimalAction::valueChanged, this, [this, decimalAction, onUpdateValue](const double& value) {
        onUpdateValue();
    });

    connect(decimalAction, &DecimalAction::defaultValueChanged, this, [this, onUpdateDefaultValue](const double& value) {
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