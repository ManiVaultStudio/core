#pragma once

#include "GroupAction.h"
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
class ColorMapSettingsAction : public GroupAction
{
    Q_OBJECT

protected:

    /**
     * Constructor
     * @param colorMapAction Pointer to color map action
     */
    ColorMapSettingsAction(ColorMapAction* colorMapAction);

public: // Action getters

    DecimalAction& getRangeMinAction() { return _rangeMinAction; }
    DecimalAction& getRangeMaxAction() { return _rangeMaxAction; }
    TriggerAction& getResetToDataRangeAction() { return _resetToDefaultRangeAction; }
    ToggleAction& getInvertAction() { return _invertAction; }

protected:
    DecimalAction   _rangeMinAction;            /** Range minimum action */
    DecimalAction   _rangeMaxAction;            /** Range maximum action */
    TriggerAction   _resetToDefaultRangeAction;    /** Reset minimum/maximum range to data range */
    ToggleAction    _invertAction;              /** Color map is horizontally mirrored action */

    /** Only color map action may instantiate this class */
    friend class ColorMapAction;
};

}
}
