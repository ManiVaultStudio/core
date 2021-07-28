#pragma once

#include "WidgetAction.h"

class QWidget;
class QSpinBox;
class QSlider;
class QPushButton;

namespace hdps {

namespace gui {

/**
 * Integral widget action class
 *
 * Stores an integral value and creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
class IntegralAction : public WidgetAction
{
    Q_OBJECT

public:

    class SpinBoxWidget : public WidgetAction::Widget
    {
    protected:
        SpinBoxWidget(QWidget* parent, IntegralAction* integralAction);
        
    public:
        QSpinBox* getSpinBox() { return _spinBox; }

    protected:
        QSpinBox*   _spinBox;

        friend class IntegralAction;
    };

    class SliderWidget : public WidgetAction::Widget
    {
    protected:
        SliderWidget(QWidget* parent, IntegralAction* integralAction);

    public:
        QSlider* getSlider() { return _slider; }

    protected:
        QSlider*   _slider;

        friend class IntegralAction;
    };

protected:
    QWidget* getWidget(QWidget* parent, const Widget::State& state = Widget::State::Standard) override;

public:
    IntegralAction(QObject * parent, const QString& title, const std::int32_t& minimum = MIN_VALUE, const std::int32_t& maximum = MAX_VALUE, const std::int32_t& value = VALUE, const std::int32_t& defaultValue = DEFAULT_VALUE);

    std::int32_t getValue() const;
    void setValue(const std::int32_t& value);

    std::int32_t getDefaultValue() const;
    void setDefaultValue(const std::int32_t& defaultValue);

    bool canReset() const;
    void reset();

    std::int32_t getMinimum() const;
    void setMinimum(const std::int32_t& minimum);

    std::int32_t getMaximum() const;
    void setMaximum(const std::int32_t& maximum);

    void setRange(const std::int32_t& min, const std::int32_t& max);

    QString getSuffix() const;
    void setSuffix(const QString& suffix);

    bool getUpdateDuringDrag() const;
    void setUpdateDuringDrag(const bool& updateDuringDrag);

    bool isAtMinimum() const;
    bool isAtMaximum() const;

    IntegralAction& operator= (const IntegralAction& other)
    {
        WidgetAction::operator=(other);

        _value              = other._value;
        _defaultValue       = other._defaultValue;
        _minimum            = other._minimum;
        _maximum            = other._maximum;
        _suffix             = other._suffix;
        _updateDuringDrag   = other._updateDuringDrag;

        return *this;
    }

    SpinBoxWidget* createSpinBoxWidget(QWidget* parent);
    SliderWidget* createSliderWidget(QWidget* parent);

signals:
    void valueChanged(const std::int32_t& value);
    void defaultValueChanged(const std::int32_t& defaultValue);
    void minimumChanged(const std::int32_t& minimum);
    void maximumChanged(const std::int32_t& maximum);
    void suffixChanged(const QString& suffix);

protected:
    std::int32_t    _value;
    std::int32_t    _defaultValue;
    std::int32_t    _minimum;
    std::int32_t    _maximum;
    QString         _suffix;
    bool            _updateDuringDrag;

    static constexpr std::int32_t MIN_VALUE       = 0;
    static constexpr std::int32_t MAX_VALUE       = 100;
    static constexpr std::int32_t VALUE           = 0;
    static constexpr std::int32_t DEFAULT_VALUE   = 0;
};

}
}