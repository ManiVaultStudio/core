#include "ColorMap2DAction.h"

using namespace hdps::util;

namespace hdps::gui {

ColorMap2DAction::ColorMap2DAction(QObject* parent, const QString& title /*= ""*/, const QString& colorMap /*= "example_a"*/) :
    ColorMapAction(parent, "Color Map", ColorMap::Type::TwoDimensional, colorMap)
{
}

}