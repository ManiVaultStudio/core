#include "ImageData.h"
#include "Images.h"
#include "Application.h"

#include "PointData.h"
#include "DataHierarchyItem.h"

#include "util/Exception.h"

#include <QDebug>

#include <stdexcept>

Q_PLUGIN_METADATA(IID "nl.BioVault.ImageData")

using namespace hdps::util;

ImageData::ImageData(const hdps::plugin::PluginFactory* factory) :
    hdps::plugin::RawData(factory, ImageType),
    _type(Type::Undefined),
    _numberOfImages(0),
    _imageOffset(),
    _sourceRectangle(),
    _targetRectangle(),
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
    return _targetRectangle.size();
}

QRect ImageData::getSourceRectangle() const
{
    return _sourceRectangle;
}

QRect ImageData::getTargetRectangle() const
{
    return _targetRectangle;
}

void ImageData::setImageGeometry(const QSize& sourceImageSize, const QSize& targetImageSize /*= QSize()*/, const QPoint& imageOffset /*= QPoint()*/)
{
    try
    {
        // Except when the target image size exceeds the source image size
        if (targetImageSize.width() > sourceImageSize.width() || targetImageSize.height() > sourceImageSize.height())
            throw std::runtime_error("Target image size may not be larger than source image size");

        // Compute source and target rectangles
        _sourceRectangle = QRect(QPoint(), sourceImageSize);
        _targetRectangle = targetImageSize.isValid() ? QRect(imageOffset, targetImageSize) : _sourceRectangle;

        // Except when the target rectangle exceeds the source rectangle boundaries
        if (!_sourceRectangle.contains(_targetRectangle.topLeft()) || !_sourceRectangle.contains(_targetRectangle.bottomRight()))
            throw std::runtime_error("Target image rectangle exceeds source rectangle boundaries");
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to set image geometry", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to set image geometry");
    }
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
