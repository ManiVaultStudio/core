#include "ImageData.h"
#include "Images.h"

#include <PointData.h>

#include <QtCore>
#include <QDebug>
#include <QImage>

#include <set>

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageData")

ImageData::ImageData(const hdps::plugin::PluginFactory* factory) :
    hdps::plugin::RawData(factory, ImageType),
    _type(Type::Undefined),
    _noImages(0),
    _imageSize(),
    _noComponents(0),
    _pointsName(""),
    _points(nullptr)
{
}

void ImageData::init()
{
    _pointsName = _core->addData("Points", QString("imagepointdata"));
    
    _points = &(dynamic_cast<Points&>(_core->requestData(_pointsName)));

    _core->notifyDataAdded(_pointsName);
}

ImageData::Type ImageData::type() const
{
    return _type;
}

void ImageData::setType(const ImageData::Type& type)
{
    _type = type;
}

QSize ImageData::imageSize() const
{
    return _imageSize;
}

void ImageData::setImageSize(const QSize& imageSize)
{
    _imageSize = imageSize;
}

std::uint32_t ImageData::noComponents() const
{
    return _noComponents;
}

void ImageData::setNoComponents(const std::uint32_t& noComponents)
{
    _noComponents = noComponents;
}

QString ImageData::pointsName() const
{
    return _pointsName;
}

Points* ImageData::points() const
{
    return _points;
}

std::uint32_t ImageData::noPoints() const
{
    if (_points == nullptr)
        return 0;

    return _points->getNumPoints();
}

std::uint32_t ImageData::noDimensions() const
{
    if (_points == nullptr)
        return 0;

    return _points->getNumDimensions();
}

std::vector<QString> ImageData::imageFilePaths() const
{
    return _imageFilePaths;
}

void ImageData::setImageFilePaths(const std::vector<QString>& imageFilePaths)
{
    _imageFilePaths = imageFilePaths;
}

std::vector<QString> ImageData::dimensionNames() const
{
    return _dimensionNames;
}

void ImageData::setDimensionNames(const std::vector<QString>& dimensionNames)
{
    _dimensionNames = dimensionNames;

    _points->setDimensionNames(dimensionNames);
}

std::uint32_t ImageData::noImages() const
{
    return _noImages;
}

void ImageData::setNoImages(const std::uint32_t& noImages)
{
    _noImages = noImages;
}

float ImageData::pointValue(const std::uint32_t& index) const
{
    return _points->getValueAt(index);
}

float ImageData::pointValue(const std::uint32_t& x, const std::uint32_t& y) const
{
    return pointValue(y * _imageSize.width() + x);
}

hdps::DataSet* ImageData::createDataSet() const
{
    return new Images(_core, getName());
}

hdps::plugin::RawData* ImageDataFactory::produce()
{
    return new ImageData(this);
}
