#include "ColorMap1DAction.h"

using namespace hdps::util;

namespace hdps::gui {

ColorMap1DAction::ColorMap1DAction(QObject* parent, const QString& title /*= ""*/, const QString& colorMap /*= "RdYlBu"*/) :
    ColorMapAction(parent, "Color Map", ColorMap::Type::OneDimensional, colorMap)
{
}

}
