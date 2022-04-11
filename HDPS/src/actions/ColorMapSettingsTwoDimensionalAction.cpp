#include "ColorMapSettingsTwoDimensionalAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

namespace hdps {

namespace gui {

ColorMapSettingsTwoDimensionalAction::ColorMapSettingsTwoDimensionalAction(ColorMapAction& colorMapAction) :
    WidgetAction(&colorMapAction),
    _colorMapAction(colorMapAction)
{
    setText("Settings (one-dimensional)");
    setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
}

ColorMapSettingsTwoDimensionalAction::Widget::Widget(QWidget* parent, ColorMapSettingsTwoDimensionalAction* colorMapSettingsTwoDimensionalAction) :
    WidgetActionWidget(parent, colorMapSettingsTwoDimensionalAction),
    _colorMapViewAction(colorMapSettingsTwoDimensionalAction->getColorMapAction())
{
    setAutoFillBackground(true);

    auto& settingsAction = colorMapSettingsTwoDimensionalAction->getColorMapAction().getSettingsAction();

    auto mainLayout     = new QHBoxLayout();
    auto settingsLayout = new QVBoxLayout();
    auto rangeLayout    = new QHBoxLayout();

    mainLayout->addLayout(settingsLayout);

    settingsLayout->addLayout(rangeLayout);
    settingsLayout->addWidget(settingsAction.getDiscreteAction().createWidget(this));

    rangeLayout->addWidget(settingsAction.getHorizontalAxisAction().createWidget(this));
    rangeLayout->addWidget(settingsAction.getVerticalAxisAction().createWidget(this));
    mainLayout->addWidget(_colorMapViewAction.createWidget(this));

    setLayout(mainLayout);
}

}
}
