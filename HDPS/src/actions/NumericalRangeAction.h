#pragma once

#include "WidgetAction.h"

namespace hdps::gui {

/**
 * Numerical range action base class
 *
 * Stores a numerical range and creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
template<typename NumericalType, typename NumericalActionType>
class NumericalRangeAction : public WidgetAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param limitMin Range lower limit
     * @param limitMax Range upper limit
     * @param rangeMin Range minimum
     * @param rangeMax Range maximum
     */
    NumericalRangeAction(QObject* parent, const QString& title, const NumericalType& limitMin, const NumericalType& limitMax, const NumericalType& rangeMin, const NumericalType& rangeMax) :
        WidgetAction(parent),
        _rangeMinAction(this, "Range Minimum"),
        _rangeMaxAction(this, "Range Maximum")
    {
        setText(title);
        setSerializationName("Range");

        _rangeMinAction.setSerializationName("Min");
        _rangeMaxAction.setSerializationName("Max");

        initialize(limitMin, limitMax, rangeMin, rangeMax);
    }

    /**
     * Initializes the numerical range action
     * @param limitMin Range lower limit
     * @param limitMax Range upper limit
     * @param rangeMin Range minimum
     */
    void initialize(const NumericalType& limitMin, const NumericalType& limitMax, const NumericalType& rangeMin, const NumericalType& rangeMax)
    {
        _rangeMinAction.initialize(limitMin, limitMax, rangeMin, rangeMin);
        _rangeMaxAction.initialize(limitMin, limitMax, rangeMax, rangeMax);
    }

    /**
     * Get range minimum
     * @return Range minimum
     */
    NumericalType getMinimum() const
    {
        return _rangeMinAction.getValue();
    }

    /**
     * Set range minimum to \p minimum
     * @return Range minimum
     */
    void setMinimum(NumericalType minimum)
    {
        _rangeMinAction.setValue(minimum);
    }

    /**
     * Get range maximum
     * @return Range maximum
     */
    NumericalType getMaximum() const
    {
        return _rangeMaxAction.getValue();
    }

    /**
     * Set range maximum to \p maximum
     * @return Range maximum
     */
    void setMaximum(NumericalType maximum)
    {
        _rangeMaxAction.setValue(maximum);
    }

    /**
     * Sets the range to \p minimum and \p maximum
     * @param minimum Range minimum
     * @param maximum Range maximum
     */
    void setRange(const NumericalType& minimum, const NumericalType& maximum)
    {
        Q_ASSERT(minimum < maximum);

        if (minimum > maximum)
            return;

        _rangeMinAction.initialize(minimum, maximum, minimum, minimum);
        _rangeMaxAction.initialize(minimum, maximum, maximum, maximum);
    }

    /**
     * Get limits minimum
     * @return Limits minimum
     */
    NumericalType getLimitsMinimum() const
    {
        return _rangeMinAction.getMinimum();
    }

    /**
     * Set limits minimum to \p limitsMinimum
     * @param limitsMinimum Limits minimum
     */
    void setLimitsMinimum(NumericalType limitsMinimum)
    {
        _rangeMinAction.setMinimum(limitsMinimum);
    }

    /**
     * Get limits maximum
     * @return Limits maximum
     */
    NumericalType getLimitsMaximum() const
    {
        return _rangeMaxAction.getMaximum();
    }

    /**
     * Set limits maximum to \p limitsMaximum
     * @param limitsMaximum Limits maximum
     */
    void setLimitsMaximum(NumericalType limitsMaximum)
    {
        _rangeMaxAction.setMaximum(limitsMaximum);
    }

    /**
     * Set limits to \p limitsMinimum and \p limitsMaximum
     * @param limitsMinimum Limits minimum
     * @param limitsMaximum Limits maximum
     */
    void setLimits(const NumericalType& limitsMinimum, const NumericalType& limitsMaximum)
    {
        Q_ASSERT(limitsMinimum < limitsMaximum);

        if (limitsMinimum > limitsMaximum)
            return;

        setLimitsMinimum(limitsMinimum);
        setLimitsMaximum(limitsMaximum);
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
    NumericalActionType     _rangeMinAction;    /** Minimum range numerical action */
    NumericalActionType     _rangeMaxAction;    /** Maximum range numerical action */
};

}
