#include "DecimalAction.h"

#include <QHBoxLayout>

namespace hdps {

namespace gui {

#if (__cplusplus < 201703L)   // definition needed for pre C++17 gcc and clang
    constexpr float DecimalAction::INIT_MIN;
    constexpr float DecimalAction::INIT_MAX;
    constexpr float DecimalAction::INIT_VALUE;
    constexpr float DecimalAction::INIT_DEFAULT_VALUE;
    constexpr int   DecimalAction::INIT_DECIMALS;
#endif

DecimalAction::DecimalAction(QObject * parent, const QString& title, const float& minimum /*= INIT_MIN*/, const float& maximum /*= INIT_MAX*/, const float& value /*= INIT_VALUE*/, const float& defaultValue /*= INIT_DEFAULT_VALUE*/, const std::uint32_t& numberOfDecimals /*= INIT_NUMBER_OF_DECIMALS*/) :
    NumericalAction<float>(parent, title, minimum, maximum, value, defaultValue, numberOfDecimals),
    _singleStep(0.1f)
{
    _valueChanged               = [this]() -> void { emit valueChanged(_value); };
    _defaultValueChanged        = [this]() -> void { emit defaultValueChanged(_defaultValue); };
    _minimumChanged             = [this]() -> void { emit minimumChanged(_minimum); };
    _maximumChanged             = [this]() -> void { emit maximumChanged(_maximum); };
    _prefixChanged              = [this]() -> void { emit prefixChanged(_prefix); };
    _suffixChanged              = [this]() -> void { emit suffixChanged(_suffix); };
    _numberOfDecimalsChanged    = [this]() -> void { emit numberOfDecimalsChanged(_numberOfDecimals); };

    initialize(minimum, maximum, value, defaultValue, numberOfDecimals);
}

QString DecimalAction::getTypeString() const
{
    return "Decimal";
}

void DecimalAction::initialize(const float& minimum, const float& maximum, const float& value, const float& defaultValue, const std::uint32_t& numberOfDecimals /*= INIT_NUMBER_OF_DECIMALS*/)
{
    _minimum            = std::min(minimum, _maximum);
    _maximum            = std::max(maximum, _minimum);
    _value              = std::max(_minimum, std::min(value, _maximum));
    _defaultValue       = std::max(_minimum, std::min(defaultValue, _maximum));
    _numberOfDecimals   = numberOfDecimals;

    _minimumChanged();
    _maximumChanged();
    _valueChanged();
    _defaultValueChanged();

    if (_numberOfDecimalsChanged)
        _numberOfDecimalsChanged();
}

float DecimalAction::getSingleStep() const
{
    return _singleStep;
}

void DecimalAction::setSingleStep(const float& singleStep)
{
    if (singleStep == _singleStep)
        return;

    _singleStep = std::max(0.0f, singleStep);

    emit singleStepChanged(_singleStep);
}

bool DecimalAction::mayPublish() const
{
    return true;
}

void DecimalAction::connectToPublicAction(WidgetAction* publicAction)
{
    auto publicDecimalAction = dynamic_cast<DecimalAction*>(publicAction);

    Q_ASSERT(publicDecimalAction != nullptr);

    connect(this, &DecimalAction::valueChanged, publicDecimalAction, &DecimalAction::setValue);
    connect(publicDecimalAction, &DecimalAction::valueChanged, this, &DecimalAction::setValue);

    WidgetAction::connectToPublicAction(publicAction);
}

void DecimalAction::disconnectFromPublicAction()
{
    auto publicDecimalAction = dynamic_cast<DecimalAction*>(_publicAction);

    Q_ASSERT(publicDecimalAction != nullptr);

    disconnect(this, &DecimalAction::valueChanged, publicDecimalAction, &DecimalAction::setValue);
    disconnect(publicDecimalAction, &DecimalAction::valueChanged, this, &DecimalAction::setValue);

    WidgetAction::disconnectFromPublicAction();
}

WidgetAction* DecimalAction::getPublicCopy() const
{
    return new DecimalAction(parent(), text(), getMinimum(), getMaximum(), getValue(), getDefaultValue(), getNumberOfDecimals());
}

void DecimalAction::fromVariantMap(const QVariantMap& variantMap)
{
    if (!variantMap.contains("Value"))
        return;

    setValue(variantMap["Value"].toDouble());
}

QVariantMap DecimalAction::toVariantMap() const
{
    return { { "Value", static_cast<double>(_value) } };
}

DecimalAction::SpinBoxWidget::SpinBoxWidget(QWidget* parent, DecimalAction* decimalAction) :
    QDoubleSpinBox(parent)
{
    setAcceptDrops(true);
    setObjectName("SpinBox");

    connect(this, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, decimalAction](double value) {
        decimalAction->setValue(value);
    });

    const auto valueString = [](const double& value, const std::uint32_t& decimals) -> QString {
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
    connect(decimalAction, &DecimalAction::suffixChanged, this, onUpdateSuffix);
    connect(decimalAction, &DecimalAction::numberOfDecimalsChanged, this, onUpdateDecimals);
    connect(decimalAction, &DecimalAction::singleStepChanged, this, onUpdateSingleStep);
    connect(decimalAction, &DecimalAction::valueChanged, this, onUpdateValue);

    onUpdateValueRange();
    onUpdateValue();
    onUpdateSuffix();
    onUpdateDecimals();
    onUpdateSingleStep();
    setToolTips();
}

DecimalAction::SliderWidget::SliderWidget(QWidget* parent, DecimalAction* decimalAction) :
    QSlider(Qt::Horizontal, parent)
{
    setAcceptDrops(true);
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

    const auto valueString = [](const double& value, const std::uint32_t& decimals) -> QString {
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

    connect(decimalAction, &DecimalAction::valueChanged, this, [this, decimalAction, onUpdateValue](const double& value) {
        onUpdateValue();
    });

    onUpdateValue();
    setToolTips();
}

QWidget* DecimalAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);

    if (widgetFlags & WidgetFlag::SpinBox)
        layout->addWidget(new SpinBoxWidget(parent, this), 1);

    if (widgetFlags & WidgetFlag::Slider)
        layout->addWidget(new SliderWidget(parent, this), 2);

    widget->setLayout(layout);

    return widget;
}

}
}
