#include "ColorMapSettings2DAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

namespace hdps::gui {

ColorMapSettings2DAction::ColorMapSettings2DAction(ColorMapAction& colorMapAction) :
    WidgetAction(&colorMapAction),
    _colorMapAction(colorMapAction)
{
    setText("Settings (one-dimensional)");
    setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
}

ColorMapSettings2DAction::Widget::Widget(QWidget* parent, ColorMapSettings2DAction* colorMapSettings2DAction) :
    WidgetActionWidget(parent, colorMapSettings2DAction),
    _colorMapViewAction(colorMapSettings2DAction->getColorMapAction())
{
    setAutoFillBackground(true);

    auto& settingsAction = colorMapSettings2DAction->getColorMapAction().getSettingsAction();

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
