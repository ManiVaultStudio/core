#pragma once

#include "WidgetAction.h"
#include "DecimalAction.h"
#include "ToggleAction.h"
#include "TriggerAction.h"

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
    class Widget : public hdps::gui::WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorMapSettingsAction Pointer to clusters action
         * @param state State of the widget
         */
        Widget(QWidget* parent, ColorMapSettingsAction* colorMapSettingsAction, const WidgetActionWidget::State& state);
    };

    /**
     * Get widget representation of the clusters action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this, state);
    };

protected:

    /**
     * Constructor
     * @param colorMapAction Pointer to color map action
     */
    ColorMapSettingsAction(ColorMapAction* colorMapAction);

public:

    /**
     * Enable/disable range editing
     * @param rangeEditingEnabled Whether range editing is enabled or not
     */
    void setRangeEditingEnabled(const bool& rangeEditingEnabled);

public: // Action getters

    ToggleAction& getInvertAction() { return _invertAction; }
    DecimalAction& getRangeMinAction() { return _rangeMinAction; }
    DecimalAction& getRangeMaxAction() { return _rangeMaxAction; }
    TriggerAction& getResetToDataRangeAction() { return _resetToDefaultRangeAction; }

protected:
    DecimalAction   _rangeMinAction;                /** Range minimum action */
    DecimalAction   _rangeMaxAction;                /** Range maximum action */
    TriggerAction   _resetToDefaultRangeAction;     /** Reset minimum/maximum range to data range */
    ToggleAction    _invertAction;                  /** Color map is horizontally mirrored action */

    /** Only color map action may instantiate this class */
    friend class ColorMapAction;
};

}
}
