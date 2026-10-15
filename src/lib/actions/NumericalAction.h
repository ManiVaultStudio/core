// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

#include "util/NumericalRange.h"

class QWidget;
class QPushButton;

namespace mv::gui {

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
    /** Templated classes with Q_OBJECT macro are not allowed, so use std functions instead */
    using ValueChangedCB                = std::function<void()>;
    using MinimumChangedCB              = std::function<void()>;
    using MaximumChangedCB              = std::function<void()>;
    using PrefixChangedCB               = std::function<void()>;
    using SuffixChangedCB               = std::function<void()>;
    using NumberOfDecimalsChangedCB     = std::function<void()>;
    using ResettableChangedCB           = std::function<void()>;

public:

    /** Describes the widget settings */
    enum WidgetFlag {
        SpinBox     = 0x00001,      /** Widget includes a spin box */
        Slider      = 0x00002,      /** Widget includes a slider */
        LineEdit    = 0x00004,      /** Widget includes a line edit */

        Default = SpinBox | Slider
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param minimum Minimum value
     * @param maximum Maximum value
     * @param value Value
     * @param numberOfDecimals Number of decimals
     */
    NumericalAction(QObject * parent, const QString& title, const NumericalType& minimum, NumericalType maximum, NumericalType value, std::uint32_t numberOfDecimals = INIT_NUMBER_OF_DECIMALS) :
        WidgetAction(parent, title),
        _value(),
        _minimum(std::numeric_limits<NumericalType>::lowest()),
        _maximum(std::numeric_limits<NumericalType>::max()),
        _numberOfDecimals(),
        _updateDuringDrag(true)
    {
        setText(title);
        setDefaultWidgetFlags(WidgetFlag::Default);
    }

    /** Gets the current value */
    virtual NumericalType getValue() const {
        return _value;
    }

    /**
     * Sets the current value
     * @param value Current value
     * @param silent Prevent notification of value change
     */
    virtual void setValue(NumericalType value, bool silent = false) {
        const auto clampedValue = std::max(_minimum, std::min(value, _maximum));

        if (clampedValue == _value)
            return;

        _value = clampedValue;

        if (silent)
            return;

        _valueChanged();

        saveToSettings();
    }

    /** Gets the minimum value */
    virtual NumericalType getMinimum() const {
        return _minimum;
    }

    /**
     * Sets the minimum value
     * @param minimum Minimum value
     */
    virtual void setMinimum(NumericalType minimum) {
        if (minimum == _minimum)
            return;

        _minimum = std::min(minimum, _maximum);

        _minimumChanged();
    }

    /** Gets the maximum value */
    virtual NumericalType getMaximum() const {
        return _maximum;
    }
    
    /**
     * Sets the maximum value
     * @param maximum Maximum value
     */
    virtual void setMaximum(NumericalType maximum) {
        if (maximum == _maximum)
            return;

        _maximum = std::max(maximum, _minimum);

        _maximumChanged();
    }
    
    /**
     * Gets the value range
     * @return Range
     */
    virtual util::NumericalRange<NumericalType> getRange() const {
        return { getMinimum(), getMaximum() };
    }

    /**
     * Sets the value range
     * @param range Range
     */
    virtual void setRange(util::NumericalRange<NumericalType> range) {
        setMinimum(range.getMinimum());
        setMaximum(range.getMaximum());
    }

    /**
     * Sets the value range
     * @param minimum Minimum value
     * @param maximum Maximum value
     */
    virtual void setRange(NumericalType minimum, NumericalType maximum) {
        setMinimum(minimum);
        setMaximum(maximum);
    }

    /** Gets the prefix */
    virtual QString getPrefix() const {
        return _prefix;
    }

    /**
     * Sets the prefix
     * @param prefix Prefix
     */
    virtual void setPrefix(const QString& prefix) {
        if (prefix == _prefix)
            return;

        _prefix = prefix;

        _prefixChanged();
    }

    /** Gets the suffix */
    virtual QString getSuffix() const {
        return _suffix;
    }

    /**
     * Sets the suffix
     * @param suffix Suffix
     */
    virtual void setSuffix(const QString& suffix) {
        if (suffix == _suffix)
            return;

        _suffix = suffix;

        _suffixChanged();
    }

    /** Gets the number of decimals */
    virtual std::uint32_t getNumberOfDecimals() const {
        return _numberOfDecimals;
    }

    /**
     * Sets the number of decimals
     * @param numberOfDecimals number of decimals
     */
    virtual void setNumberOfDecimals(std::uint32_t numberOfDecimals) {
        if (numberOfDecimals == _numberOfDecimals)
            return;

        _numberOfDecimals = numberOfDecimals;

        _numberOfDecimalsChanged();
    }

    /** Gets whether the value should update during interaction */
    virtual bool getUpdateDuringDrag() const {
        return _updateDuringDrag;
    }

    /**
     * Sets whether the value should update during interaction
     * @param updateDuringDrag Whether the value should update during interaction
     */
    virtual void setUpdateDuringDrag(bool updateDuringDrag) {
        if (updateDuringDrag == _updateDuringDrag)
            return;

        _updateDuringDrag = updateDuringDrag;
    }

    /** Returns whether the current value is at its minimum */
    virtual bool isAtMinimum() const {
        return _value == _minimum;
    }

    /** Returns whether the current value is at its maximum */
    virtual bool isAtMaximum() const {
        return _value == _maximum;
    }

    /** Returns the length of the interval defined by the minimum and maximum value */
    virtual double getIntervalLength() const  {
        return static_cast<double>(_maximum) - static_cast<double>(_minimum);
    }

    /** Returns the normalized value */
    virtual double getNormalized() const {
        const auto offset = static_cast<double>(_value) - static_cast<double>(_minimum);

        return static_cast<double>(offset / getIntervalLength());
    }

protected: // Numerical and auxiliary data
    NumericalType       _value;                 /** Current value */
    NumericalType       _minimum;               /** Minimum value */
    NumericalType       _maximum;               /** Maximum value */
    QString             _prefix;                /** Prefix string */
    QString             _suffix;                /** Suffix string */
    std::uint32_t       _numberOfDecimals;      /** Number of decimals */
    bool                _updateDuringDrag;      /** Whether the value should update during interaction */

protected: // Callbacks for implementations of the numerical action
    ValueChangedCB              _valueChanged;                  /** Callback which is called when the value changed */
    MinimumChangedCB            _minimumChanged;                /** Callback which is called when the minimum changed */
    MaximumChangedCB            _maximumChanged;                /** Callback which is called when the maximum changed */
    PrefixChangedCB             _prefixChanged;                 /** Callback which is called when the prefix changed */
    SuffixChangedCB             _suffixChanged;                 /** Callback which is called when the suffix changed */
    NumberOfDecimalsChangedCB   _numberOfDecimalsChanged;       /** Callback which is called when the number of decimals changed */

    static constexpr std::uint32_t  INIT_NUMBER_OF_DECIMALS = 1;        /** Initialization number of decimals */
};

}
