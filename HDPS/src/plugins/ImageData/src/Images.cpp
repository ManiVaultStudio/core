#include "Images.h"
#include "ImageData.h"

#include <QDebug>
#include <QImage>
#include <QInputDialog>

#include <set>
#include <cmath>

#include "util/Timer.h"

#include "PointData.h"

#define PROFILE

Images::Images(hdps::CoreInterface* core, QString dataName) :
    DataSet(core, dataName),
    _imageData(nullptr),
    _points(nullptr)
{
    _imageData = &getRawData<ImageData>();
}

hdps::DataSet* Images::copy() const
{
    Images* images = new Images(_core, getDataName());

    images->setName(getName());

    images->_points = _points;

    return images;
}

ImageData::Type Images::type() const
{
    return _imageData->type();
}

std::uint32_t Images::noImages() const
{
    return _imageData->noImages();
}

QSize Images::imageSize() const
{
    return _imageData->imageSize();
}

std::uint32_t Images::noComponents() const
{
    return _imageData->noComponents();
}

std::vector<QString> Images::imageFilePaths() const
{
    return _imageData->imageFilePaths();
}

std::vector<QString> Images::dimensionNames() const
{
    return _imageData->dimensionNames();
}

std::uint32_t Images::noPixels() const
{
    return imageSize().width() * imageSize().height();
}

std::uint32_t Images::noChannelsPerPixel()
{
    return 4;
}

QString Images::pointsName()
{
    return _imageData->pointsName();
}

Points* Images::points()
{
    return _points;
}

void Images::setPoints(Points* points)
{
    _points = points;
}

QIcon Images::getIcon() const
{
    return QIcon();
}
