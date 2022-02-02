#pragma once

#include "NumericalAction.h"

#include <QSpinBox>
#include <QSlider>
#include <QLineEdit>

class QWidget;
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
class IntegralAction : public NumericalAction<std::int32_t>
{
    Q_OBJECT

public:

    /** Spinbox widget class for integral action */
    class SpinBoxWidget : public QSpinBox
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param integralAction Pointer to integral action
         */
        SpinBoxWidget(QWidget* parent, IntegralAction* integralAction);
        
        friend class IntegralAction;
    };

    /** Slider widget class for integral action */
    class SliderWidget : public QSlider
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param integralAction Pointer to integral action
         */
        SliderWidget(QWidget* parent, IntegralAction* integralAction);

        friend class IntegralAction;
    };

    /** Line edit widget class for integral action */
    class LineEditWidget : public QLineEdit
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param integralAction Pointer to integral action
         */
        LineEditWidget(QWidget* parent, IntegralAction* integralAction);

        friend class IntegralAction;
    };

protected:

    /**
     * Get widget representation of the integral action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

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
    IntegralAction(QObject * parent, const QString& title, const std::int32_t& minimum = INIT_MIN, const std::int32_t& maximum = INIT_MAX, const std::int32_t& value = INIT_VALUE, const std::int32_t& defaultValue = INIT_DEFAULT_VALUE);

    /**
     * Initialize the integral action
     * @param minimum Minimum value
     * @param maximum Maximum value
     * @param value Value
     * @param defaultValue Default value
     */
    void initialize(const std::int32_t& minimum, const std::int32_t& maximum, const std::int32_t& value, const std::int32_t& defaultValue);

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
     * Signals that the prefix changed
     * @param prefix New prefix
     */
    void prefixChanged(const QString& prefix);

    /**
     * Signals that the suffix changed
     * @param suffix New suffix
     */
    void suffixChanged(const QString& suffix);

protected:
    static constexpr std::int32_t INIT_MIN              = 0;        /** Initialization minimum value */
    static constexpr std::int32_t INIT_MAX              = 100;      /** Initialization maximum value */
    static constexpr std::int32_t INIT_VALUE            = 0;        /** Initialization value */
    static constexpr std::int32_t INIT_DEFAULT_VALUE    = 0;        /** Initialization default value */
};

}
}
