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
         * @param state State of the widget
         */
        DecimalRangeWidget(QWidget* parent, DecimalRangeAction* decimalRangeAction, const WidgetActionWidget::State& state);

    protected:
        friend class DecimalRangeAction;
    };

protected:

    /**
     * Get widget representation of the decimal range action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override {
        return new DecimalRangeWidget(parent, this, state);
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

    /** Get range minimum */
    float getMinimum() const;

    /** Get range minimum */
    float getMaximum() const;

    /**
     * Sets the range
     * @param minimum Range minimum
     * @param maximum Range maximum
     */
    void setRange(const float& minimum, const float& maximum);

    /** Determines whether the current range can be reset to its default */
    bool isResettable() const override;

    /** Reset the current range to the default value */
    void reset() override;

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
