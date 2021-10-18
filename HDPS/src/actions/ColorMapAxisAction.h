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
         */
        Widget(QWidget* parent, ColorMapAxisAction* colorMapAxisAction);
    };

    /**
     * Get widget representation of the color map axis action
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
     * @param title Axis title
     */
    ColorMapAxisAction(ColorMapAction& colorMapAction, const QString& title);

    /** Determines whether the current color can be reset to its default */
    bool isResettable() const override;

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
