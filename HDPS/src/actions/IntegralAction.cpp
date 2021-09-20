#include "IntegralAction.h"

#include <QHBoxLayout>

namespace hdps {

namespace gui {

IntegralAction::IntegralAction(QObject * parent, const QString& title, const std::int32_t& minimum /*= INIT_MIN*/, const std::int32_t& maximum /*= INIT_MAX*/, const std::int32_t& value /*= INIT_VALUE*/, const std::int32_t& defaultValue /*= INIT_DEFAULT_VALUE*/) :
    NumericalAction<std::int32_t>(parent, title, minimum, maximum, value, defaultValue)
{
    _valueChanged           = [this]() -> void { emit valueChanged(_value); };
    _defaultValueChanged    = [this]() -> void { emit defaultValueChanged(_defaultValue); };
    _minimumChanged         = [this]() -> void { emit minimumChanged(_minimum); };
    _maximumChanged         = [this]() -> void { emit maximumChanged(_maximum); };
    _prefixChanged          = [this]() -> void { emit prefixChanged(_prefix); };
    _suffixChanged          = [this]() -> void { emit suffixChanged(_suffix); };
    _resettableChanged      = [this]() -> void { emit resettableChanged(isResettable()); };

    initialize(minimum, maximum, value, defaultValue);
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

    if (hasWidgetFlag(WidgetFlag::SpinBox))
        layout->addWidget(new SpinBoxWidget(parent, this), 1);

    if (hasWidgetFlag(WidgetFlag::Slider))
        layout->addWidget(new SliderWidget(parent, this), 2);

    if (hasWidgetFlag(WidgetFlag::Reset))
        layout->addWidget(createResetButton(parent));

    widget->setLayout(layout);

    const auto update = [this, widget]() -> void {
        widget->setEnabled(isEnabled());
        widget->setToolTip(text());
    };

    connect(this, &IntegralAction::changed, this, [update]() {
        update();
    });

    update();

    return widget;
}

IntegralAction::SpinBoxWidget* IntegralAction::createSpinBoxWidget(QWidget* parent)
{
    return new SpinBoxWidget(parent, this);
}

IntegralAction::SliderWidget* IntegralAction::createSliderWidget(QWidget* parent)
{
    return new SliderWidget(parent, this);
}

}
}
