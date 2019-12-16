#include "ImageData.h"
#include "Images.h"

#include <PointData.h>

#include <QtCore>
#include <QDebug>
#include <QImage>

#include <set>

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageData")

ImageData::ImageData() :
    hdps::RawData("Image Data", ImageType),
    _imageCollectionType(ImageCollectionType::Undefined),
    _noImages(0),
    _imageSize(),
    _noComponents(0),
    _pointsName(""),
    _points(nullptr)
{
}

ImageData::~ImageData()
{
}

void ImageData::init()
{
    _pointsName = _core->addData("Points", QString("imagepointdata"));
    
    _points = &(dynamic_cast<Points&>(_core->requestData(_pointsName)));

    _core->notifyDataAdded(_pointsName);
}

ImageCollectionType ImageData::imageCollectionType() const
{
    return _imageCollectionType;
}

void ImageData::setImageCollectionType(const ImageCollectionType& imageCollectionType)
{
    _imageCollectionType = imageCollectionType;
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

/*
void ImageData::setSequence(const std::vector<Image>& images, const QSize& size)
{
    _imageCollectionType    = ImageCollectionType::Sequence;
    _noImages               = static_cast<std::uint32_t>(images.size());
    _imageSize              = size;
    _noComponents           = images.front().noComponents();
    
    _imageFilePaths.clear();
    _dimensionNames.clear();

    _imageFilePaths.reserve(images.size());
    _dimensionNames.reserve(images.size());

    const auto noDimensions = _imageSize.width() * _imageSize.height() * _noComponents;
    const auto noPoints     = images.size();

    std::vector<float> pixels;

    for (const Image& image : images)
    {
        std::vector<float> imagePixels;

        image.toFloatVector(imagePixels);
        pixels.insert(pixels.end(), imagePixels.begin(), imagePixels.end());

        _imageFilePaths.push_back(image.imageFilePath());
        _dimensionNames.push_back(image.dimensionName());
    }

    _points->setData(pixels.data(), static_cast<std::uint32_t>(noPoints), noDimensions);
    _points->setDimensionNames(_dimensionNames);
    _core->notifyDataChanged(_pointsName);
}

void ImageData::setStack(const std::vector<Image>& images, const QSize& size)
{
    _imageCollectionType    = ImageCollectionType::Stack;
    _noImages               = static_cast<std::uint32_t>(images.size());
    _imageSize              = size;
    _noComponents           = images.front().noComponents();

    const auto noDimensions = images.size() * _noComponents;
    
    std::vector<float> dataPoints;

    _imageFilePaths.clear();
    _dimensionNames.clear();

    _imageFilePaths.reserve(noDimensions);
    _dimensionNames.reserve(noDimensions);

    for (const Image& image : images)
    {
        _imageFilePaths.push_back(image.imageFilePath());
        _dimensionNames.push_back(image.dimensionName());
    }

    const auto noPoints = _imageSize.width() * _imageSize.height();

    dataPoints.reserve(noPoints * noDimensions);

    auto pixel = std::vector<std::uint16_t>();

    pixel.resize(_noComponents);
    
    for (std::int32_t y = 0; y < _imageSize.height(); y++)
    {
        for (std::int32_t x = 0; x < _imageSize.width(); x++)
        {
            for (const auto& image : images)
            {
                image.getPixel(x, y, &pixel[0]);

                for (std::uint32_t c = 0; c < _noComponents; c++)
                {
                    dataPoints.push_back(static_cast<float>(pixel[c]));
                }
            }
        }
    }

    _points->setData(dataPoints.data(), static_cast<std::uint32_t>(noPoints), static_cast<std::uint32_t>(noDimensions));
    _points->setDimensionNames(_dimensionNames);
    _core->notifyDataChanged(_pointsName);
}
*/

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
    return (*_points)[index];
}

float ImageData::pointValue(const std::uint32_t& x, const std::uint32_t& y) const
{
    return pointValue(y * _imageSize.width() + x);
}

hdps::DataSet* ImageData::createDataSet() const
{
    return new Images(_core, getName());
}

hdps::RawData* ImageDataFactory::produce()
{
    return new ImageData();
}