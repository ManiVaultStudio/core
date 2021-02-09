#include "DoubleAction.h"

namespace hdps {

namespace gui {

DoubleAction::Widget::Widget(QWidget* parent, DoubleAction* doubleAction) :
    WidgetAction::Widget(parent, doubleAction),
    _layout(),
    _label(QString("%1:").arg(doubleAction->text())),
    _spinBox(),
    _slider(Qt::Horizontal)
{
    setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred));

    if (childOfWidget()) {
        _label.setFixedWidth(80);
    }

    if (childOfMenu()) {
        _label.setFixedWidth(80);
        _spinBox.setFixedWidth(60);
        _slider.setFixedWidth(100);
    }
    
    if (childOfToolbar()) {
        _spinBox.setFixedWidth(60);
        _slider.setFixedWidth(60);
    }

    _layout.setMargin(0);
    _layout.addWidget(&_label);
    _layout.addWidget(&_spinBox);
    _layout.addWidget(&_slider);

    setLayout(&_layout);

    //_label.setVisible(childOfMenu());

    const auto setToolTips = [this, doubleAction]() {
        _label.setToolTip(doubleAction->text());

        const auto toolTip = QString("%1: %2%3").arg(doubleAction->text(), QString::number(doubleAction->getValue(), 'f', doubleAction->getDecimals()), doubleAction->getSuffix());

        _spinBox.setToolTip(toolTip);
        _slider.setToolTip(toolTip);
    };

    connect(&_spinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, doubleAction](double value) {
        doubleAction->setValue(value);
    });

    connect(&_slider, &QSlider::valueChanged, this, [this, doubleAction](int value) {
        doubleAction->setValue(static_cast<double>(value) / static_cast<double>(SLIDER_MULTIPLIER));
    });

    const auto updateValue = [this, doubleAction, setToolTips]() {
        QSignalBlocker spinBoxBlocker(&_spinBox), sliderBlocker(&_slider);

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

        const auto minValue = doubleAction->getMinValue();

        _spinBox.setMinimum(minValue);
        _slider.setMinimum(minValue * SLIDER_MULTIPLIER);

        const auto maxValue = doubleAction->getMaxValue();

        _spinBox.setMaximum(maxValue);
        _slider.setMaximum(maxValue * SLIDER_MULTIPLIER);
    };

    connect(doubleAction, &DoubleAction::minValueChanged, this, [this, doubleAction, updateValueRange](const double& minValue) {
        updateValueRange();
    });

    connect(doubleAction, &DoubleAction::maxValueChanged, this, [this, doubleAction, updateValueRange](const double& maxValue) {
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

DoubleAction::DoubleAction(QObject * parent, const QString& title /*= ""*/) :
    WidgetAction(parent),
    _value(0.0f)
{
    setText(title);
    setMinValue(DEFAULT_MIN_VALUE);
    setMaxValue(DEFAULT_MAX_VALUE);
    setDecimals(DEFAULT_DECIMALS);
}

QWidget* DoubleAction::createWidget(QWidget* parent)
{
    auto widget = new Widget(parent, this);

    widget->show();

    return widget;
}

}
}