#pragma once

#include "WidgetAction.h"

#include <QDoubleSpinBox>
#include <QSlider>

class QWidget;
class QPushButton;

namespace hdps {

namespace gui {

/**
 * Decimal widget action class
 *
 * Stores a float value and creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
class DecimalAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Spinbox widget class for decimal action */
    class SpinBoxWidget : public QDoubleSpinBox
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param decimalAction Pointer to decimal action
         */
        SpinBoxWidget(QWidget* parent, DecimalAction* decimalAction);

        friend class DecimalAction;
    };

    /** Slider widget class for decimal action */
    class SliderWidget : public QSlider
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param decimalAction Pointer to decimal action
         */
        SliderWidget(QWidget* parent, DecimalAction* decimalAction);

        friend class DecimalAction;
    };

protected:

    /**
     * Get widget representation of the decimal action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override;;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param minimum Minimum value
     * @param maximum Maximum value
     * @param value Value
     * @param defaultValue Default value
     * @param decimals Number of decimals
     */
    DecimalAction(QObject * parent, const QString& title, const float& minimum = INIT_MIN, const float& maximum = INIT_MAX, const float& value = INIT_VALUE, const float& defaultValue = INIT_DEFAULT_VALUE, const std::int32_t& decimals = INIT_DECIMALS);

    /**
     * Initialize the decimal action
     * @param minimum Minimum value
     * @param maximum Maximum value
     * @param value Value
     * @param defaultValue Default value
     * @param decimals Number of decimals
     */
    void initialize(const float& minimum = INIT_MIN, const float& maximum = INIT_MAX, const float& value = INIT_VALUE, const float& defaultValue = INIT_DEFAULT_VALUE, const std::int32_t& decimals = INIT_DECIMALS);

    /** Gets the current value */
    float getValue() const;

    /**
     * Sets the current value
     * @param value Current value
     */
    void setValue(const float& value);

    /** Gets the default value */
    float getDefaultValue() const;

    /**
     * Sets the default value
     * @param defaultValue Default value
     */
    void setDefaultValue(const float& defaultValue);

    /** Determines whether the current value can be reset to its default */
    bool canReset() const override;

    /** Reset the current value to the default value */
    void reset() override;

    /** Gets the minimum value */
    float getMinimum() const;

    /**
     * Sets the minimum value
     * @param minimum Minimum value
     */
    void setMinimum(const float& minimum);

    /** Gets the maximum value */
    float getMaximum() const;
    
    /**
     * Sets the maximum value
     * @param maximum Maximum value
     */
    void setMaximum(const float& maximum);

    /**
     * Sets the value range
     * @param minimum Minimum value
     * @param maximum Maximum value
     */
    void setRange(const float& minimum, const float& maximum);

    /** Gets the suffix */
    QString getSuffix() const;

    /**
     * Sets the suffix
     * @param suffix Suffix
     */
    void setSuffix(const QString& suffix);

    /** Gets the number of decimals */
    std::uint32_t getDecimals() const;

    /**
     * Sets the number of decimals
     * @param decimals Number of decimals
     */
    void setDecimals(const std::uint32_t& decimals);

    /** Gets whether the value should update during interaction */
    bool getUpdateDuringDrag() const;

    /**
     * Sets whether the value should update during interaction
     * @param updateDuringDrag Whether the value should update during interaction
     */
    void setUpdateDuringDrag(const bool& updateDuringDrag);

    /** Returns whether the current value is at its minimum */
    bool isAtMinimum() const;

    /** Returns the length of the interval defined by the minimum and maximum value */
    double getIntervalLength() const;

    /** Returns the normalized value */
    float getNormalized() const;

    /*
    DecimalAction& operator= (const DecimalAction& other)
    {
        WidgetAction::operator=(other);

        _value              = other._value;
        _defaultValue       = other._defaultValue;
        _minimum            = other._minimum;
        _maximum            = other._maximum;
        _suffix             = other._suffix;
        _decimals           = other._decimals;
        _updateDuringDrag   = other._updateDuringDrag;

        return *this;
    }
    */

    /**
     * Creates a spin box widget to interact with the decimal action
     * @param parent Pointer to parent widget
     * @return Spin box widget
     */
    SpinBoxWidget* createSpinBoxWidget(QWidget* parent);

    /**
     * Creates a slider widget to interact with the decimal action
     * @param parent Pointer to parent widget
     * @return Slider widget
     */
    SliderWidget* createSliderWidget(QWidget* parent);

signals:

    /**
     * Signals that the current value changed
     * @param value Current value that changed
     */
    void valueChanged(const float& value);

    /**
     * Signals that the default value changed
     * @param defaultValue Default value that changed
     */
    void defaultValueChanged(const float& defaultValue);

    /**
     * Signals that the minimum value changed
     * @param minimum New minimum
     */
    void minimumChanged(const float& minimum);

    /**
     * Signals that the maximum value changed
     * @param maximum New maximum
     */
    void maximumChanged(const float& maximum);

    /**
     * Signals that the suffix changed
     * @param suffix New suffix
     */
    void suffixChanged(const QString& suffix);

    /**
     * Signals that the number of decimals changed
     * @param decimals Number of decimals
     */
    void decimalsChanged(const std::uint32_t& decimals);

protected:
    float          _value;                 /** Current value */
    float          _defaultValue;          /** Default value */
    float          _minimum;               /** Minimum value */
    float          _maximum;               /** Maximum value */
    QString         _suffix;                /** Suffix */
    std::uint32_t   _decimals;              /** Number of decimals */
    bool            _updateDuringDrag;      /** Whether the value should update during interaction */

    static constexpr float  INIT_MIN            = std::numeric_limits<float>::lowest();     /** Initialization minimum value */
    static constexpr float  INIT_MAX            = std::numeric_limits<float>::max();        /** Initialization maximum value */
    static constexpr float  INIT_VALUE          = 0.0;                                      /** Initialization value */
    static constexpr float  INIT_DEFAULT_VALUE  = 0.0;                                      /** Initialization default value */
    static constexpr int    INIT_DECIMALS       = 1;                                        /** Initialization number of decimals */
};

}
}
