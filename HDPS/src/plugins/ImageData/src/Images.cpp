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

void Images::setSequence(const std::vector<Image>& images, const QSize& size)
{
    _imageData->setType(ImageData::Type::Sequence);
    _imageData->setNoImages(static_cast<std::uint32_t>(images.size()));
    _imageData->setImageSize(size);
    _imageData->setNoComponents(images.front().noComponents());

    auto imageFilePaths = std::vector<QString>();
    auto dimensionNames = std::vector<QString>();

    imageFilePaths.reserve(images.size());
    dimensionNames.reserve(images.size());

    const auto noDimensions = _imageData->imageSize().width() * _imageData->imageSize().height() * _imageData->noComponents();
    const auto noPoints = images.size();

    std::vector<float> pointsData;

    for (const Image& image : images)
    {
        std::vector<float> imagePixels;

        image.toFloatVector(imagePixels);
        pointsData.insert(pointsData.end(), imagePixels.begin(), imagePixels.end());

        imageFilePaths.push_back(image.imageFilePath());
        dimensionNames.push_back(image.dimensionName());
    }

    _imageData->setImageFilePaths(imageFilePaths);
    _imageData->setDimensionNames(dimensionNames);

    _points->setData(pointsData.data(), static_cast<std::uint32_t>(noPoints), noDimensions);

    _core->notifyDataChanged(_imageData->pointsName());
}

void Images::setStack(const std::vector<Image>& images, const QSize& size)
{
    _imageData->setType(ImageData::Type::Stack);
    _imageData->setNoImages(static_cast<std::uint32_t>(images.size()));
    _imageData->setImageSize(size);
    _imageData->setNoComponents(images.front().noComponents());

    const auto noDimensions = images.size() * _imageData->noComponents();

    std::vector<float> pointsData;

    auto imageFilePaths = std::vector<QString>();
    auto dimensionNames = std::vector<QString>();

    imageFilePaths.reserve(images.size());
    dimensionNames.reserve(images.size());

    for (const Image& image : images)
    {
        imageFilePaths.push_back(image.imageFilePath());
        dimensionNames.push_back(image.dimensionName());
    }
    
    const auto noPoints = _imageData->imageSize().width() * _imageData->imageSize().height();

    pointsData.reserve(noPoints * noDimensions);

    auto pixel = std::vector<std::uint16_t>();

    pixel.resize(_imageData->noComponents());
    
    for (std::int32_t y = 0; y < _imageData->imageSize().height(); y++)
    {
        for (std::int32_t x = 0; x < _imageData->imageSize().width(); x++)
        {
            for (const auto& image : images)
            {
                image.getPixel(x, y, &pixel[0]);

                for (std::uint32_t c = 0; c < _imageData->noComponents(); c++)
                {
                    pointsData.push_back(static_cast<float>(pixel[c]));
                }
            }
        }
    }

    _imageData->setImageFilePaths(imageFilePaths);
    _imageData->setDimensionNames(dimensionNames);

    _points->setData(pointsData.data(), static_cast<std::uint32_t>(noPoints), static_cast<std::uint32_t>(noDimensions));
    
    _core->notifyDataChanged(_imageData->pointsName());
}

QImage Images::sequenceImage(const std::vector<std::uint32_t>& imageIds)
{
#ifdef PROFILE
    auto timer = Timer("Compute sequence image");
#endif
    
    const auto noImages             = static_cast<float>(imageIds.size());
    const auto noElementsPerImage   = noPixels() * _imageData->noComponents();
    const auto width                = imageSize().width();
    const auto height               = imageSize().height();

    auto imageData = std::vector<std::uint16_t>();

    imageData.resize(noPixels() * Images::noChannelsPerPixel());

    for (std::int32_t y = 0; y < height; y++)
    {
        for (std::int32_t x = 0; x < width; x++)
        {
            const auto imagePixelId = y * width + x;
            const auto imageDataIdStart = imagePixelId * Images::noChannelsPerPixel();

            switch (_imageData->noComponents())
            {
                case 1:
                {
                    auto sum = 0.f;

                    for (const auto& imageId : imageIds)
                    {
                        const auto offset       = imageId * noElementsPerImage;
                        const auto pointDataId  = offset + imagePixelId;
                        const auto pointValue   = _imageData->pointValue(pointDataId);

                        sum += pointValue;
                    }

                    const auto average = sum / static_cast<float>(noImages);

                    for (std::uint32_t c = 0; c < 3; c++)
                    {
                        imageData[imageDataIdStart + c] = average;
                    }

                    break;
                }

                case 3:
                {
                    for (std::uint32_t c = 0; c < _imageData->noComponents(); c++)
                    {
                        auto sum = 0.f;

                        for (const auto& imageId : imageIds)
                        {
                            const auto offset       = imageId * noElementsPerImage;
                            const auto pointDataId  = offset + imagePixelId * _imageData->noComponents() + c;
                            const auto pointValue   = _imageData->pointValue(pointDataId);

                            sum += pointValue;
                        }

                        imageData[imageDataIdStart + c] = sum / static_cast<float>(noImages);
                    }

                    break;
                }
            }

            imageData[imageDataIdStart + (Images::noChannelsPerPixel() - 1)] = std::numeric_limits<std::uint16_t>::max();
        }
    }

    auto image = QImage(width, height, QImage::Format::Format_RGBA64);

    memcpy(image.bits(), imageData.data(), imageData.size() * sizeof(std::uint16_t));

    return image;
}

QImage Images::stackImage(const std::uint32_t& imageId)
{
#ifdef PROFILE
    auto timer = Timer("Compute stack image");
#endif

    const auto width    = imageSize().width();
    const auto height   = imageSize().height();

    auto imageData = std::vector<std::uint16_t>();

    imageData.resize(noPixels() *  Images::noChannelsPerPixel());

    for (std::int32_t y = 0; y < height; y++)
    {
        for (std::int32_t x = 0; x < width; x++)
        {
            const auto imagePixelId     = (y * width) + x;
            const auto imageDataX       = x;
            const auto imageDataY       = y;
            const auto imageDataId      = (imageDataY * _imageData->imageSize().width()) + imageDataX;
            const auto imageDataIdStart = imagePixelId * Images::noChannelsPerPixel();

            switch (_imageData->noComponents())
            {
                case 1:
                {
                    const auto pointDataId  = (imageDataId * _imageData->noDimensions()) + (imageId * _imageData->noComponents());
                    const auto pointValue   = static_cast<std::uint16_t>(_imageData->pointValue(pointDataId));

                    for (std::uint32_t c = 0; c < 3; c++)
                    {
                        imageData[imageDataIdStart + c] = pointValue;
                    }

                    break;
                }

                case 3:
                {
                    for (std::uint32_t c = 0; c < _imageData->noComponents(); c++)
                    {
                        const auto pointDataId  = (imageDataId *  _imageData->noDimensions()) + (imageId *  _imageData->noComponents()) + c;
                        const auto pointValue   = static_cast<std::uint16_t>(_imageData->pointValue(pointDataId));

                        imageData[imageDataIdStart + c] = pointValue;
                    }

                    break;
                }

                default:
                    break;
            }

            imageData[imageDataIdStart + (Images::noChannelsPerPixel() - 1)] = std::numeric_limits<std::uint16_t>::max();
        }
    }

    auto image = QImage(width, height, QImage::Format::Format_RGBA64);

    memcpy(image.bits(), imageData.data(), imageData.size() * sizeof(std::uint16_t));

    return image;
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