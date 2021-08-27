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

    /**
     * Spinbox widget class for integral action
     */
    class SpinBoxWidget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param integralAction Pointer to integral action
         */
        SpinBoxWidget(QWidget* parent, IntegralAction* integralAction);
        
    public:

        /** Gets the spinbox widget */
        QSpinBox* getSpinBox() { return _spinBox; }

    protected:
        QSpinBox*   _spinBox;       /** Spinbox widget */

        friend class IntegralAction;
    };

    /**
     * Slider widget class for integral action
     */
    class SliderWidget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param integralAction Pointer to integral action
         */
        SliderWidget(QWidget* parent, IntegralAction* integralAction);

    public:

        /** Gets the slider widget */
        QSlider* getSlider() { return _slider; }

    protected:
        QSlider*   _slider;     /** Slider widget */

        friend class IntegralAction;
    };

protected:

    /**
     * Get widget representation of the integral action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param minimum Minimum value
     * @param maximum Maximum value
     * @param value Value
     * @param defaultValue Default value
     */
    IntegralAction(QObject * parent, const QString& title, const std::int32_t& minimum = MIN_VALUE, const std::int32_t& maximum = MAX_VALUE, const std::int32_t& value = VALUE, const std::int32_t& defaultValue = DEFAULT_VALUE);

    /**
     * Initialize the integral action
     * @param minimum Minimum value
     * @param maximum Maximum value
     * @param value Value
     * @param defaultValue Default value
     */
    void initialize(const std::int32_t& minimum = MIN_VALUE, const std::int32_t& maximum = MAX_VALUE, const std::int32_t& value = VALUE, const std::int32_t& defaultValue = DEFAULT_VALUE);

    /** Gets the current value */
    std::int32_t getValue() const;

    /**
     * Sets the current value
     * @param value Current value
     */
    void setValue(const std::int32_t& value);

    /** Gets the default value */
    std::int32_t getDefaultValue() const;

    /**
     * Sets the default value
     * @param defaultValue Default value
     */
    void setDefaultValue(const std::int32_t& defaultValue);

    /** Determines whether the current value can be reset to its default */
    bool canReset() const override;

    /** Reset the current value to the default value */
    void reset() override;

    /** Gets the minimum value */
    std::int32_t getMinimum() const;

    /**
     * Sets the minimum value
     * @param minimum Minimum value
     */
    void setMinimum(const std::int32_t& minimum);

    /** Gets the maximum value */
    std::int32_t getMaximum() const;

    /**
     * Sets the maximum value
     * @param maximum Maximum value
     */
    void setMaximum(const std::int32_t& maximum);

    /**
     * Sets the value range
     * @param minimum Minimum value
     * @param maximum Maximum value
     */
    void setRange(const std::int32_t& minimum, const std::int32_t& maximum);

    /** Gets the suffix */
    QString getSuffix() const;

    /**
     * Sets the suffix
     * @param suffix Suffix
     */
    void setSuffix(const QString& suffix);

    /** Gets whether the value should update during interaction */
    bool getUpdateDuringDrag() const;

    /**
     * Sets whether the value should update during interaction
     * @param updateDuringDrag Whether the value should update during interaction
     */
    void setUpdateDuringDrag(const bool& updateDuringDrag);

    /** Returns whether the current value is at its minimum */
    bool isAtMinimum() const;

    /** Returns whether the current value is at its maximum */
    bool isAtMaximum() const;

    /*
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
    */

    /**
     * Creates a spin box widget to interact with the integral action
     * @param parent Pointer to parent widget
     * @return Spin box widget
     */
    SpinBoxWidget* createSpinBoxWidget(QWidget* parent);

    /**
     * Creates a slider widget to interact with the integral action
     * @param parent Pointer to parent widget
     * @return Slider widget
     */
    SliderWidget* createSliderWidget(QWidget* parent);

signals:

    /**
     * Signals that the current value changed
     * @param value Current value that changed
     */
    void valueChanged(const std::int32_t& value);

    /**
     * Signals that the default value changed
     * @param defaultValue Default value that changed
     */
    void defaultValueChanged(const std::int32_t& defaultValue);

    /**
     * Signals that the minimum value changed
     * @param minimum New minimum
     */
    void minimumChanged(const std::int32_t& minimum);

    /**
     * Signals that the maximum value changed
     * @param maximum New maximum
     */
    void maximumChanged(const std::int32_t& maximum);

    /**
     * Signals that the suffix changed
     * @param suffix New suffix
     */
    void suffixChanged(const QString& suffix);

protected:
    std::int32_t    _value;                 /** Current value */
    std::int32_t    _defaultValue;          /** Default value */
    std::int32_t    _minimum;               /** Minimum value */
    std::int32_t    _maximum;               /** Maximum value */
    QString         _suffix;                /** Suffix */
    bool            _updateDuringDrag;      /** Whether the value should update during interaction */

    static constexpr std::int32_t MIN_VALUE       = 0;          /** Default initialization minimum value */
    static constexpr std::int32_t MAX_VALUE       = 100;        /** Default initialization maximum value */
    static constexpr std::int32_t VALUE           = 0;          /** Default initialization value */
    static constexpr std::int32_t DEFAULT_VALUE   = 0;          /** Default initialization default value */
};

}
}
