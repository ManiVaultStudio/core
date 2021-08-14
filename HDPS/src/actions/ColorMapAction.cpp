#include "ColorMapAction.h"

namespace hdps {

namespace gui {

ColorMapAction::ColorMapAction(QObject* parent, const QString& title /*= ""*/, const QString& colorMap /*= ""*/, const QString& defaultColorMap /*= ""*/) :
    OptionAction(parent)
{
    initialize(colorMap, defaultColorMap);
}

void ColorMapAction::initialize(const QString& colorMap /*= ""*/, const QString& defaultColorMap /*= ""*/)
{
}

}
}
