#include "ColorMapSettingsAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

using namespace hdps::util;

namespace hdps {

namespace gui {

ColorMapSettingsAction::ColorMapSettingsAction(ColorMapAction& colorMapAction) :
    WidgetAction(&colorMapAction),
    _colorMapAction(colorMapAction),
    _horizontalAxisAction(colorMapAction, "Horizontal"),
    _verticalAxisAction(colorMapAction, "Vertical"),
    _discreteAction(colorMapAction)
{
    setText("Settings");
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("cog"));

    _horizontalAxisAction.setToolTip("Range of the color map");
    _horizontalAxisAction.setToolTip("Mirror the color map horizontally");
    _verticalAxisAction.setToolTip("Mirror the color map vertically");

    const auto updateRangeActions = [this]() -> void {
        _horizontalAxisAction.setText(_colorMapAction.getColorMapType() == ColorMap::Type::OneDimensional ? "Range" : "Horizontal");
    };

    connect(&colorMapAction, &ColorMapAction::colorMapTypeChanged, this, updateRangeActions);

    updateRangeActions();
}

void ColorMapSettingsAction::reset()
{
    _colorMapAction.reset();
    _horizontalAxisAction.reset();
    _verticalAxisAction.reset();
    _discreteAction.reset();
}

ColorMapSettingsAction::Widget::Widget(QWidget* parent, ColorMapSettingsAction* colorMapSettingsAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, colorMapSettingsAction, state),
    _colorMapViewAction(colorMapSettingsAction->getColorMapAction())
{
    auto mainLayout     = new QHBoxLayout();
    auto settingsLayout = new QVBoxLayout();
    auto rangeLayout    = new QHBoxLayout();

    mainLayout->setMargin(0);

    switch (colorMapSettingsAction->getColorMapAction().getColorMapType())
    {
        case ColorMap::Type::OneDimensional:
        {
            rangeLayout->addWidget(colorMapSettingsAction->getHorizontalAxisAction().createWidget(this));
            break;
        }

        case ColorMap::Type::TwoDimensional:
        {
            mainLayout->addWidget(_colorMapViewAction.createWidget(this));
            rangeLayout->addWidget(colorMapSettingsAction->getHorizontalAxisAction().createWidget(this));
            rangeLayout->addWidget(colorMapSettingsAction->getVerticalAxisAction().createWidget(this));
            break;
        }

        default:
            break;
    }

    mainLayout->addLayout(settingsLayout);

    settingsLayout->addLayout(rangeLayout);
    settingsLayout->addWidget(colorMapSettingsAction->getDiscreteAction().createWidget(this));

    setLayout(mainLayout);
}

}
}
