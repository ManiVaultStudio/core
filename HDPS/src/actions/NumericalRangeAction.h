// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GroupAction.h"

#include "util/NumericalRange.h"

namespace hdps::gui {

/**
 * Numerical range action base class
 *
 * Stores a numerical range and creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
template<typename NumericalType, typename NumericalActionType>
class NumericalRangeAction : public GroupAction
{
    /** Templated classes with Q_OBJECT macro are not allowed, so use std functions instead */
    using LimitsChangedCB   = std::function<void()>;
    using RangeChangedCB    = std::function<void()>;

public:
    using ValueType = NumericalType;

    /** Describes the widget settings */
    enum WidgetFlag {
        MinimumSpinBox  = 0x00001,      /** Widget includes a spin box for the minimum range*/
        MinimumLineEdit = 0x00002,      /** Widget includes a label for the minimum range*/
        Slider          = 0x00004,      /** Widget includes a slider */
        MaximumSpinBox  = 0x00008,      /** Widget includes a spin box for the maximum range */
        MaximumLineEdit = 0x00010,      /** Widget includes a label for the maximum range */

        Default = MinimumSpinBox | Slider | MaximumSpinBox
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param limits Limits
     * @param range Range
     */
    NumericalRangeAction(QObject* parent, const QString& title, const util::NumericalRange<NumericalType>& limits, const util::NumericalRange<NumericalType>& range) :
        GroupAction(parent, title),
        _rangeMinAction(this, "Minimum"),
        _rangeMaxAction(this, "Maximum")
    {
        setText(title);
        setDefaultWidgetFlags(WidgetFlag::Default);

        _rangeMinAction.setPrefix("min: ");
        _rangeMaxAction.setPrefix("max: ");

        initialize(limits, range);
    }

    /**
     * Initializes the numerical range action with \p limits and \p range
     * @param limits Limits
     * @param range Range
     */
    void initialize(const util::NumericalRange<NumericalType>& limits, const util::NumericalRange<NumericalType>& range)
    {
        _rangeMinAction.initialize(limits.getMinimum(), limits.getMaximum(), range.getMinimum());
        _rangeMaxAction.initialize(limits.getMinimum(), limits.getMaximum(), range.getMaximum());
    }

    /**
     * Get range minimum
     * @return Range minimum
     */
    NumericalType getMinimum() const {
        return _rangeMinAction.getValue();
    }

    /**
     * Set range minimum to \p minimum
     * @return Range minimum
     */
    void setMinimum(NumericalType minimum) {
        if (minimum == _rangeMinAction.getValue())
            return;

        _rangeMinAction.setValue(minimum);

        _rangeChanged();
    }

    /**
     * Get range maximum
     * @return Range maximum
     */
    NumericalType getMaximum() const {
        return _rangeMaxAction.getValue();
    }

    /**
     * Set range maximum to \p maximum
     * @return Range maximum
     */
    void setMaximum(NumericalType maximum) {
        if (maximum == _rangeMaxAction.getValue())
            return;

        _rangeMaxAction.setValue(maximum);

        _rangeChanged();
    }

    /** Get the range */
    util::NumericalRange<NumericalType> getRange() const {
        return util::NumericalRange<NumericalType>(_rangeMinAction.getValue(), _rangeMaxAction.getValue());
    }

    /**
     * Sets the range to \p range
     * @param range Range
     */
    void setRange(const util::NumericalRange<NumericalType>& range) {
        if (range == getRange())
            return;

        QSignalBlocker rangeMinActionBlocker(&_rangeMinAction);
        QSignalBlocker rangeMaxActionBlocker(&_rangeMaxAction);

        _rangeMinAction.setValue(range.getMinimum());
        _rangeMaxAction.setValue(range.getMaximum());

        _rangeChanged();
    }

    /**
     * Get limits minimum
     * @return Limits minimum
     */
    NumericalType getLimitsMinimum() const {
        return _rangeMinAction.getMinimum();
    }

    /**
     * Set limits minimum to \p limitsMinimum
     * @param limitsMinimum Limits minimum
     */
    void setLimitsMinimum(NumericalType limitsMinimum) {
        if (limitsMinimum == _rangeMinAction.getMinimum())
            return;

        _rangeMinAction.setMinimum(limitsMinimum);
        _rangeMaxAction.setMinimum(limitsMinimum);

        _limitsChanged();
    }

    /**
     * Get limits maximum
     * @return Limits maximum
     */
    NumericalType getLimitsMaximum() const {
        return _rangeMaxAction.getMaximum();
    }

    /**
     * Set limits maximum to \p limitsMaximum
     * @param limitsMaximum Limits maximum
     */
    void setLimitsMaximum(NumericalType limitsMaximum) {
        if (limitsMaximum == _rangeMaxAction.getMaximum())
            return;

        _rangeMinAction.setMaximum(limitsMaximum);
        _rangeMaxAction.setMaximum(limitsMaximum);

        _limitsChanged();
    }

    /** Get the limits */
    util::NumericalRange<NumericalType> getLimits() const {
        return util::NumericalRange<NumericalType>(_rangeMinAction.getMinimum(), _rangeMaxAction.getMaximum());
    }

    /**
     * Set limits to \p limits
     * @param limits Limits
     */
    void setLimits(const util::NumericalRange<NumericalType>& limits) {
        if (limits == getLimits())
            return;

        _rangeMinAction.setMinimum(limits.getMinimum());
        _rangeMaxAction.setMinimum(limits.getMinimum());

        _rangeMinAction.setMaximum(limits.getMaximum());
        _rangeMaxAction.setMaximum(limits.getMaximum());

        _limitsChanged();
    }

public: // Serialization

    /**
     * Load numerical range action from variant map
     * @param Variant map representation of the numerical range action
     */
    void fromVariantMap(const QVariantMap& variantMap) override
    {
        WidgetAction::fromVariantMap(variantMap);

        _rangeMinAction.fromParentVariantMap(variantMap);
        _rangeMaxAction.fromParentVariantMap(variantMap);
    }

    /**
     * Save numerical range action to variant map
     * @return Variant map representation of the numerical range action
     */
    QVariantMap toVariantMap() const override
    {
        auto variantMap = WidgetAction::toVariantMap();

        _rangeMinAction.insertIntoVariantMap(variantMap);
        _rangeMaxAction.insertIntoVariantMap(variantMap);

        return variantMap;
    }

public: // Action getters

    NumericalActionType& getRangeMinAction() { return _rangeMinAction; }
    NumericalActionType& getRangeMaxAction() { return _rangeMaxAction; }

protected:
    NumericalActionType     _rangeMinAction;        /** Minimum range numerical action */
    NumericalActionType     _rangeMaxAction;        /** Maximum range numerical action */
    LimitsChangedCB         _limitsChanged;         /** Callback which is called when the limits change */
    RangeChangedCB          _rangeChanged;          /** Callback which is called when the range changes */
};

}
