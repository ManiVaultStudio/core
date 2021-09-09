#include "IntegralAction.h"

#include <QHBoxLayout>

namespace hdps {

namespace gui {

IntegralAction::IntegralAction(QObject * parent, const QString& title, const std::int32_t& minimum /*= INIT_MIN*/, const std::int32_t& maximum /*= INIT_MAX*/, const std::int32_t& value /*= INIT_VALUE*/, const std::int32_t& defaultValue /*= INIT_DEFAULT_VALUE*/) :
    WidgetAction(parent),
    _value(),
    _defaultValue(),
    _minimum(std::numeric_limits<std::int32_t>::lowest()),
    _maximum(std::numeric_limits<std::int32_t>::max()),
    _suffix(),
    _updateDuringDrag(true)
{
    setText(title);
    initialize(minimum, maximum, value, defaultValue);
}

void IntegralAction::initialize(const std::int32_t& minimum /*= INIT_MIN*/, const std::int32_t& maximum /*= INIT_MAX*/, const std::int32_t& value /*= INIT_VALUE*/, const std::int32_t& defaultValue /*= INIT_DEFAULT_VALUE*/)
{
    _minimum        = std::min(minimum, _maximum);
    _maximum        = std::max(maximum, _minimum);
    _value          = std::max(_minimum, std::min(value, _maximum));
    _defaultValue   = std::max(_minimum, std::min(defaultValue, _maximum));

    emit minimumChanged(_minimum);
    emit maximumChanged(_maximum);
    emit valueChanged(_value);
    emit defaultValueChanged(_defaultValue);
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

void IntegralAction::setRange(const std::int32_t& minimum, const std::int32_t& maximum)
{
    setMinimum(minimum);
    setMaximum(maximum);
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
    QSpinBox(parent)
{
    setAcceptDrops(true);
    setObjectName("SpinBox");

    const auto update = [this, integralAction]() -> void {
        setEnabled(integralAction->isEnabled());
        setToolTip(integralAction->text());
    };

    connect(integralAction, &IntegralAction::changed, this, [update]() {
        update();
    });

    update();

    connect(this, qOverload<int>(&QSpinBox::valueChanged), this, [this, integralAction](int value) {
        integralAction->setValue(value);
    });

    const auto valueString = [](const std::int32_t& value) -> QString {
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

    const auto onUpdateSuffix = [this, integralAction, setToolTips]() {
        QSignalBlocker blocker(this);

        setSuffix(integralAction->getSuffix());

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
    QSlider(Qt::Horizontal, parent)
{
    setAcceptDrops(true);
    setObjectName("Slider");

    const auto update = [this, integralAction]() -> void {
        setEnabled(integralAction->isEnabled());
        setToolTip(integralAction->text());
    };

    connect(integralAction, &IntegralAction::changed, this, [update]() {
        update();
    });

    update();

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

    const auto valueString = [](const std::int32_t& value) -> QString {
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

QWidget* IntegralAction::getWidget(QWidget* parent, const WidgetActionWidget::State& state /*= WidgetActionWidget::State::Standard*/)
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
