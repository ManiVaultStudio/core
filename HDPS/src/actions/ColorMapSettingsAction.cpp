#include "ColorMapSettingsAction.h"
#include "ColorMapAction.h"
#include "Application.h"

using namespace hdps::util;

namespace hdps {

namespace gui {

ColorMapSettingsAction::ColorMapSettingsAction(ColorMapAction* colorMapAction) :
    GroupAction(colorMapAction),
    _rangeMinAction(this, "Range minimum", 0, 1, 0, 0),
    _rangeMaxAction(this, "Range maximum", 0, 1, 1, 1),
    _invertAction(this, "Invert"),
    _resetToDefaultRangeAction(this, "Reset to default range")
{
    setText("Color map settings");
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("cog"));

    _rangeMinAction.setToolTip("Minimum value of the color map");
    _rangeMaxAction.setToolTip("Maximum value of the color map");
    _invertAction.setToolTip("Mirror the color map horizontally");
    _resetToDefaultRangeAction.setToolTip("Reset the min/max value to the data range");

    const auto update = [this]() {
        _resetToDefaultRangeAction.setEnabled(_rangeMinAction.canReset() || _rangeMaxAction.canReset());
    };

    connect(&_rangeMinAction, &DecimalAction::valueChanged, this, [this, update](const double& value) {
        if (value >= _rangeMaxAction.getValue())
            _rangeMaxAction.setValue(value);

        update();
    });

    connect(&_rangeMaxAction, &DecimalAction::valueChanged, this, [this, update](const double& value) {
        if (value <= _rangeMinAction.getValue())
            _rangeMinAction.setValue(value);

        update();
    });

    connect(&_resetToDefaultRangeAction, &TriggerAction::triggered, this, [this, update]() {
        _rangeMinAction.reset();
        _rangeMaxAction.reset();
    });

    update();
}

}
}
