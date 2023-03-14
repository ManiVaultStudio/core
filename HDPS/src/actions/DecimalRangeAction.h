#pragma once

#include "NumericalRangeAction.h"
#include "DecimalAction.h"

namespace hdps::gui {

/**
 * Decimal range action class
 *
 * Stores a decimal range and creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
class DecimalRangeAction : public NumericalRangeAction<float, DecimalAction>
{
    Q_OBJECT

public:

    /** Widget class for decimal range action */
    class DecimalRangeWidget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param decimalRangeAction Pointer to decimal range action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        DecimalRangeWidget(QWidget* parent, DecimalRangeAction* decimalRangeAction, const std::int32_t& widgetFlags = 0);

    protected:
        friend class DecimalRangeAction;
    };

protected:

    /**
     * Get widget representation of the decimal range action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new DecimalRangeWidget(parent, this);
    };

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
    DecimalRangeAction(QObject* parent, const QString& title, const float& limitMin = INIT_LIMIT_MIN, const float& limitMax = INIT_LIMIT_MAX, const float& rangeMin = INIT_RANGE_MIN, const float& rangeMax = INIT_RANGE_MAX);

signals:

    /**
     * Signals that the range changed
     * @param minimum Range minimum
     * @param maximum Range maximum
     */
    void rangeChanged(const float& minimum, const float& maximum);

protected:
    static constexpr float  INIT_LIMIT_MIN  = std::numeric_limits<float>::lowest();     /** Initialization minimum limit */
    static constexpr float  INIT_LIMIT_MAX  = std::numeric_limits<float>::max();        /** Initialization maximum limit */
    static constexpr float  INIT_RANGE_MIN  = 0.0f;                                     /** Initialization minimum range */
    static constexpr float  INIT_RANGE_MAX  = 100.0f;                                   /** Initialization maximum range */
};

}
