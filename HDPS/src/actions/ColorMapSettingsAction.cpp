#include "ColorMapSettingsAction.h"
#include "ColorMapAction.h"
#include "Application.h"

using namespace hdps::util;

namespace hdps {

namespace gui {

ColorMapSettingsAction::ColorMapSettingsAction(ColorMapAction* colorMapAction) :
    GroupAction(colorMapAction),
    _rangeMinAction(this, "Range minimum", std::numeric_limits<double>::min(), std::numeric_limits<double>::max(), 0, 0, 1),
    _rangeMaxAction(this, "Range maximum", std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), 1, 1, 1),
    _invertAction(this, "Invert"),
    _resetToDataRangeAction(this, "Reset to data range")
{
    setText("Color map settings");
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("cog"));

    _rangeMinAction.setToolTip("Minimum value of the color map");
    _rangeMaxAction.setToolTip("Maximum value of the color map");
    _invertAction.setToolTip("Mirror the color map horizontally");
    _resetToDataRangeAction.setToolTip("Reset the min/max value to the data range");

    connect(&_rangeMinAction, &DecimalAction::valueChanged, this, [this](const double& value) {
        if (value >= _rangeMaxAction.getValue())
            _rangeMaxAction.setValue(value);
    });

    connect(&_rangeMaxAction, &DecimalAction::valueChanged, this, [this](const double& value) {
        if (value <= _rangeMinAction.getValue())
            _rangeMinAction.setValue(value);
    });
}

}
}
