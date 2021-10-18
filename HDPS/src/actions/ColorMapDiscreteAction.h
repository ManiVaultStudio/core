#pragma once

#include "WidgetAction.h"
#include "IntegralAction.h"

namespace hdps {

namespace gui {

class ColorMapAction;

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
     * @param colorMapAction Reference to color map action
     */
    ColorMapDiscreteAction(ColorMapAction& colorMapAction);

    /** Determines whether the current color can be reset to its default */
    bool isResettable() const override;

    /** Reset to default */
    void reset() override;

public: // Action getters

    ColorMapAction& getColorMapAction() { return _colorMapAction; }
    IntegralAction& getNumberOfStepsAction() { return _numberOfStepsAction; }

protected:
    ColorMapAction&     _colorMapAction;            /** Reference to color map action */
    IntegralAction      _numberOfStepsAction;       /** Number of discrete steps action */

    /** Only color map settings action may instantiate this class */
    friend class ColorMapSettingsAction;
};

}
}
