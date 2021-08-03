#pragma once

#include "WidgetAction.h"

class QWidget;
class QDoubleSpinBox;
class QSlider;
class QPushButton;

namespace hdps {

namespace gui {

/**
 * Decimal widget action class
 *
 * Stores a double value and creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
class DecimalAction : public WidgetAction
{
    Q_OBJECT

public:

    /**
     * Spinbox widget class for decimal action
     */
    class SpinBoxWidget : public WidgetAction::Widget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param decimalAction Pointer to decimal action
         */
        SpinBoxWidget(QWidget* parent, DecimalAction* decimalAction);
        
    public:

        /** Gets the double spinbox widget */
        QDoubleSpinBox* getDoubleSpinBox() { return _doubleSpinBox; }

    protected:
        QDoubleSpinBox*     _doubleSpinBox;     /** Double spinbox widget */

        friend class DecimalAction;
    };

    /**
     * Slider widget class for decimal action
     */
    class SliderWidget : public WidgetAction::Widget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param decimalAction Pointer to decimal action
         */
        SliderWidget(QWidget* parent, DecimalAction* decimalAction);

    public:

        /** Gets the slider widget */
        QSlider* getSlider() { return _slider; }

    protected:
        QSlider*    _slider;        /** Slider widget */

        friend class DecimalAction;
    };

protected:

    /**
     * Get widget representation of the action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const Widget::State& state = Widget::State::Standard) override;;

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
    DecimalAction(QObject * parent, const QString& title, const double& minimum = MIN_VALUE, const double& maximum = MAX_VALUE, const double& value = VALUE, const double& defaultValue = DEFAULT_VALUE, const std::int32_t& decimals = DECIMALS);

    /**
     * Initialize the decimal action
     * @param minimum Minimum value
     * @param maximum Maximum value
     * @param value Value
     * @param defaultValue Default value
     * @param decimals Number of decimals
     */
    void initialize(const double& minimum = MIN_VALUE, const double& maximum = MAX_VALUE, const double& value = VALUE, const double& defaultValue = DEFAULT_VALUE, const std::int32_t& decimals = DECIMALS);

    /** Gets the current value */
    double getValue() const;

    /**
     * Sets the current value
     * @param value Current value
     */
    void setValue(const double& value);

    /** Gets the default value */
    double getDefaultValue() const;

    /**
     * Sets the default value
     * @param defaultValue Default value
     */
    void setDefaultValue(const double& defaultValue);

    /** Determines whether the current value can be reset to its default */
    bool canReset() const;

    /** Reset the current value to the default value */
    void reset();

    /** Gets the minimum value */
    double getMinimum() const;

    /**
     * Sets the minimum value
     * @param minimum Minimum value
     */
    void setMinimum(const double& minimum);

    /** Gets the maximum value */
    double getMaximum() const;
    
    /**
     * Sets the maximum value
     * @param maximum Maximum value
     */
    void setMaximum(const double& maximum);

    /**
     * Sets the value range
     * @param minimum Minimum value
     * @param maximum Maximum value
     */
    void setRange(const double& minimum, const double& maximum);

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
     * Create a spin box widget for the decimal action
     * @param parent Pointer to parent widget
     * @return Spin box widget
     */
    SpinBoxWidget* createSpinBoxWidget(QWidget* parent);

    /**
     * Create a slider widget for the decimal action
     * @param parent Pointer to parent widget
     * @return Slider widget
     */
    SliderWidget* createSliderWidget(QWidget* parent);

signals:

    /**
     * Signals that the current value changed
     * @param value Current value that changed
     */
    void valueChanged(const double& value);

    /**
     * Signals that the default value changed
     * @param defaultValue Default value that changed
     */
    void defaultValueChanged(const double& defaultValue);

    /**
     * Signals that the minimum value changed
     * @param minimum New minimum
     */
    void minimumChanged(const double& minimum);

    /**
     * Signals that the maximum value changed
     * @param maximum New maximum
     */
    void maximumChanged(const double& maximum);

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
    double          _value;                 /** Current value */
    double          _defaultValue;          /** Default value */
    double          _minimum;               /** Minimum value */
    double          _maximum;               /** Maximum value */
    QString         _suffix;                /** Suffix */
    std::uint32_t   _decimals;              /** Number of decimals */
    bool            _updateDuringDrag;      /** Whether the value should update during interaction */

    static constexpr int        SLIDER_MULTIPLIER   = 1000;     /** Slider multiplier */
    static constexpr double     MIN_VALUE           = 0.0;      /** Default initialization minimum value */
    static constexpr double     MAX_VALUE           = 100.0;    /** Default initialization maximum value */
    static constexpr double     VALUE               = 0.0;      /** Default initialization value */
    static constexpr double     DEFAULT_VALUE       = 0.0;      /** Default initialization default value */
    static constexpr int        DECIMALS            = 1;        /** Default initialization number of decimals */
};

}
}
