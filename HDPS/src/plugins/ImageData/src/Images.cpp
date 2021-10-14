#include "Images.h"
#include "ImageData.h"
#include "InfoAction.h"

#include "util/Exception.h"

#include "DataHierarchyItem.h"

#include "PointData.h"
#include "ClusterData.h"

#include <QDebug>

using namespace hdps::util;

Images::Images(hdps::CoreInterface* core, QString dataName) :
    DataSet(core, dataName),
    _imageData(nullptr)
{
    _imageData = &getRawData<ImageData>();
}

void Images::init()
{
    _infoAction = QSharedPointer<InfoAction>::create(nullptr, getName());

    addAction(*_infoAction.get());
}

QString Images::createSubset(const QString subsetName /*= "subset"*/, const QString parentSetName /*= ""*/, const bool& visible /*= true*/) const
{
    return "";
}

hdps::DataSet* Images::copy() const
{
    Images* images = new Images(_core, getDataName());

    images->setName(getName());

    return images;
}

ImageData::Type Images::getType() const
{
    return _imageData->getType();
}

void Images::setType(const ImageData::Type& type)
{
    _imageData->setType(type);
}

std::uint32_t Images::getNumberOfImages() const
{
    return _imageData->getNumberOfImages();
}

void Images::setNumberOfImages(const std::uint32_t& numberOfImages)
{
    _imageData->setNumberImages(numberOfImages);
}

QSize Images::getImageSize() const
{
    return _imageData->getImageSize();
}

QRect Images::getSourceRectangle() const
{
    return _imageData->getSourceRectangle();
}

QRect Images::getTargetRectangle() const
{
    return _imageData->getTargetRectangle();
}

void Images::setImageGeometry(const QSize& sourceImageSize, const QSize& targetImageSize /*= QSize()*/, const QPoint& imageOffset /*= QPoint()*/)
{
    _imageData->setImageGeometry(sourceImageSize, targetImageSize, imageOffset);
}

std::uint32_t Images::getNumberOfComponentsPerPixel() const
{
    return _imageData->getNumberOfComponentsPerPixel();
}

void Images::setNumberOfComponentsPerPixel(const std::uint32_t& numberOfComponentsPerPixel)
{
    _imageData->setNumberOfComponentsPerPixel(numberOfComponentsPerPixel);
}

QStringList Images::getImageFilePaths() const
{
    return _imageData->getImageFilePaths();
}

void Images::setImageFilePaths(const QStringList& imageFilePaths)
{
    _imageData->setImageFilePaths(imageFilePaths);
}

std::uint32_t Images::getNumberOfPixels() const
{
    return getImageSize().width() * getImageSize().height();
}

std::uint32_t Images::noChannelsPerPixel()
{
    return 4;
}

QIcon Images::getIcon() const
{
    return hdps::Application::getIconFont("FontAwesome").getIcon("images");
}

void Images::getScalarData(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange, const std::uint32_t& subsample /*= 1*/)
{
    try
    {
        if (static_cast<std::uint32_t>(scalarData.count()) < getNumberOfPixels())
            throw std::runtime_error("Scalar data vector number of elements is smaller than the number of pixels");

        if (subsample < 1)
            throw std::runtime_error("Subsample amount may not be zero");

        switch (_imageData->getType())
        {
            case ImageData::Undefined:
                break;

            case ImageData::Sequence:
                getScalarDataForImageSequence(dimensionIndex, scalarData, scalarDataRange, subsample);
                break;

            case ImageData::Stack:
                getScalarDataForImageStack(dimensionIndex, scalarData, scalarDataRange, subsample);
                break;

            case ImageData::MultiPartSequence:
                break;

            default:
                break;
        }

        // Initialize scalar data range
        scalarDataRange = { std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest() };

        // Compute the actual scalar data range
        for (auto& scalar : scalarData) {
            scalarDataRange.first = std::min(scalar, scalarDataRange.first);
            scalarDataRange.second = std::max(scalar, scalarDataRange.second);
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get scalar data", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get scalar data");
    }
}

void Images::getSelectionData(std::vector<std::uint8_t>& selectionImageData, std::vector<std::uint32_t>& selectedIndices, QRect& selectionBoundaries)
{
    try
    {
        // Get reference to input dataset
        auto& dataset = _core->getDataHierarchyItem(getName())->getParent()->getDataset<DataSet>();

        if (dataset.getDataType() == PointType) {

            // Obtain reference to the point source input dataset
            auto& sourcePoints = DataSet::getSourceData(dynamic_cast<Points&>(dataset));

            // Get selection indices from points dataset
            auto& globalSelectionIndices = dynamic_cast<Points&>(sourcePoints.getSelection()).indices;

            // Clear the selected indices
            selectedIndices.clear();
            selectedIndices.reserve(getNumberOfPixels());

            const auto sourceImageWidth = getSourceRectangle().width();
            const auto targetImageWidth = getTargetRectangle().width();

            // Fill selection data with non-selected
            std::fill(selectionImageData.begin(), selectionImageData.end(), 0);

            // Initialize selection boundaries with numeric extremes
            selectionBoundaries.setTop(std::numeric_limits<int>::max());
            selectionBoundaries.setBottom(std::numeric_limits<int>::lowest());
            selectionBoundaries.setLeft(std::numeric_limits<int>::max());
            selectionBoundaries.setRight(std::numeric_limits<int>::lowest());

            // Iterate over all global selection indices
            for (const auto& globalSelectionIndex : globalSelectionIndices) {

                // Compute global pixel coordinate
                const auto globalPixelCoordinate = QPoint(globalSelectionIndex % sourceImageWidth, static_cast<std::int32_t>(floorf(globalSelectionIndex / static_cast<float>(sourceImageWidth))));

                // Only proceed if the pixel is located within the source image rectangle
                if (!getTargetRectangle().contains(globalPixelCoordinate))
                    continue;

                // Compute local pixel coordinate and index
                const auto localPixelCoordinate = globalPixelCoordinate - getTargetRectangle().topLeft();
                const auto localPixelIndex = localPixelCoordinate.y() * targetImageWidth + localPixelCoordinate.x();

                // And add the target pixel index to the list of selected pixels
                if (static_cast<std::uint32_t>(localPixelIndex) < getNumberOfPixels())
                    selectedIndices.push_back(localPixelIndex);

                // Assign selected pixel
                selectionImageData[localPixelIndex] = 255;

                // Add pixel pixel coordinate and possibly inflate the selection boundaries
                selectionBoundaries.setLeft(std::min(selectionBoundaries.left(), localPixelCoordinate.x()));
                selectionBoundaries.setRight(std::max(selectionBoundaries.right(), localPixelCoordinate.x()));
                selectionBoundaries.setTop(std::min(selectionBoundaries.top(), localPixelCoordinate.y()));
                selectionBoundaries.setBottom(std::max(selectionBoundaries.bottom(), localPixelCoordinate.y()));
            }
        }

        if (dataset.getDataType() == ClusterType) {
        }

        //if (static_cast<std::uint32_t>(scalarData.count()) < getNumberOfPixels())
        //    throw std::runtime_error("Scalar data vector number of elements is smaller than the number of pixels");

        //if (subsample < 1)
        //    throw std::runtime_error("Subsample amount may not be zero");

        //switch (_imageData->getType())
        //{
        //case ImageData::Undefined:
        //    break;

        //case ImageData::Sequence:
        //    getScalarDataForImageSequence(dimensionIndex, scalarData, scalarDataRange, subsample);
        //    break;

        //case ImageData::Stack:
        //    getScalarDataForImageStack(dimensionIndex, scalarData, scalarDataRange, subsample);
        //    break;

        //case ImageData::MultiPartSequence:
        //    break;

        //default:
        //    break;
        //}

        //// Initialize scalar data range
        //scalarDataRange = { std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest() };

        //// Compute the actual scalar data range
        //for (auto& scalar : scalarData) {
        //    scalarDataRange.first = std::min(scalar, scalarDataRange.first);
        //    scalarDataRange.second = std::max(scalar, scalarDataRange.second);
        //}
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get scalar data", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get scalar data");
    }

    
}

void Images::getScalarDataForImageSequence(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange, const std::uint32_t& subsample /*= 1*/)
{
    QSize sourceImageSize = getImageSize(), targetImageSize;

    // Establish target image size based on sub-sampling
    targetImageSize.setWidth(static_cast<int>(floorf(sourceImageSize.width() / subsample)));
    targetImageSize.setHeight(static_cast<int>(floorf(sourceImageSize.height() / subsample)));

    // Get reference to input points dataset
    auto& points = _core->getDataHierarchyItem(getName())->getParent()->getDataset<Points>();

    points.visitData([this, points, dimensionIndex, &scalarData, subsample, sourceImageSize, targetImageSize](auto pointData) {
        const auto dimensionId      = dimensionIndex;
        const auto imageSize        = _imageData->getImageSize();
        const auto noPixels         = getNumberOfPixels();
        const auto selection        = dynamic_cast<Points&>(points.getSelection());
        const auto selectionIndices = selection.indices;
        const auto selectionSize    = selectionIndices.size();

        if (!selectionIndices.empty()) {
            for (std::uint32_t p = 0; p < noPixels; p++) {
                auto sum = 0.0f;

                for (auto selectionIndex : selectionIndices)
                    sum += pointData[selectionIndex][p];

                scalarData[p] = static_cast<float>(sum / selectionSize);
            }
        }
        else {

            // Populate scalar data vector with pixel data
            for (std::int32_t pixelX = 0; pixelX < targetImageSize.width(); pixelX++) {
                for (std::int32_t pixelY = 0; pixelY < targetImageSize.height(); pixelY++) {

                    // Compute the source and target pixel index
                    const auto sourcePixelIndex = (pixelY * subsample) * sourceImageSize.width() + (pixelX * subsample);
                    const auto targetPixelIndex = pixelY * targetImageSize.width() + pixelX;

                    // And assign the scalar data
                    scalarData[targetPixelIndex] = pointData[dimensionIndex][sourcePixelIndex];
                }
            }
        }
    });
}

void Images::getScalarDataForImageStack(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange, const std::uint32_t& subsample /*= 1*/)
{
    // Get reference to input points dataset
    auto& points = _core->getDataHierarchyItem(getName())->getParent()->getDataset<Points>();

    // Treat derived and non-derived differently
    if (points.isDerivedData()) {

        // Visit derived points dataset
        points.visitData([this, &points, dimensionIndex, &scalarData](auto pointData) {

            // Get reference to source data
            auto& sourceData = points.getSourceData<Points>(points);

            if (sourceData.isFull()) {

                // Populate from full dataset
                for (std::uint32_t pixelIndex = 0; pixelIndex < points.getNumPoints(); pixelIndex++)
                    if (pixelIndex < scalarData.count() && pixelIndex < pointData.size())
                        scalarData[pixelIndex] = pointData[pixelIndex][dimensionIndex];
            }
            else {

                // Populate from partial dataset
                for (std::int32_t pixelIndex = 0; pixelIndex < sourceData.indices.size(); pixelIndex++)
                    if (pixelIndex < scalarData.count() && pixelIndex < pointData.size())
                        scalarData[pixelIndex] = pointData[pixelIndex][dimensionIndex];
            }
        });
    }
    else {

        points.visitSourceData([this, dimensionIndex, &scalarData](auto pointData) {

            // Cache the target rectangle
            const auto targetRectangle = getTargetRectangle();

            // Populate scalar data vector with pixel data
            for (std::int32_t pixelX = targetRectangle.left(); pixelX <= targetRectangle.right(); pixelX++) {
                for (std::int32_t pixelY = targetRectangle.top(); pixelY <= targetRectangle.bottom(); pixelY++) {

                    // Establish pixel coordinate
                    const QPoint pixelCoordinate(pixelX, pixelY);

                    // Compute the target pixel index
                    const auto targetPixelIndex = getTargetPixelIndex(pixelCoordinate);
                    
                    // Assign the scalar data
                    scalarData[targetPixelIndex] = pointData[targetPixelIndex][dimensionIndex];
                }
            }
        });
    }
}

std::uint32_t Images::getTargetPixelIndex(const QPoint& coordinate) const
{
    const auto targetRectangle      = _imageData->getTargetRectangle();
    const auto relativeCoordinate   = coordinate - targetRectangle.topLeft();

    return relativeCoordinate.y() * targetRectangle.width() + relativeCoordinate.x();
}

std::uint32_t Images::getSourceDataIndex(const QPoint& coordinate) const
{
    // Get reference to input points dataset
    auto& points = _core->getDataHierarchyItem(getName())->getParent()->getDataset<Points>();

    // Get reference to source data
    auto& sourceData = points.getSourceData<Points>(points);

    const auto targetRectangle      = _imageData->getTargetRectangle();
    const auto relativeCoordinate   = coordinate - targetRectangle.topLeft();

    return relativeCoordinate.y() * targetRectangle.width() + relativeCoordinate.x();
    //return (targetRectangle.height() - relativeCoordinate.y()) * targetRectangle.width() + relativeCoordinate.x();
}
