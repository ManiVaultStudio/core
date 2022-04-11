#include "ColorMapSettingsOneDimensionalAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

using namespace hdps::util;

namespace hdps {

namespace gui {

ColorMapSettingsOneDimensionalAction::ColorMapSettingsOneDimensionalAction(ColorMapAction& colorMapAction) :
    WidgetAction(&colorMapAction),
    _colorMapAction(colorMapAction)
{
    setText("Settings (one-dimensional)");
    setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
}

ColorMapSettingsOneDimensionalAction::Widget::Widget(QWidget* parent, ColorMapSettingsOneDimensionalAction* colorMapSettingsOneDimensionalAction) :
    WidgetActionWidget(parent, colorMapSettingsOneDimensionalAction)
{
    setAutoFillBackground(true);

    auto& settingsAction = colorMapSettingsOneDimensionalAction->getColorMapAction().getSettingsAction();

    auto mainLayout     = new QHBoxLayout();
    auto settingsLayout = new QVBoxLayout();
    auto rangeLayout    = new QHBoxLayout();

    mainLayout->setMargin(0);

    mainLayout->addLayout(settingsLayout);

    settingsLayout->addLayout(rangeLayout);
    settingsLayout->addWidget(settingsAction.getDiscreteAction().createWidget(this));

    rangeLayout->addWidget(settingsAction.getHorizontalAxisAction().createWidget(this));

    setLayout(mainLayout);
}

}
}
