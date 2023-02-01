#include "ColorMapSettings1DAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

using namespace hdps::util;

namespace hdps::gui {

ColorMapSettings1DAction::ColorMapSettings1DAction(ColorMapAction& colorMapAction) :
    WidgetAction(&colorMapAction),
    _colorMapAction(colorMapAction)
{
    setText("Settings (one-dimensional)");
    setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
}

ColorMapSettings1DAction::Widget::Widget(QWidget* parent, ColorMapSettings1DAction* colorMapSettings1DAction) :
    WidgetActionWidget(parent, colorMapSettings1DAction)
{
    setAutoFillBackground(true);

    auto& settingsAction = colorMapSettings1DAction->getColorMapAction().getSettingsAction();

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(settingsAction.getHorizontalAxisAction().createWidget(this));
    layout->addWidget(settingsAction.getDiscreteAction().createWidget(this));
    layout->addWidget(settingsAction.getEditor1DAction().createWidget(this));

    setLayout(layout);
}

}
