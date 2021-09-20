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
         * @param state State of the widget
         */
        Widget(QWidget* parent, ColorMapDiscreteAction* colorMapDiscreteAction, const WidgetActionWidget::State& state);
    };

    /**
     * Get widget representation of the color map discrete action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this, state);
    };

protected:

    /**
     * Constructor
     * @param colorMapAction Reference to color map action
     */
    ColorMapDiscreteAction(ColorMapAction& colorMapAction);

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
