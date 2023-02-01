#pragma once

#include "WidgetAction.h"

namespace hdps::gui {

class ColorMapAction;

/**
 * Color map settings one-dimensional action class
 *
 * Widget action class for one-dimensional color map settings
 *
 * @author Thomas Kroes
 */
class ColorMapSettings1DAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for one-dimensional color map settings action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorMapSettings1DAction Pointer to one-dimensional color map settings action
         */
        Widget(QWidget* parent, ColorMapSettings1DAction* colorMapSettings1DAction);
    };

    /**
     * Get widget representation of the one-dimensional color map settings action
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
    ColorMapSettings1DAction(ColorMapAction& colorMapAction);

public: // Action getters

    ColorMapAction& getColorMapAction() { return _colorMapAction; }

protected:
    ColorMapAction&     _colorMapAction;    /** Reference to color map action */

    /** Only color map settings action may instantiate this class */
    friend class ColorMapSettingsAction;
};

}
