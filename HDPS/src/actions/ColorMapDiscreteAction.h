#pragma once

#include "WidgetAction.h"
#include "IntegralAction.h"

namespace hdps {

namespace gui {

class ColorMapSettingsAction;

/**
 * Color map discrete action class
 *
 * Action class for discrete color map settings
 *
 * @author Thomas Kroes
 */
class ColorMapDiscreteAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for color map discrete action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorMapDiscreteAction Pointer to color map discrete action
         */
        Widget(QWidget* parent, ColorMapDiscreteAction* colorMapDiscreteAction);
    };

    /**
     * Get widget representation of the color map discrete action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

protected:

    /**
     * Constructor
     * @param colorMapSettingsAction Reference to color map settings action
     */
    ColorMapDiscreteAction(ColorMapSettingsAction& colorMapSettingsAction);

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

    IntegralAction& getNumberOfStepsAction() { return _numberOfStepsAction; }

protected:
    IntegralAction      _numberOfStepsAction;       /** Number of discrete steps action */

    /** Only color map settings action may instantiate this class */
    friend class ColorMapSettingsAction;
};

}
}
