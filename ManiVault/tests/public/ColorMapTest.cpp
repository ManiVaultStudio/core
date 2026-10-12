#include "util/ColorMap.h"

#include <QDebug>

namespace {
bool require(bool condition, const char* message)
{
    if (!condition)
        qCritical().noquote() << message;
    return condition;
}
}

bool preservesDefaultColorMapState()
{
    const mv::util::ColorMap colorMap;
    return require(colorMap.getName().isEmpty(), "default color map name was not empty")
        && require(colorMap.getResourcePath().isEmpty(), "default color map resource path was not empty")
        && require(colorMap.getType() == mv::util::ColorMap::Type::OneDimensional, "default color map type was incorrect")
        && require(colorMap.getImage().isNull(), "default color map image was not null")
        && require(colorMap.getNoDimensions() == 1, "default color map dimension count was incorrect");
}

bool preservesConfiguredColorMapState()
{
    QImage image(QSize(4, 2), QImage::Format_RGBA8888);
    image.fill(Qt::blue);
    const mv::util::ColorMap colorMap("Test map", ":/maps/test.png", mv::util::ColorMap::Type::TwoDimensional, image);

    return require(colorMap.getName() == "Test map", "color map name was not preserved")
        && require(colorMap.getResourcePath() == ":/maps/test.png", "color map resource path was not preserved")
        && require(colorMap.getType() == mv::util::ColorMap::Type::TwoDimensional, "color map type was not preserved")
        && require(colorMap.getImage() == image, "color map image was not preserved")
        && require(colorMap.getNoDimensions() == 2, "two-dimensional color map dimension count was incorrect");
}

int main()
{
    return preservesDefaultColorMapState()
        && preservesConfiguredColorMapState() ? 0 : 1;
}
