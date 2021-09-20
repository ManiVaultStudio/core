#pragma once

#include "WidgetAction.h"
#include "DecimalRangeAction.h"
#include "ToggleAction.h"
#include "TriggerAction.h"

namespace hdps {

namespace gui {

class ColorMapAction;

/**
 * Color map axis action class
 *
 * Action class for configuring the color map along an axis (horizontal/vertical)
 *
 * @author Thomas Kroes
 */
class ColorMapAxisAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for color map axis action */
    class Widget : public hdps::gui::WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorMapAxisAction Pointer to color map axis action
         * @param state State of the widget
         */
        Widget(QWidget* parent, ColorMapAxisAction* colorMapAxisAction, const WidgetActionWidget::State& state);
    };

    /**
     * Get widget representation of the color map axis action
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
     * @param title Axis title
     */
    ColorMapAxisAction(ColorMapAction& colorMapAction, const QString& title);

    /** Reset to default */
    void reset() override;

public: // Action getters

    ColorMapAction& getColorMapAction() { return _colorMapAction; }
    DecimalRangeAction& getRangeAction() { return _rangeAction; }
    TriggerAction& getResetAction() { return _resetAction; }
    ToggleAction& getMirrorAction() { return _mirrorAction; }

protected:
    ColorMapAction&     _colorMapAction;    /** Reference to color map action */
    DecimalRangeAction  _rangeAction;       /** Range action */
    TriggerAction       _resetAction;       /** Reset to default range action */
    ToggleAction        _mirrorAction;      /** Mirror along the axis action */

    /** Only color map settings action may instantiate this class */
    friend class ColorMapSettingsAction;
};

}
}
