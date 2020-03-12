#include "Images.h"
#include "ImageData.h"

#include <QDebug>
#include <QImage>
#include <QInputDialog>

#include <set>
#include <cmath>

#include "Timer.h"

#include "PointData.h"

#define PROFILE

Images::Images(hdps::CoreInterface* core, QString dataName) :
    DataSet(core, dataName),
    _imageData(nullptr),
    _points(nullptr),
    _roi()
{
    _imageData = &getRawData<ImageData>();
}

void Images::createSubset() const
{
#ifdef PROFILE
    auto timer = Timer("Create subset from selection");
#endif

    const auto defaultName = QString("%1_sub").arg(getName());
    const auto dataSetName = QInputDialog::getText(nullptr, "Select image subset name", "Name", QLineEdit::Normal, defaultName);

    if (dataSetName.isEmpty())
        return;

    auto* images    = new Images(_core, getDataName());
    auto* points    = new Points(_core, _points->getDataName());

    qDebug() << "Create subset for" << getDataName() << _points->getDataName();

    const auto bounds = selectionBounds();

    images->setRoi(bounds);
    images->setPoints(points);

    points->setName(QString("%1_pts").arg(dataSetName));

    for (std::int32_t y = bounds.top(); y <= bounds.bottom(); y++) {
        for (std::int32_t x = bounds.left(); x <= bounds.right(); x++) {
            const auto index = y * _imageData->imageSize().width() + x;

            points->indices.push_back(index);
        }
    }

    _core->createSubsetFromSelection(*points, _points->getDataName(), QString("%1_pts").arg(dataSetName));
    _core->createSubsetFromSelection(*images, getDataName(), QString("%1_img").arg(dataSetName));
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
                        const auto offset = imageId * noElementsPerImage;
                        const auto pointDataId = offset + imagePixelId;
                        const auto pointValue = _imageData->pointValue(pointDataId);

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
                            const auto offset = imageId * noElementsPerImage;
                            const auto pointDataId = offset + imagePixelId * _imageData->noComponents() + c;
                            const auto pointValue = _imageData->pointValue(pointDataId);

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
            const auto imagePixelId = (y * width) + x;
            const auto imageDataX = _roi.left() + x;
            const auto imageDataY = _roi.top() + y;
            const auto imageDataId = (imageDataY * _imageData->imageSize().width()) + imageDataX;
            const auto imageDataIdStart = imagePixelId * Images::noChannelsPerPixel();

            switch (_imageData->noComponents())
            {
                case 1:
                {
                    const auto pointDataId = (imageDataId * _imageData->noDimensions()) + (imageId * _imageData->noComponents());
                    const auto pointValue = static_cast<std::uint16_t>(_imageData->pointValue(pointDataId));

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
                        const auto pointDataId = (imageDataId *  _imageData->noDimensions()) + (imageId *  _imageData->noComponents()) + c;
                        const auto pointValue = static_cast<std::uint16_t>(_imageData->pointValue(pointDataId));

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

QImage Images::selectionImage() const
{
#ifdef PROFILE
    auto timer = Timer("Compute selection image");
#endif

    const auto noElements   = _imageData->noImages() * _imageData->noComponents();
    const auto width        = imageSize().width();
    const auto height       = imageSize().height();
    
    auto imageData = std::vector<std::uint8_t>();

    imageData.resize(noPixels() * 4);

    auto& selection = dynamic_cast<Points&>(_core->requestSelection(_imageData->points()->getDataName()));

    const auto imageDataWidth = _imageData->imageSize().width();

    for (const auto& selectionId : selection.indices)
    {
        const auto x = selectionId % imageDataWidth;
        const auto y = static_cast<std::uint32_t>(floorf(static_cast<float>(selectionId) / static_cast<float>(imageDataWidth)));

        if (_roi.contains(x, y)) {
            const auto pixelId      = ((y - _roi.top()) * width) + (x - _roi.left());
            const auto pixelOffset  = pixelId * 4;

            imageData[pixelOffset + 0] = 255;
            imageData[pixelOffset + 1] = 255;
            imageData[pixelOffset + 2] = 255;
            imageData[pixelOffset + 3] = 255;
        }
    }

    auto image = QImage(width, height, QImage::Format::Format_RGB32);

    memcpy(image.bits(), imageData.data(), imageData.size() * sizeof(std::uint8_t));

    return image;
}

void Images::selectPixels(const std::vector<std::pair<std::uint32_t, std::uint32_t>>& pixelCoordinates, const SelectionModifier& selectionModifier)
{
#ifdef PROFILE
    auto timer = Timer("Select pixels");
#endif

    qDebug() << "Select" << QString::number(pixelCoordinates.size()) << "pixels" << QString("(%1)").arg(selectionModifierName(selectionModifier));

    auto selectedPointIds = std::vector<std::uint32_t>();

    for (const auto& pixelCoordinate : pixelCoordinates) {
        const auto imageDataX = _roi.left() + pixelCoordinate.first;
        const auto imageDataY = (_roi.bottom() - pixelCoordinate.second);
        const auto pixelId = imageDataY * _imageData->imageSize().width() + imageDataX;

        selectedPointIds.push_back(pixelId);
    }

    const auto& currentIndices = indices();

    switch (selectionModifier) {
        case SelectionModifier::Replace:
        {
            setIndices(selectedPointIds);
            break;
        }

        case SelectionModifier::Add:
        {
            auto selectionSet = std::set<std::uint32_t>(currentIndices.begin(), currentIndices.end());

            for (auto& pixelId : selectedPointIds) {
                selectionSet.insert(pixelId);
            }

            setIndices(std::vector<std::uint32_t>(selectionSet.begin(), selectionSet.end()));
            break;
        }

        case SelectionModifier::Remove:
        {
            auto selectionSet = std::set<std::uint32_t>(currentIndices.begin(), currentIndices.end());

            for (auto& pixelId : selectedPointIds) {
                selectionSet.erase(pixelId);
            }

            setIndices(std::vector<std::uint32_t>(selectionSet.begin(), selectionSet.end()));
            break;
        }

        case SelectionModifier::All:
        {
            auto indices = std::vector<std::uint32_t>();

            indices.resize(_roi.width() * _roi.height());

            for (std::int32_t y = 0; y < _roi.height(); y++) {
                for (std::int32_t x = 0; x < _roi.width(); x++) {
                    const auto dataPointId = y * _imageData->imageSize().width() + x;

                    indices.push_back(dataPointId);
                }
            }

            setIndices(indices);
            break;
        }

        case SelectionModifier::None:
        {
            setIndices(std::vector<std::uint32_t>());
            break;
        }

        case SelectionModifier::Invert:
        {
            auto selectionSet = std::set<std::uint32_t>(currentIndices.begin(), currentIndices.end());

            auto indices = std::vector<std::uint32_t>();

            for (std::int32_t y = 0; y < _roi.height(); y++) {
                for (std::int32_t x = 0; x < _roi.width(); x++) {
                    const auto dataPointId = y * _imageData->imageSize().width() + x;

                    const auto selected = selectionSet.find(dataPointId) != selectionSet.end();

                    if (!selected) {
                        indices.push_back(dataPointId);
                    }
                }
            }

            setIndices(indices);
            break;
        }
    }
}

QRect Images::selectionBounds(const bool& relative /*= false*/) const
{
#ifdef PROFILE
    auto timer = Timer("Compute selection bounds");
#endif

    const auto numericMin = std::numeric_limits<std::uint32_t>::min();
    const auto numericMax = std::numeric_limits<std::uint32_t>::max();

    std::uint32_t bounds[4];

    bounds[0] = numericMax;
    bounds[1] = numericMin;
    bounds[2] = numericMax;
    bounds[3] = numericMin;

    auto noSelectedPixels = std::int32_t();

    const auto imageDataWidth = _imageData->imageSize().width();

    auto& selection = dynamic_cast<Points&>(_points->getSelection());

    for (const auto& selectionId : selection.indices)
    {
        const auto x = selectionId % imageDataWidth;
        const auto y = static_cast<std::uint32_t>(floorf(static_cast<float>(selectionId) / static_cast<float>(imageDataWidth)));

        if (_roi.contains(x, y)) {
            noSelectedPixels++;

            if (x < bounds[0])
                bounds[0] = x;

            if (x > bounds[1])
                bounds[1] = x;

            if (y < bounds[2])
                bounds[2] = y;

            if (y > bounds[3])
                bounds[3] = y;
        }
    }

    if (noSelectedPixels == 0)
        return QRect();

    auto topLeft = QPoint(bounds[0], bounds[2]);

    if (relative)
        topLeft -= _roi.topLeft();

    auto rect = QRect(QPoint(bounds[0], bounds[2]), QPoint(bounds[1], bounds[3]));

    rect.translate(-_roi.topLeft());

    return rect;
}

hdps::DataSet* Images::copy() const
{
    Images* images = new Images(_core, getDataName());

    images->setName(getName());

    images->_roi = _roi;
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
    return _roi.size();
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

QRect Images::roi() const
{
    return _roi;
}

void Images::setRoi(const QRect& roi)
{
    _roi = roi;
}

std::uint32_t Images::noPixels() const
{
    return imageSize().width() * imageSize().height();
}

std::uint32_t Images::noSelectedPixels() const
{
#ifdef PROFILE
    auto timer = Timer("Compute the number of selected pixels");
#endif

    auto noSelectedPixels = 0;

    const auto imageDataWidth = _imageData->imageSize().width();

    auto& selection = dynamic_cast<Points&>(_points->getSelection());

    for (const auto& selectionId : selection.indices)
    {
        const auto x = selectionId % imageDataWidth;
        const auto y = static_cast<std::uint32_t>(floorf(static_cast<float>(selectionId) / static_cast<float>(imageDataWidth)));

        if (_roi.contains(x, y)) {
            noSelectedPixels++;
        }
    }

    return noSelectedPixels;
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

std::vector<std::uint32_t>& Images::indices()
{
    return dynamic_cast<Points&>(_points->getSelection()).indices;
}

void Images::setIndices(const std::vector<std::uint32_t>& indices)
{
    auto& selection = dynamic_cast<Points&>(_points->getSelection());

    selection.indices = indices;

    _core->notifySelectionChanged(selection.getDataName());
}
