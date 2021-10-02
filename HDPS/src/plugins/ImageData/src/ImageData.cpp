#include "ImageData.h"
#include "Images.h"
#include "Application.h"

#include <QDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageData")

ImageData::ImageData(const hdps::plugin::PluginFactory* factory) :
    hdps::plugin::RawData(factory, ImageType),
    _type(Type::Undefined),
    _numberOfImages(0),
    _imageSize(),
    _imageRectangle(),
    _numberOfComponentsPerPixel(0),
    _imageFilePaths(),
    _dimensionNames()
{
}

void ImageData::init()
{
}

ImageData::Type ImageData::getType() const
{
    return _type;
}

void ImageData::setType(const ImageData::Type& type)
{
    _type = type;
}

QSize ImageData::getImageSize() const
{
    return _imageSize;
}

void ImageData::setImageSize(const QSize& imageSize)
{
    _imageSize = imageSize;
}

QRect ImageData::getImageRectangle() const
{
    return _imageRectangle;
}

void ImageData::setImageRectangle(const QRect& imageRectangle)
{
    _imageRectangle = imageRectangle;
}

std::uint32_t ImageData::getNumberOfComponentsPerPixel() const
{
    return _numberOfComponentsPerPixel;
}

void ImageData::setNumberOfComponentsPerPixel(const std::uint32_t& numberOfComponentsPerPixel)
{
    _numberOfComponentsPerPixel = numberOfComponentsPerPixel;
}

QStringList ImageData::getImageFilePaths() const
{
    return _imageFilePaths;
}

void ImageData::setImageFilePaths(const QStringList& imageFilePaths)
{
    _imageFilePaths = imageFilePaths;
}

const QStringList& ImageData::getDimensionNames() const
{
    return _dimensionNames;
}

void ImageData::setDimensionNames(const QStringList& dimensionNames)
{
    _dimensionNames = dimensionNames;
}

std::uint32_t ImageData::getNumberOfImages() const
{
    return _numberOfImages;
}

void ImageData::setNumberImages(const std::uint32_t& numberOfImages)
{
    _numberOfImages = numberOfImages;
}

hdps::DataSet* ImageData::createDataSet() const
{
    return new Images(_core, getName());
}

QIcon ImageDataFactory::getIcon() const
{
    return hdps::Application::getIconFont("FontAwesome").getIcon("images");
}

hdps::plugin::RawData* ImageDataFactory::produce()
{
    return new ImageData(this);
}
