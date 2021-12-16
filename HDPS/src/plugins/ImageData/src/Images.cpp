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
    DatasetImpl(core, dataName),
    _indices(),
    _imageData(nullptr),
    _infoAction(),
    _visibleRectangle(),
    _globalIndices()
{
    _imageData = &getRawData<ImageData>();
}

void Images::init()
{
    _infoAction = QSharedPointer<InfoAction>::create(nullptr, *this);

    addAction(*_infoAction.get());
}

Dataset<DatasetImpl> Images::createSubset(const QString& guiName, const Dataset<DatasetImpl>& parentDataSet /*= Dataset<DatasetImpl>()*/, const bool& visible /*= true*/) const
{
    return _core->createSubsetFromSelection(getSelection(), toSmartPointer(), guiName, parentDataSet, visible);
}

Dataset<DatasetImpl> Images::copy() const
{
    auto images = new Images(_core, getRawDataName());

    images->setGuiName(getGuiName());

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

void Images::setImageSize(const QSize& imageSize)
{
    _imageData->setImageSize(imageSize);
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

QRect Images::getRectangle() const
{
    return QRect(QPoint(), getImageSize());
}

QRect Images::getVisibleRectangle()
{
    if (_visibleRectangle.isValid())
        return _visibleRectangle;

    // Obtain reference to the point source input dataset
    auto points = getParent()->getSourceDataset<Points>();

    const float imageWidth = getImageSize().width();

    // Initialize selection boundaries with numeric extremes
    _visibleRectangle.setTop(std::numeric_limits<int>::max());
    _visibleRectangle.setBottom(std::numeric_limits<int>::lowest());
    _visibleRectangle.setLeft(std::numeric_limits<int>::max());
    _visibleRectangle.setRight(std::numeric_limits<int>::lowest());

    // Get global indices into data
    auto globalIndices = getGlobalIndices();

    for (const auto& globalIndex : globalIndices) {

        // Compute global pixel coordinate
        const auto globalPixelCoordinate = QPoint(globalIndex % getImageSize().width(), static_cast<std::int32_t>(floorf(globalIndex / static_cast<float>(getImageSize().width()))));

        // Add pixel pixel coordinate and possibly inflate the visible rectangle
        _visibleRectangle.setLeft(std::min(_visibleRectangle.left(), globalPixelCoordinate.x()));
        _visibleRectangle.setRight(std::max(_visibleRectangle.right(), globalPixelCoordinate.x()));
        _visibleRectangle.setTop(std::min(_visibleRectangle.top(), globalPixelCoordinate.y()));
        _visibleRectangle.setBottom(std::max(_visibleRectangle.bottom(), globalPixelCoordinate.y()));
    }

    return _visibleRectangle;
}

QIcon Images::getIcon() const
{
    return hdps::Application::getIconFont("FontAwesome").getIcon("images");
}

std::vector<std::uint32_t>& Images::getSelectionIndices()
{
    return _indices;
}

void Images::setSelectionIndices(const std::vector<std::uint32_t>& indices)
{
}

bool Images::canSelect() const
{
    return false;
}

bool Images::canSelectAll() const
{
    return false;
}

bool Images::canSelectNone() const
{
    return false;
}

bool Images::canSelectInvert() const
{
    return false;
}

void Images::selectAll()
{
}

void Images::selectNone()
{
}

void Images::selectInvert()
{
}

void Images::getScalarData(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange)
{
    try
    {
        if (static_cast<std::uint32_t>(scalarData.count()) < getNumberOfPixels())
            throw std::runtime_error("Scalar data vector number of elements is smaller than the number of pixels");

        switch (_imageData->getType())
        {
            case ImageData::Undefined:
                break;

            case ImageData::Sequence:
                getScalarDataForImageSequence(dimensionIndex, scalarData, scalarDataRange);
                break;

            case ImageData::Stack:
                getScalarDataForImageStack(dimensionIndex, scalarData, scalarDataRange);
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

void Images::getMaskData(std::vector<std::uint8_t>& maskData)
{
    // Get reference to input dataset
    auto inputDataset = getParent();

    if (inputDataset->getDataType() == PointType) {

        // All masked by default
        std::fill(maskData.begin(), maskData.end(), 0);

        // Obtain reference to the points dataset
        auto points = Dataset<Points>(inputDataset);

        // Get global indices into data
        auto& globalIndices = getGlobalIndices();

        for (const auto& globalIndex : globalIndices)
            maskData[globalIndex] = 255;
    }

    if (inputDataset->getDataType() == ClusterType) {
        std::fill(maskData.begin(), maskData.end(), 255);
    }
}

void Images::getSelectionData(std::vector<std::uint8_t>& selectionImageData, std::vector<std::uint32_t>& selectedIndices, QRect& selectionBoundaries)
{
    try
    {
        // Get smart pointer to parent dataset
        auto parentDataset = getDataHierarchyItem().getParent().getDataset<DatasetImpl>();

        if (parentDataset->getDataType() == PointType) {
            
            // Obtain reference to the point source input dataset
            auto points = Dataset<Points>(parentDataset->getParent());

            // Get selection indices from points dataset
            auto& selectionIndices = points->getSelection<Points>()->indices;

            // Clear the selected indices
            selectedIndices.clear();
            selectedIndices.reserve(getNumberOfPixels());

            const auto imageWidth = getImageSize().width();

            // Fill selection data with non-selected
            std::fill(selectionImageData.begin(), selectionImageData.end(), 0);

            // Initialize selection boundaries with numeric extremes
            selectionBoundaries.setTop(std::numeric_limits<int>::max());
            selectionBoundaries.setBottom(std::numeric_limits<int>::lowest());
            selectionBoundaries.setLeft(std::numeric_limits<int>::max());
            selectionBoundaries.setRight(std::numeric_limits<int>::lowest());

            // Get global indices into data
            auto& globalIndices = getGlobalIndices();

            // The global indices to include for selection boundaries computation
            std::vector<std::uint32_t> intersectionGlobalIndices;

            // Find the intersection between the selection indices and the global indices
            std::set_intersection(selectionIndices.begin(), selectionIndices.end(), globalIndices.begin(), globalIndices.end(), std::back_inserter(intersectionGlobalIndices));

            // Iterate over selection indices and modify the selection boundaries when not masked
            for (const auto& selectionIndex : selectionIndices) {

                // Selected item is present in the (sub)set, so add it
                selectedIndices.push_back(selectionIndex);

                // Assign selected pixel
                selectionImageData[selectionIndex] = 255;

                // Compute global pixel coordinate
                const auto globalPixelCoordinate = QPoint(selectionIndex % imageWidth, static_cast<std::int32_t>(floorf(selectionIndex / static_cast<float>(imageWidth))));

                // Add pixel pixel coordinate and possibly inflate the selection boundaries
                selectionBoundaries.setLeft(std::min(selectionBoundaries.left(), globalPixelCoordinate.x()));
                selectionBoundaries.setRight(std::max(selectionBoundaries.right(), globalPixelCoordinate.x()));
                selectionBoundaries.setTop(std::min(selectionBoundaries.top(), globalPixelCoordinate.y()));
                selectionBoundaries.setBottom(std::max(selectionBoundaries.bottom(), globalPixelCoordinate.y()));
            }

            selectionBoundaries = selectionBoundaries.marginsAdded(QMargins(0, 0, 1, 1));
        }

        if (parentDataset->getDataType() == ClusterType) {

            // Obtain reference to the cluster source input dataset
            auto sourceClusters = parentDataset->getSelection<Clusters>();

            // Clear the selected indices
            selectedIndices.clear();
            selectedIndices.reserve(getNumberOfPixels());

            const auto sourceImageWidth = getImageSize().width();
            const auto targetImageWidth = getImageSize().width();

            // Fill selection data with non-selected
            std::fill(selectionImageData.begin(), selectionImageData.end(), 0);

            // Initialize selection boundaries with numeric extremes
            selectionBoundaries.setTop(std::numeric_limits<int>::max());
            selectionBoundaries.setBottom(std::numeric_limits<int>::lowest());
            selectionBoundaries.setLeft(std::numeric_limits<int>::max());
            selectionBoundaries.setRight(std::numeric_limits<int>::lowest());

            // Get clusters input points dataset
            auto points = parentDataset->getParent()->getSourceDataset<Points>();

            // Iterate over all clusters and populate the selection data
            for (const auto& clusterIndex : sourceClusters->indices) {
                
                // Get reference to cluster
                const auto& cluster = sourceClusters->getClusters()[clusterIndex];
                
                // Iterate over all indices in the cluster
                for (const auto& clusterSelectionIndex : cluster.getIndices()) {

                    // Get translated point index
                    const auto pointIndex = points->isFull() ? clusterSelectionIndex : points->indices[clusterSelectionIndex];

                    // Compute global pixel coordinate
                    const auto globalPixelCoordinate = QPoint(pointIndex % sourceImageWidth, static_cast<std::int32_t>(floorf(static_cast<float>(pointIndex) / static_cast<float>(sourceImageWidth))));

                    // Compute local pixel index
                    const auto globalPixelIndex = globalPixelCoordinate.y() * targetImageWidth + globalPixelCoordinate.x();

                    // And add the target pixel index to the list of selected pixels
                    if (static_cast<std::uint32_t>(globalPixelIndex) < getNumberOfPixels())
                        selectedIndices.push_back(globalPixelIndex);

                    // Assign selected pixel
                    selectionImageData[globalPixelIndex] = 255;

                    // Add pixel pixel coordinate and possibly inflate the selection boundaries
                    selectionBoundaries.setLeft(std::min(selectionBoundaries.left(), globalPixelCoordinate.x()));
                    selectionBoundaries.setRight(std::max(selectionBoundaries.right(), globalPixelCoordinate.x()));
                    selectionBoundaries.setTop(std::min(selectionBoundaries.top(), globalPixelCoordinate.y()));
                    selectionBoundaries.setBottom(std::max(selectionBoundaries.bottom(), globalPixelCoordinate.y()));
                }
            }
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get image selection data", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get image selection data");
    }
}

void Images::getScalarDataForImageSequence(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange)
{
    // Get smart pointer to parent dataset
    auto parentDataset = getDataHierarchyItem().getParent().getDataset<DatasetImpl>();

    if (parentDataset->getDataType() == PointType) {

        // Obtain reference to the points source input dataset
        auto points = parentDataset->getSourceDataset<Points>();

        QSize sourceImageSize = getImageSize(), targetImageSize;

        // Establish target image size based on sub-sampling
        targetImageSize.setWidth(static_cast<int>(floorf(sourceImageSize.width())));
        targetImageSize.setHeight(static_cast<int>(floorf(sourceImageSize.height())));

        points->visitData([this, points, dimensionIndex, &scalarData, sourceImageSize, targetImageSize](auto pointData) {
            const auto dimensionId      = dimensionIndex;
            const auto imageSize        = _imageData->getImageSize();
            const auto noPixels         = getNumberOfPixels();
            const auto selection        = points->getSelection<Points>();
            const auto selectionIndices = selection->indices;
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
                        const auto targetPixelIndex = pixelY * targetImageSize.width() + pixelX;

                        // And assign the scalar data
                        scalarData[targetPixelIndex] = pointData[dimensionIndex][targetPixelIndex];
                    }
                }
            }
        });
    }

    if (parentDataset->getDataType() == ClusterType) {
    }
}

void Images::getScalarDataForImageStack(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange)
{
    // Get reference to input dataset
    auto dataset = getParent();

    if (dataset->getDataType() == PointType) {

        // Obtain reference to the points dataset
        auto points = Dataset<Points>(dataset);

        // Get global indices into data
        auto& globalIndices = getGlobalIndices();

        // Treat derived and non-derived differently
        if (points->isDerivedData()) {

            // Visit derived points dataset
            points->visitData([this, &points, dimensionIndex, &scalarData](auto pointData) {

                // Get reference to source data
                auto sourceData = points->getSourceDataset<Points>();

                if (sourceData->isFull()) {

                    // Cache number of scalars
                    const auto numberOfScalars = static_cast<std::uint32_t>(scalarData.count());

                    // Populate from full dataset
                    for (std::uint32_t pixelIndex = 0; pixelIndex < points->getNumPoints(); pixelIndex++)
                        if (pixelIndex < numberOfScalars && pixelIndex < pointData.size())
                            scalarData[pixelIndex] = pointData[pixelIndex][dimensionIndex];
                }
                else {

                    // Populate from partial dataset
                    for (std::int32_t pixelIndex = 0; pixelIndex < sourceData->indices.size(); pixelIndex++)
                        if (pixelIndex < scalarData.count() && pixelIndex < pointData.size())
                            scalarData[pixelIndex] = pointData[pixelIndex][dimensionIndex];
                }
            });
        }
        else {

            points->visitSourceData([this, dimensionIndex, &globalIndices, &scalarData](auto pointData) {

                // Populate scalar data vector with pixel data
                for (std::int32_t localPointIndex = 0; localPointIndex < globalIndices.size(); localPointIndex++) {

                    // Establish the target pixel index
                    const auto targetPixelIndex = globalIndices[localPointIndex];

                    // And assign the scalar data
                    scalarData[targetPixelIndex] = pointData[localPointIndex][dimensionIndex];
                }
            });
        }
    }

    if (dataset->getDataType() == ClusterType) {

        // Obtain reference to the clusters dataset
        auto clusters = Dataset<Clusters>(dataset);

        auto clusterIndex = 0;

        // Width of the source image
        const auto sourceWidth = getImageSize().width();

        // Get clusters input points dataset
        auto points = dataset->getParent()->getSourceDataset<Points>();

        // Iterate over all clusters
        for (auto& cluster : clusters->getClusters()) {

            // Iterate over all indices in the cluster
            for (const auto index : cluster.getIndices()) {

                // Get translated point index
                const auto pointIndex = points->isFull() ? index : points->indices[index];

                // Compute pixel coordinate
                const auto clusterPixelCoordinate = QPoint(pointIndex % sourceWidth, static_cast<std::int32_t>(floorf(static_cast<float>(pointIndex) / static_cast<float>(sourceWidth))));

                // Compute pixel index into scalar data
                const auto targetPixelIndex = clusterPixelCoordinate.y() * getImageSize().width() + clusterPixelCoordinate.x();

                // Assign cluster index to scalar data
                scalarData[targetPixelIndex] = clusterIndex;
            }

            clusterIndex++;
        }
    }
}

std::vector<std::uint32_t>& Images::getGlobalIndices()
{
    if (!_globalIndices.empty())
        return _globalIndices;

    // Get reference to input dataset
    auto inputDataset = getParent();

    // Get global indices for mapping the selected local indices
    if (inputDataset->getDataType() == PointType)
        Dataset<Points>(inputDataset)->getGlobalIndices(_globalIndices);

    return _globalIndices;
}
