#include "ColorMapSettings1DAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

using namespace hdps::util;

namespace hdps::gui {

ColorMapSettings1DAction::ColorMapSettings1DAction(ColorMapAction& colorMapAction) :
    GroupAction(&colorMapAction),
    _colorMapAction(colorMapAction)
{
    setText("Settings (one-dimensional)");
    setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));

    
    //layout->addWidget(settingsAction.getHorizontalAxisAction().createWidget(this));
    //layout->addWidget(settingsAction.getHorizontalAxisAction().getMirrorAction().createWidget(this));
    //layout->addWidget(settingsAction.getDiscreteAction().createWidget(this));
    //layout->addWidget(settingsAction.getEditor1DAction().createWidget(this));
}

}
