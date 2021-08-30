#include "ColorMapSettingsAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QGridLayout>

using namespace hdps::util;

namespace hdps {

namespace gui {

ColorMapSettingsAction::ColorMapSettingsAction(ColorMapAction* colorMapAction) :
    WidgetAction(colorMapAction),
    _rangeMinAction(this, "Range minimum", 0, 1, 0, 0, 2),
    _rangeMaxAction(this, "Range maximum", 0, 1, 1, 1, 2),
    _resetToDefaultRangeAction(this, "Reset to default range"),
    _invertAction(this, "Mirror horizontally")
{
    setText("Settings");
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

ColorMapSettingsAction::Widget::Widget(QWidget* parent, ColorMapSettingsAction* colorMapSettingsAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, colorMapSettingsAction, state)
{
    auto layout = new QGridLayout();

    layout->addWidget(colorMapSettingsAction->getRangeMinAction().createLabelWidget(this), 0, 0);
    layout->addWidget(colorMapSettingsAction->getRangeMinAction().createWidget(this), 0, 1);

    layout->addWidget(colorMapSettingsAction->getRangeMaxAction().createLabelWidget(this), 1, 0);
    layout->addWidget(colorMapSettingsAction->getRangeMaxAction().createWidget(this), 1, 1);

    layout->addWidget(colorMapSettingsAction->getResetToDataRangeAction().createWidget(this), 2, 1);

    layout->addWidget(colorMapSettingsAction->getInvertAction().createWidget(this), 3, 1);

    setPopupLayout(layout);
}

}
}
