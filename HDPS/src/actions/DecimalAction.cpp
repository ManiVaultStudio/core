#include "DecimalAction.h"

#include <QHBoxLayout>

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

DecimalAction::SpinBoxWidget::SpinBoxWidget(QWidget* parent, DecimalAction* decimalAction) :
    QDoubleSpinBox(parent)
{
    setAcceptDrops(true);
    setObjectName("DoubleSpinBox");

    connect(this, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, decimalAction](double value) {
        decimalAction->setValue(value);
    });

    const auto valueString = [](const double& value, const std::uint32_t& decimals) -> QString {
        return QString::number(value, 'f', decimals);
    };

    const auto setToolTips = [this, decimalAction, valueString]() {
        setToolTip(QString("%1: %2%3").arg(decimalAction->text(), valueString(decimalAction->getValue(), decimalAction->getDecimals()), decimalAction->getSuffix()));
    };

    const auto onUpdateValue = [this, decimalAction, setToolTips]() {
        const auto actionValue = decimalAction->getValue();

        if (actionValue == value())
            return;
        
        QSignalBlocker doubleSpinBoxBlocker(this);

        setValue(actionValue);

        setToolTips();
    };

    const auto onUpdateValueRange = [this, decimalAction]() {
        QSignalBlocker spinBoxBlocker(this);

        setMinimum(decimalAction->getMinimum());
        setMaximum(decimalAction->getMaximum());
    };

    connect(decimalAction, &DecimalAction::minimumChanged, this, [this, decimalAction, onUpdateValueRange](const double& minimum) {
        onUpdateValueRange();
    });

    connect(decimalAction, &DecimalAction::maximumChanged, this, [this, decimalAction, onUpdateValueRange](const double& maximum) {
        onUpdateValueRange();
    });

    const auto onUpdateSuffix = [this, decimalAction, setToolTips]() {
        QSignalBlocker blocker(this);

        setSuffix(decimalAction->getSuffix());

        setToolTips();
    };

    connect(decimalAction, &DecimalAction::suffixChanged, this, [this, decimalAction, onUpdateSuffix](const QString& suffix) {
        onUpdateSuffix();
    });

    const auto onUpdateDecimals = [this, decimalAction]() {
        QSignalBlocker blocker(this);

        setDecimals(decimalAction->getDecimals());
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
    QSlider(Qt::Horizontal, parent)
{
    setAcceptDrops(true);
    setObjectName("Slider");

    const auto onUpdateAction = [this, decimalAction]() -> void {
        decimalAction->setValue(static_cast<double>(value()) / static_cast<double>(SLIDER_MULTIPLIER));
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

    const auto valueString = [](const double& value, const std::uint32_t& decimals) -> QString {
        return QString::number(value, 'f', decimals);
    };

    const auto setToolTips = [this, decimalAction, valueString]() {
        const auto toolTip = QString("%1: %2%3").arg(decimalAction->text(), valueString(decimalAction->getValue(), decimalAction->getDecimals()), decimalAction->getSuffix());
        
        setToolTip(toolTip);
    };

    const auto onUpdateValue = [this, decimalAction, setToolTips]() {
        const auto actionValue  = decimalAction->getValue();
        const auto sliderValue  = actionValue * static_cast<double>(SLIDER_MULTIPLIER);

        if (sliderValue == value())
            return;

        QSignalBlocker blocker(this);

        setValue(sliderValue);

        setToolTips();
    };

    const auto onUpdateValueRange = [this, decimalAction]() {
        QSignalBlocker blocker(this);

        setMinimum(decimalAction->getMinimum() * SLIDER_MULTIPLIER);
        setMaximum(decimalAction->getMaximum() * SLIDER_MULTIPLIER);
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

hdps::gui::DecimalAction::SpinBoxWidget* DecimalAction::createSpinBoxWidget(QWidget* parent)
{
    return new SpinBoxWidget(parent, this);
}

hdps::gui::DecimalAction::SliderWidget* DecimalAction::createSliderWidget(QWidget* parent)
{
    return new SliderWidget(parent, this);
}

QWidget* DecimalAction::getWidget(QWidget* parent, const WidgetActionWidget::State& state /*= WidgetActionWidget::State::Standard*/)
{
    auto widget = new QWidget(parent);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->addWidget(new SpinBoxWidget(parent, this), 1);
    layout->addWidget(new SliderWidget(parent, this), 2);

    widget->setLayout(layout);

    return widget;
}

}
}
