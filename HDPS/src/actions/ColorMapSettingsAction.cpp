#include "ColorMapSettingsAction.h"
#include "ColorMapAction.h"
#include "Application.h"

using namespace hdps::util;

namespace hdps {

namespace gui {

ColorMapSettingsAction::ColorMapSettingsAction(ColorMapAction* colorMapAction) :
    GroupAction(colorMapAction),
    _rangeMinAction(this, "Range minimum"),
    _rangeMaxAction(this, "Range maximum"),
    _invertedAction(this, "Invert"),
    _resetToDataRange(this, "Reset to data range")
{
    setText("Color map settings");
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("cog"));

    _rangeMinAction.setToolTip("Minimum value of the color map");
    _rangeMaxAction.setToolTip("Maximum value of the color map");
    _invertedAction.setToolTip("Mirror the color map horizontally");
    _resetToDataRange.setToolTip("Reset the min/max value to the data range");
}

}
}
