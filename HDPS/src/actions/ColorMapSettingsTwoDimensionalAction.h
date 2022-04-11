#pragma once

#include "ColorMapViewAction.h"

namespace hdps {

namespace gui {

class ColorMapAction;

/**
 * Color map settings two-dimensional action class
 *
 * Widget action class for two-dimensional color map settings
 *
 * @author Thomas Kroes
 */
class ColorMapSettingsTwoDimensionalAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for two-dimensional color map settings action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorMapSettingsTwoDimensionalAction Pointer to two-dimensional color map settings action
         */
        Widget(QWidget* parent, ColorMapSettingsTwoDimensionalAction* colorMapSettingsTwoDimensionalAction);

    protected:
        ColorMapViewAction  _colorMapViewAction;    /** Color map view action */
    };

    /**
     * Get widget representation of the two-dimensional color map settings action
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
    ColorMapSettingsTwoDimensionalAction(ColorMapAction& colorMapAction);

public: // Action getters

    ColorMapAction& getColorMapAction() { return _colorMapAction; }

protected:
    ColorMapAction&             _colorMapAction;            /** Reference to color map action */

    /** Only color map settings action may instantiate this class */
    friend class ColorMapSettingsAction;
};

}
}
