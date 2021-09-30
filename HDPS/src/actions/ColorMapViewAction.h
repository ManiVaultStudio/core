#pragma once

#include "WidgetAction.h"

namespace hdps {

namespace gui {

class ColorMapAction;

/**
 * Color map view action class
 *
 * Action class for color map display in a widget
 *
 * @author Thomas Kroes
 */
class ColorMapViewAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for color map view action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorMapViewAction Pointer to color map view action
         * @param state State of the widget
         */
        Widget(QWidget* parent, ColorMapViewAction* colorMapViewAction, const WidgetActionWidget::State& state);
    };

    /**
     * Get widget representation of the color map view action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     * @param state State of the widget (for stateful widgets)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this, state);
    };

protected:

    /**
     * Constructor
     * @param colorMapAction Reference to color map action
     */
    ColorMapViewAction(ColorMapAction& colorMapAction);

public: // Action getters

    ColorMapAction& getColorMapAction() { return _colorMapAction; }

protected:
    ColorMapAction&     _colorMapAction;    /** Reference to color map action */

    /** Only color map settings action may instantiate this class */
    friend class ColorMapSettingsAction;
};

}
}
