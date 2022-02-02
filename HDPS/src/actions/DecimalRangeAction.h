#pragma once

#include "DecimalAction.h"

namespace hdps {

namespace gui {

/**
 * Decimal range action class
 *
 * Stores a decimal range and creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
class DecimalRangeAction : public WidgetAction
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
     * @param defaultRangeMin Default range minimum
     * @param defaultRangeMax Default range maximum
     */
    DecimalRangeAction(QObject* parent, const QString& title = "", const float& limitMin = INIT_LIMIT_MIN, const float& limitMax = INIT_LIMIT_MAX, const float& rangeMin = INIT_RANGE_MIN, const float& rangeMax = INIT_RANGE_MAX, const float& defaultRangeMin = INIT_DEFAULT_RANGE_MIN, const float& defaultRangeMax = INIT_DEFAULT_RANGE_MAX);

    /**
     * Initialize the color action
     * @param limitMin Range lower limit
     * @param limitMax Range upper limit
     * @param rangeMin Range minimum
     * @param rangeMax Range maximum
     * @param defaultRangeMin Default range minimum
     * @param defaultRangeMax Default range maximum
     */
    void initialize(const float& limitMin = INIT_LIMIT_MIN, const float& limitMax = INIT_LIMIT_MAX, const float& rangeMin = INIT_RANGE_MIN, const float& rangeMax = INIT_RANGE_MAX, const float& defaultRangeMin = INIT_DEFAULT_RANGE_MIN, const float& defaultRangeMax = INIT_DEFAULT_RANGE_MAX);

    /**
     * Get range minimum
     * @return Range minimum
     */
    float getMinimum() const;

    /**
     * Set range minimum
     * @return Range minimum
     */
    void setMinimum(float minimum);

    /**
     * Get range maximum
     * @return Range maximum
     */
    float getMaximum() const;

    /**
     * Set range maximum
     * @return Range maximum
     */
    void setMaximum(float maximum);

    /**
     * Sets the range
     * @param minimum Range minimum
     * @param maximum Range maximum
     */
    void setRange(const float& minimum, const float& maximum);

public: // Settings

    /**
     * Set value from variant
     * @param value Value
     */
    void setValueFromVariant(const QVariant& value) override final;

    /**
     * Convert value to variant
     * @return Value as variant
     */
    QVariant valueToVariant() const override final;

    /**
     * Convert default value to variant
     * @return Default value as variant
     */
    QVariant defaultValueToVariant() const override final;

public: // Action getters

    DecimalAction& getRangeMinAction() { return _rangeMinAction; }
    DecimalAction& getRangeMaxAction() { return _rangeMaxAction; }

signals:

    /**
     * Signals that the range changed
     * @param minimum Range minimum
     * @param maximum Range maximum
     */
    void rangeChanged(const float& minimum, const float& maximum);

protected:
    DecimalAction       _rangeMinAction;        /** Minimum range decimal action */
    DecimalAction       _rangeMaxAction;        /** Maximum range decimal action */

protected:
    static constexpr float  INIT_LIMIT_MIN          = std::numeric_limits<float>::lowest();     /** Initialization minimum limit */
    static constexpr float  INIT_LIMIT_MAX          = std::numeric_limits<float>::max();        /** Initialization maximum limit */
    static constexpr float  INIT_RANGE_MIN          = 0.0f;                                     /** Initialization minimum range */
    static constexpr float  INIT_RANGE_MAX          = 100.0f;                                   /** Initialization maximum range */
    static constexpr float  INIT_DEFAULT_RANGE_MIN  = 0.0f;                                     /** Initialization default minimum range */
    static constexpr float  INIT_DEFAULT_RANGE_MAX  = 100.0f;                                   /** Initialization default maximum range */
};

}
}
