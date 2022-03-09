#pragma once

#include "ColorMapAxisAction.h"
#include "ColorMapViewAction.h"
#include "ColorMapDiscreteAction.h"

#include "WidgetAction.h"
#include "DecimalAction.h"
#include "DecimalRangeAction.h"
#include "ToggleAction.h"
#include "TriggerAction.h"
#include "IntegralAction.h"

namespace hdps {

namespace gui {

class ColorMapAction;

/**
 * Color map settings action class
 *
 * Group action for color map settings
 *
 * @author Thomas Kroes
 */
class ColorMapSettingsAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for color map settings action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorMapSettingsAction Pointer to clusters action
         */
        Widget(QWidget* parent, ColorMapSettingsAction* colorMapSettingsAction);

    protected:
        ColorMapViewAction      _colorMapViewAction;    /** Color map view action */
    };

    /**
     * Get widget representation of the color map settings action
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
    ColorMapSettingsAction(ColorMapAction& colorMapAction);

public: // Action getters

    ColorMapAction& getColorMapAction() { return _colorMapAction; }
    ColorMapAxisAction& getHorizontalAxisAction() { return _horizontalAxisAction; }
    ColorMapAxisAction& getVerticalAxisAction() { return _verticalAxisAction; }
    ColorMapDiscreteAction& getDiscreteAction() { return _discreteAction; }

protected:
    ColorMapAction&             _colorMapAction;            /** Reference to color map action */
    ColorMapAxisAction          _horizontalAxisAction;      /** Horizontal axis action */
    ColorMapAxisAction          _verticalAxisAction;        /** Vertical axis action */
    ColorMapDiscreteAction      _discreteAction;            /** Discrete action */

    /** Only color map actions may instantiate this class */
    friend class ColorMapAction;
};

}
}
