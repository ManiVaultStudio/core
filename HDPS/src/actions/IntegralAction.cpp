#include "IntegralAction.h"

#include <QHBoxLayout>

namespace hdps {

namespace gui {

#if (__cplusplus < 201703L)   // definition needed for pre C++17 gcc and clang
    constexpr std::int32_t IntegralAction::INIT_MIN;
    constexpr std::int32_t IntegralAction::INIT_MAX;
    constexpr std::int32_t IntegralAction::INIT_VALUE;
    constexpr std::int32_t IntegralAction::INIT_DEFAULT_VALUE;
#endif

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

void IntegralAction::initialize(const std::int32_t& minimum, const std::int32_t& maximum, const std::int32_t& value, const std::int32_t& defaultValue)
{
    _minimum        = std::min(minimum, _maximum);
    _maximum        = std::max(maximum, _minimum);
    _value          = std::max(_minimum, std::min(value, _maximum));
    _defaultValue   = std::max(_minimum, std::min(defaultValue, _maximum));

    _minimumChanged();
    _maximumChanged();
    _valueChanged();
    _defaultValueChanged();
}

IntegralAction::SpinBoxWidget::SpinBoxWidget(QWidget* parent, IntegralAction* integralAction) :
    QSpinBox(parent)
{
    setAcceptDrops(true);
    setObjectName("SpinBox");

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

IntegralAction::LineEditWidget::LineEditWidget(QWidget* parent, IntegralAction* integralAction) :
    QLineEdit(parent)
{
    setAcceptDrops(true);
    setObjectName("LineEdit");

    connect(this, &QLineEdit::textChanged, this, [this, integralAction](const QString& value) {
        integralAction->setValue(text().toInt());
    });

    const auto valueString = [](const std::int32_t& value) -> QString {
        return QString::number(value);

    };
    const auto setToolTips = [this, integralAction, valueString]() {
        setToolTip(QString("%1: %2%3").arg(integralAction->text(), valueString(integralAction->getValue()), integralAction->getSuffix()));
    };

    const auto onUpdateValue = [this, integralAction, setToolTips]() {
        const auto actionValue = integralAction->getValue();

        if (actionValue == text().toInt())
            return;

        QSignalBlocker blocker(this);

        setText(QString::number(integralAction->getValue()));

        setToolTips();
    };

    connect(integralAction, &IntegralAction::valueChanged, this, [this, integralAction, onUpdateValue](const std::int32_t& value) {
        onUpdateValue();
    });

    onUpdateValue();
}

QWidget* IntegralAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);

    if (widgetFlags & WidgetFlag::SpinBox)
        layout->addWidget(new SpinBoxWidget(parent, this), 1);

    if (widgetFlags & WidgetFlag::Slider)
        layout->addWidget(new SliderWidget(parent, this), 2);

    if (widgetFlags & WidgetFlag::LineEdit)
        layout->addWidget(new LineEditWidget(parent, this));

    widget->setLayout(layout);

    return widget;
}

}
}
