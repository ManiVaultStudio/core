#pragma once

#include "WidgetAction.h"

class QWidget;
class QPushButton;

namespace hdps {

namespace gui {

/**
 * Numerical action class
 *
 * Action class for manipulating a numerical value
 *
 * @author Thomas Kroes
 */
template<typename NumericalType>
class NumericalAction : public WidgetAction
{
    /** Templated classes with Q_OBJECT macro are not allowed, so use function pointers in stead */
    using ValueChangedCB                = std::function<void()>;
    using DefaultValueChangedCB         = std::function<void()>;
    using MinimumChangedCB              = std::function<void()>;
    using MaximumChangedCB              = std::function<void()>;
    using PrefixChangedCB               = std::function<void()>;
    using SuffixChangedCB               = std::function<void()>;
    using NumberOfDecimalsChangedCB     = std::function<void()>;

    /** Describes the widget configurations */
    enum class WidgetConfiguration {
        ValueSpinBox        = 0x00001,
        ValueSlider         = 0x00002,
        ResetPushButton     = 0x00004,

        Basic               = ValueSpinBox | ValueSlider,
        All                 = ValueSpinBox | ValueSlider | ResetPushButton
    };

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
    NumericalAction(QObject * parent, const QString& title, const NumericalType& minimum, const NumericalType& maximum, const NumericalType& value, const NumericalType& defaultValue, const std::uint32_t& numberOfDecimals = INIT_NUMBER_OF_DECIMALS) :
        WidgetAction(parent),
        _value(),
        _defaultValue(),
        _minimum(std::numeric_limits<NumericalType>::lowest()),
        _maximum(std::numeric_limits<NumericalType>::max()),
        _prefix(),
        _suffix(),
        _numberOfDecimals(),
        _updateDuringDrag(true),
        _widgetConfiguration(WidgetConfiguration::All),
        _valueChanged(),
        _defaultValueChanged(),
        _minimumChanged(),
        _maximumChanged(),
        _prefixChanged(),
        _suffixChanged(),
        _numberOfDecimalsChanged()
    {
        setText(title);
    }

    /**
     * Initialize the decimal action
     * @param minimum Minimum value
     * @param maximum Maximum value
     * @param value Value
     * @param defaultValue Default value
     */
    void initialize(const NumericalType& minimum, const NumericalType& maximum, const NumericalType& value, const NumericalType& defaultValue, const std::uint32_t& numberOfDecimals = INIT_NUMBER_OF_DECIMALS)
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

    /** Gets the current value */
    virtual NumericalType getValue() const final {
        return _value;
    }

    /**
     * Sets the current value
     * @param value Current value
     * @return Whether the value changed
     */
    virtual void setValue(const NumericalType& value) {
        if (value == _value)
            return;

        _value = std::max(_minimum, std::min(value, _maximum));

        _valueChanged();
    }

    /** Gets the default value */
    virtual NumericalType getDefaultValue() const final {
        return _defaultValue;
    }

    /**
     * Sets the default value
     * @param defaultValue Default value
     */
    virtual void setDefaultValue(const NumericalType& defaultValue) {
        if (defaultValue == _defaultValue)
            return;

        _defaultValue = std::max(_minimum, std::min(defaultValue, _maximum));

        _defaultValueChanged();
    }

    /** Determines whether the current value can be reset to its default */
    virtual bool canReset() const final {
        return _value != _defaultValue;
    }

    /** Reset the current value to the default value */
    virtual void reset() final {
        setValue(_defaultValue);
    }

    /** Gets the minimum value */
    virtual NumericalType getMinimum() const final {
        return _minimum;
    }

    /**
     * Sets the minimum value
     * @param minimum Minimum value
     */
    virtual void setMinimum(const NumericalType& minimum) final {
        if (minimum == _minimum)
            return;

        _minimum = std::min(minimum, _maximum);

        _minimumChanged();
    }

    /** Gets the maximum value */
    virtual NumericalType getMaximum() const final {
        return _maximum;
    }
    
    /**
     * Sets the maximum value
     * @param maximum Maximum value
     */
    virtual void setMaximum(const NumericalType& maximum) final {
        if (maximum == _maximum)
            return;

        _maximum = std::max(maximum, _minimum);

        _maximumChanged();
    }

    /**
     * Sets the value range
     * @param minimum Minimum value
     * @param maximum Maximum value
     */
    virtual void setRange(const NumericalType& minimum, const NumericalType& maximum) final {
        setMinimum(minimum);
        setMaximum(maximum);
    }

    /** Gets the prefix */
    virtual QString getPrefix() const final {
        return _prefix;
    }

    /**
     * Sets the prefix
     * @param prefix Prefix
     */
    virtual void setPrefix(const QString& prefix) final {
        if (prefix == _prefix)
            return;

        _prefix = prefix;

        _prefixChanged();
    }

    /** Gets the suffix */
    virtual QString getSuffix() const final {
        return _suffix;
    }

    /**
     * Sets the suffix
     * @param suffix Suffix
     */
    virtual void setSuffix(const QString& suffix) final {
        if (suffix == _suffix)
            return;

        _suffix = suffix;

        _suffixChanged();
    }

    /** Gets the number of decimals */
    virtual std::uint32_t getNumberOfDecimals() const final {
        return _numberOfDecimals;
    }

    /**
     * Sets the number of decimals
     * @param numberOfDecimals number of decimals
     */
    virtual void setNumberOfDecimals(const std::uint32_t& numberOfDecimals) final {
        if (numberOfDecimals == _numberOfDecimals)
            return;

        _numberOfDecimals = numberOfDecimals;

        _numberOfDecimalsChanged();
    }

    /** Gets whether the value should update during interaction */
    virtual bool getUpdateDuringDrag() const final {
        return _updateDuringDrag;
    }

    /**
     * Sets whether the value should update during interaction
     * @param updateDuringDrag Whether the value should update during interaction
     */
    virtual void setUpdateDuringDrag(const bool& updateDuringDrag) final {
        if (updateDuringDrag == _updateDuringDrag)
            return;

        _updateDuringDrag = updateDuringDrag;
    }

    /** Returns whether the current value is at its minimum */
    virtual bool isAtMinimum() const final {
        return _value == _minimum;
    }

    /** Returns whether the current value is at its maximum */
    virtual bool isAtMaximum() const final {
        return _value == _maximum;
    }

    /** Returns the length of the interval defined by the minimum and maximum value */
    virtual double getIntervalLength() const final {
        return static_cast<double>(_maximum) - static_cast<double>(_minimum);
    }

    /** Returns the normalized value */
    virtual double getNormalized() const final {
        const auto offset = static_cast<double>(_value) - static_cast<double>(_minimum);
        return static_cast<double>(offset / getIntervalLength());
    }

    /** Gets the widget configuration */
    virtual const WidgetConfiguration& getWidgetConfiguration() const final {
        return _widgetConfiguration;
    };

    /** Gets the widget configuration */
    virtual void  setWidgetConfiguration(const WidgetConfiguration& widgetConfiguration) final {
        _widgetConfiguration = widgetConfiguration;
    };

protected: // Numerical and auxiliary data
    NumericalType           _value;                 /** Current value */
    NumericalType           _defaultValue;          /** Default value */
    NumericalType           _minimum;               /** Minimum value */
    NumericalType           _maximum;               /** Maximum value */
    QString                 _prefix;                /** Prefix string */
    QString                 _suffix;                /** Suffix string */
    std::uint32_t           _numberOfDecimals;      /** Number of decimals */
    bool                    _updateDuringDrag;      /** Whether the value should update during interaction */
    WidgetConfiguration     _widgetConfiguration;   /** Widget configuration */

protected: // Callbacks for implementations of the numerical action
    ValueChangedCB              _valueChanged;                  /** Callback which is called when the value changed */
    DefaultValueChangedCB       _defaultValueChanged;           /** Callback which is called when the default value changed */
    MinimumChangedCB            _minimumChanged;                /** Callback which is called when the minimum changed */
    MaximumChangedCB            _maximumChanged;                /** Callback which is called when the maximum changed */
    PrefixChangedCB             _prefixChanged;                 /** Callback which is called when the prefix changed */
    SuffixChangedCB             _suffixChanged;                 /** Callback which is called when the suffix changed */
    NumberOfDecimalsChangedCB   _numberOfDecimalsChanged;       /** Callback which is called when the number of decimals changed */

    static constexpr std::uint32_t  INIT_NUMBER_OF_DECIMALS     = 1;        /** Initialization number of decimals */
};

}
}
