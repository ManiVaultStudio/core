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
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("sliders-h"));
    setMayReset(true);

    const auto updateRangeActions = [this]() -> void {
        _horizontalAxisAction.setText(_colorMapAction.getColorMapType() == ColorMap::Type::OneDimensional ? "Range" : "Horizontal");
    };

    connect(&colorMapAction, &ColorMapAction::typeChanged, this, updateRangeActions);

    const auto updateResettable = [this]() {
        setResettable(isResettable());
    };

    connect(&_horizontalAxisAction, &ColorMapAxisAction::resettableChanged, this, updateResettable);
    connect(&_verticalAxisAction, &ColorMapAxisAction::resettableChanged, this, updateResettable);
    connect(&_discreteAction, &ColorMapDiscreteAction::resettableChanged, this, updateResettable);

    updateRangeActions();
}

bool ColorMapSettingsAction::isResettable() const
{
    return _horizontalAxisAction.isResettable() | _verticalAxisAction.isResettable() | _discreteAction.isResettable();
}

void ColorMapSettingsAction::reset()
{
    _horizontalAxisAction.reset();
    _verticalAxisAction.reset();
    _discreteAction.reset();
}

ColorMapSettingsAction::Widget::Widget(QWidget* parent, ColorMapSettingsAction* colorMapSettingsAction) :
    WidgetActionWidget(parent, colorMapSettingsAction),
    _colorMapViewAction(colorMapSettingsAction->getColorMapAction())
{
    auto mainLayout     = new QHBoxLayout();
    auto settingsLayout = new QVBoxLayout();
    auto rangeLayout    = new QHBoxLayout();

    mainLayout->setMargin(0);

    mainLayout->addLayout(settingsLayout);

    settingsLayout->addLayout(rangeLayout);
    settingsLayout->addWidget(colorMapSettingsAction->getDiscreteAction().createWidget(this));

    switch (colorMapSettingsAction->getColorMapAction().getColorMapType())
    {
        case ColorMap::Type::OneDimensional:
        {
            rangeLayout->addWidget(colorMapSettingsAction->getHorizontalAxisAction().createWidget(this));
            break;
        }

        case ColorMap::Type::TwoDimensional:
        {
            rangeLayout->addWidget(colorMapSettingsAction->getHorizontalAxisAction().createWidget(this));
            rangeLayout->addWidget(colorMapSettingsAction->getVerticalAxisAction().createWidget(this));
            mainLayout->addWidget(_colorMapViewAction.createWidget(this));
            break;
        }

        default:
            break;
    }

    setLayout(mainLayout);
}

}
}
