// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Images.h"

#include "ImageData.h"
#include "InfoAction.h"

#include <util/Exception.h>
#include <util/Timer.h>

#include <DataHierarchyItem.h>
#include <Dataset.h>
#include <LinkedData.h>

#include <ClusterData/ClusterData.h>
#include <PointData/PointData.h>

#include <cmath>
#include <exception>
#include <limits>
#include <numeric>
#include <stdexcept>

#include <QDebug>

using namespace mv::util;

Images::Images(QString dataName, bool mayUnderive /*= false*/, const QString& guid /*= ""*/) :
    DatasetImpl(dataName, mayUnderive, guid),
    _indices(),
    _imageData(nullptr),
    _infoAction(),
    _visibleRectangle(),
    _maskData(),
    _maskDataGiven(false)
{
    _imageData = getRawData<ImageData>();

    setLinkedDataFlags(0);
}

void Images::init()
{
    DatasetImpl::init();

    _infoAction = QSharedPointer<InfoAction>::create(nullptr, *this);

    addAction(*_infoAction.get());

    // parent data set must be valid and derived from either point or cluster data
    if (!getDataHierarchyItem().getParent()->getDataset<DatasetImpl>().isValid() ||
        !(getDataHierarchyItem().getParent()->getDataType() == PointType || getDataHierarchyItem().getParent()->getDataType() == ClusterType))
        qCritical() << "Images: warning: image data set must be derived from points or clusters.";

}

std::tuple<mv::Dataset<mv::DatasetImpl>, mv::Dataset<Images>> Images::addImageDataset(QString datasetGuiName, const mv::Dataset<mv::DatasetImpl>& parentDataSet /*= Dataset<DatasetImpl>()*/, const QString pluginKind /*= "Points"*/)
{
    // default to point type
    QString pkind = "Points";
    using ptype = Points;

    if (pluginKind == "Cluster")
    {
        pkind = "Cluster";
        using ptype = Clusters;
    }
    else if (pluginKind != "Points")
        qCritical() << "Images::addImageDataset: warning: pluginKind must be Points or Cluster - defaulting to Points. Given: " << pluginKind;

    auto points = mv::data().createDataset<ptype>(pkind, datasetGuiName, parentDataSet);

    mv::Dataset<Images> images = mv::data().createDataset<Images>("Images", "images", Dataset<DatasetImpl>(*points));

    return { points, images };
}

Dataset<DatasetImpl> Images::createSubsetFromSelection(const QString& guiName, const Dataset<DatasetImpl>& parentDataSet /*= Dataset<DatasetImpl>()*/, const bool& visible /*= true*/) const
{
    return mv::data().createSubsetFromSelection(getSelection(), toSmartPointer(), guiName, parentDataSet, visible);
}

Dataset<DatasetImpl> Images::copy() const
{
    auto images = new Images(getRawDataName());

    images->setText(text());

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

QRect Images::getVisibleRectangle() const
{
    return _visibleRectangle;
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
        const auto numberOfElementsRequired = getNumberOfPixels();

        if (static_cast<std::uint32_t>(scalarData.count()) < numberOfElementsRequired)
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
            scalarDataRange.first   = std::min(scalar, scalarDataRange.first);
            scalarDataRange.second  = std::max(scalar, scalarDataRange.second);
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get scalar data for the given dimension index", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get scalar data for the given dimension index");
    }
}

void Images::getScalarData(const std::vector<std::uint32_t>& dimensionIndices, QVector<float>& scalarData, QPair<float, float>& scalarDataRange)
{
    try
    {
        const auto numberOfPixels               = static_cast<std::int32_t>(getNumberOfPixels());
        const auto numberOfElementsRequired     = dimensionIndices.size() * getNumberOfPixels();
        const auto numberOfComponentsPerPixel   = static_cast<std::int32_t>(getNumberOfComponentsPerPixel());

        if (static_cast<std::uint32_t>(scalarData.count()) < numberOfElementsRequired)
            throw std::runtime_error("Scalar data vector number of elements is smaller than (nDimensions * nPixels)");

        QVector<float> tempScalarData(numberOfElementsRequired);
        QPair<float, float> tempScalarDataRange(std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest());

        switch (_imageData->getType())
        {
            case ImageData::Undefined:
                break;

            case ImageData::Sequence:
            {
                std::int32_t componentIndex = 0;

                for (const auto& dimensionIndex : dimensionIndices) {
                    getScalarDataForImageSequence(dimensionIndex, tempScalarData, tempScalarDataRange);

                    for (std::int32_t pixelIndex = 0; pixelIndex < numberOfPixels; pixelIndex++)
                        scalarData[static_cast<size_t>(pixelIndex * numberOfComponentsPerPixel) + componentIndex] = tempScalarData[pixelIndex];

                    componentIndex++;
                }

                break;
            }

            case ImageData::Stack:
            {
                std::int32_t componentIndex = 0;

                for (const auto& dimensionIndex : dimensionIndices) {
                    getScalarDataForImageStack(dimensionIndex, tempScalarData, tempScalarDataRange);

                    for (std::int32_t pixelIndex = 0; pixelIndex < numberOfPixels; pixelIndex++)
                        scalarData[static_cast<size_t>(pixelIndex * numberOfComponentsPerPixel) + componentIndex] = tempScalarData[pixelIndex];

                    componentIndex++;
                }

                break;
            }

            case ImageData::MultiPartSequence:
                break;

            default:
                break;
        }

        scalarDataRange = { std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest() };

        for (auto& scalar : scalarData) {
            scalarDataRange.first   = std::min(scalar, scalarDataRange.first);
            scalarDataRange.second  = std::max(scalar, scalarDataRange.second);
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get scalar data for the given dimension indices", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get scalar data for the given dimension indices");
    }
}

void Images::getImageScalarData(std::uint32_t imageIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange)
{
    try {
        const std::uint32_t numberOfPixels               = getNumberOfPixels();
        const std::uint32_t numberOfComponentsPerPixel   = getNumberOfComponentsPerPixel();
        const std::uint32_t numberOfElementsRequired     = numberOfPixels * numberOfComponentsPerPixel;

        if (static_cast<std::uint32_t>(scalarData.count()) < numberOfElementsRequired)
            throw std::runtime_error("Scalar data vector number of elements is smaller than (nComponentsPerPixel * nPixels)");

        const std::uint32_t dimensionIndexOffset = imageIndex * numberOfComponentsPerPixel;

        std::vector<std::uint32_t> dimensionIndices;

        dimensionIndices.resize(numberOfComponentsPerPixel);

        std::iota(dimensionIndices.begin(), dimensionIndices.end(), dimensionIndexOffset);

        getScalarData(dimensionIndices, scalarData, scalarDataRange);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get image scalar data", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get image scalar data");
    }
}

void Images::getMaskData(std::vector<std::uint8_t>& maskData)
{
    if (_maskData.empty())
        computeMaskData();

    maskData = _maskData;
}

void Images::setMaskData(const std::vector<std::uint8_t>& maskData)
{
    _maskData = maskData;
    _maskDataGiven = true;
    updateVisibleRectangle();
}

void Images::setMaskData(std::vector<std::uint8_t>&& maskData)
{
    _maskData = std::move(maskData);
    _maskDataGiven = true;
    updateVisibleRectangle();
}

void Images::resetMaskData()
{
    _maskData.clear();
    _maskDataGiven = false;
    updateVisibleRectangle();
}

void Images::getSelectionData(std::vector<std::uint8_t>& selectionImageData, std::vector<std::uint32_t>& selectedIndices, QRect& selectionBoundaries)
{
    try
    {
        // Get smart pointer to parent dataset
        auto parentDataset = getDataHierarchyItem().getParent()->getDataset<DatasetImpl>();

        // Generate selection data for points
        if (parentDataset->getDataType() == PointType) {
            
            // Obtain reference to the point source input dataset
            auto points = Dataset<Points>(parentDataset);

            // Get selection indices from points dataset
            auto& selectionIndices = points->getSelection<Points>()->indices;

            // Clear the selected indices
            selectedIndices.clear();
            selectedIndices.reserve(getNumberOfPixels());

            // Fill selection data with non-selected
            std::fill(selectionImageData.begin(), selectionImageData.end(), 0);

            // Computes and caches the mask data
            computeMaskData();

            // Global indices into data
            std::vector<std::uint32_t> globalIndices;

            // Get global indices from points
            points->getGlobalIndices(globalIndices);

            // Iterate over selection indices and modify the selection boundaries when not masked
            for (const auto& selectionIndex : selectionIndices) {

                // Do not continue if masked
                if (_maskData[selectionIndex] == 0)
                    continue;

                // Selected item is present in the (sub)set, so add it
                selectedIndices.push_back(selectionIndex);

                // Assign selected pixel
                selectionImageData[selectionIndex] = 255;
            }
        }

        // Generate selection data for clusters
        if (parentDataset->getDataType() == ClusterType) {

            // Computes and caches the mask data
            computeMaskData();

            // Obtain reference to the cluster source input dataset
            auto sourceClusters = parentDataset->getSelection<Clusters>();

            // Clear the selected indices
            selectedIndices.clear();
            selectedIndices.reserve(getNumberOfPixels());

            // Fill selection data with non-selected
            std::fill(selectionImageData.begin(), selectionImageData.end(), 0);

            // Get clusters input points dataset
            auto points = parentDataset->getParent()->getSourceDataset<Points>();

            // Global indices into data
            std::vector<std::uint32_t> globalIndices;

            // Get global indices from points
            points->getGlobalIndices(globalIndices);

            // Iterate over all clusters and populate the selection data
            for (const auto& clusterIndex : sourceClusters->indices) {
                
                // Get reference to cluster
                const auto& cluster = sourceClusters->getClusters()[clusterIndex];
                
                // Iterate over all indices in the cluster
                for (const auto& index : cluster.getIndices()) {

                    // Get the global pixel index
                    const auto globalPixelIndex = globalIndices[index];

                    // Add the global index to the list of selected indices
                    selectedIndices.push_back(globalPixelIndex);

                    // Assign selected pixel
                    selectionImageData[globalPixelIndex] = 255;
                }
            }
        }

        // Initialize selection boundaries with numeric extremes
        selectionBoundaries.setTop(std::numeric_limits<int>::max());
        selectionBoundaries.setBottom(std::numeric_limits<int>::lowest());
        selectionBoundaries.setLeft(std::numeric_limits<int>::max());
        selectionBoundaries.setRight(std::numeric_limits<int>::lowest());

        // Compute the selection boundaries from the selected indices
        for (const auto& selectedIndex : selectedIndices) {

            // Compute global pixel coordinate
            const auto globalPixelCoordinate = getPixelCoordinateFromPixelIndex(selectedIndex);

            // Add pixel pixel coordinate and possibly inflate the selection boundaries
            selectionBoundaries.setLeft(std::min(selectionBoundaries.left(), globalPixelCoordinate.x()));
            selectionBoundaries.setRight(std::max(selectionBoundaries.right(), globalPixelCoordinate.x()));
            selectionBoundaries.setTop(std::min(selectionBoundaries.top(), globalPixelCoordinate.y()));
            selectionBoundaries.setBottom(std::max(selectionBoundaries.bottom(), globalPixelCoordinate.y()));
        }

        // Tweak selection boundaries
        selectionBoundaries = selectionBoundaries.marginsAdded(QMargins(0, 0, 1, 1));
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
    auto parentDataset = getDataHierarchyItem().getParent()->getDataset<DatasetImpl>();

    if (parentDataset->getDataType() == PointType) {

        // Obtain reference to the points source input dataset
        auto points = parentDataset->getSourceDataset<Points>();

        QSize sourceImageSize = getImageSize(), targetImageSize;

        // Establish target image size based on sub-sampling
        targetImageSize.setWidth(static_cast<int>(floorf(sourceImageSize.width())));
        targetImageSize.setHeight(static_cast<int>(floorf(sourceImageSize.height())));

        points->visitData([this, points, dimensionIndex, &scalarData, sourceImageSize, targetImageSize](auto pointData) {
            const auto dimensionId       = dimensionIndex;
            const auto imageSize         = _imageData->getImageSize();
            const auto noPixels          = getNumberOfPixels();
            const auto selection         = points->getSelection<Points>();
            const auto& selectionIndices = selection->indices;
            const auto selectionSize     = selectionIndices.size();

            if (!selectionIndices.empty()) {
                for (std::uint32_t p = 0; p < noPixels; p++) {
                    auto sum = 0.0f;

                    for (const auto& selectionIndex : selectionIndices)
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
    //Timer timer(__FUNCTION__);

    auto parent = getParent();

    if (parent->getDataType() == PointType) {
        auto points = Dataset<Points>(parent);

        std::vector<std::uint32_t> globalIndices;

        points->getGlobalIndices(globalIndices);

        if (points->isFull()) {
            points->visitData([this, &points, dimensionIndex, &globalIndices, &scalarData](auto pointData) {
                for (std::int32_t localPointIndex = 0; localPointIndex < globalIndices.size(); localPointIndex++) {
                    const auto targetPixelIndex = globalIndices[localPointIndex];

                    // If the data has any linked data
                    for (LinkedData& linkedData : points->getLinkedData())
                    {
                        // Check if the linked data has the same original full data, because we don't want to
                        // add data here that belongs to a different dataset
                        if (linkedData.getTargetDataset()->getFullDataset<Points>() == points->getSourceDataset<Points>()->getFullDataset<Points>())
                        {
                            SelectionMap::Indices linkedIndices;
                    
                            linkedData.getMapping().populateMappingIndices(targetPixelIndex, linkedIndices);
                            
                            // Fill in the data for all the linked data indices based on the location of the original id
                            for (unsigned int linkedIndex : linkedIndices)
                                scalarData[linkedIndex] = pointData[localPointIndex][dimensionIndex];
                        }
                    }

                    scalarData[targetPixelIndex] = pointData[localPointIndex][dimensionIndex];
                }
            });
        }
        else {
            points->visitData([this, dimensionIndex, &globalIndices, &scalarData](auto pointData) {
                for (std::uint32_t pointIndex = 0; pointIndex < pointData.size(); pointIndex++)
                    scalarData[globalIndices[pointIndex]] = pointData[pointIndex][dimensionIndex];
            });
        }
    }

    // Generate scalars for clusters
    if (parent->getDataType() == ClusterType) {

        // Obtain reference to the clusters dataset
        auto clusters = Dataset<Clusters>(parent);

        // All masked by default
        //std::fill(scalarData.begin(), scalarData.end(), -1.0f);

        // Get clusters input points dataset
        auto points = parent->getParent()->getSourceDataset<Points>();
        auto embedding = parent->getParent();

        // Global indices into data
        //std::vector<std::uint32_t> globalIndices;

        // Get global indices from points
        //points->getGlobalIndices(globalIndices);

        auto clusterIndex = 0;

        // Iterate over all clusters
        for (auto& cluster : clusters->getClusters()) {

            // If the data has any linked data
            for (LinkedData& linkedData : embedding->getLinkedData())
            {
                // Check if the linked data has the same original full data, because we don't want to
                // add data here that belongs to a different dataset
                if (linkedData.getTargetDataset()->getFullDataset<Points>() == embedding->getSourceDataset<Points>()->getFullDataset<Points>())
                {
                    for (const auto globalPointIndex : cluster.getIndices())
                    {
                        SelectionMap::Indices linkedIndices;
                            
                        linkedData.getMapping().populateMappingIndices(globalPointIndex, linkedIndices);

                        // Fill in the data for all the linked data indices based on the location of the original id
                        for (unsigned int linkedIndex : linkedIndices)
                            scalarData[linkedIndex] = clusterIndex;
                    }
                }
            }

            // Iterate over all indices in the cluster and assign cluster index to scalar data
            for (const auto globalPointIndex : cluster.getIndices())
                scalarData[globalPointIndex] = clusterIndex;

            clusterIndex++;
        }
    }
}

void Images::computeMaskData()
{
    //Timer timer(__FUNCTION__);

    // Only compute if necessary
    if (_maskDataGiven)
        return;
        
    // Allocate mask data
    if (_maskData.size() != getNumberOfPixels())
        _maskData.resize(getNumberOfPixels());

    // All masked by default
    std::fill(_maskData.begin(), _maskData.end(), 0);

    // Get reference to input dataset
    auto inputDataset = getParent();

    // Generate mask data for points
    if (inputDataset->getDataType() == PointType) {

        // Obtain reference to the points dataset
        auto points = Dataset<Points>(inputDataset);

        // Global indices into data
        std::vector<std::uint32_t> globalIndices;

        // Get global indices from points
        points->getGlobalIndices(globalIndices);

        // Loop over all point indices and unmask them
        points->visitData([this, &points, &globalIndices](auto pointData) {
            for (std::int32_t localPointIndex = 0; localPointIndex < globalIndices.size(); localPointIndex++) {
                const auto targetPixelIndex = globalIndices[localPointIndex];

                if (hasLinkedDataFlag(DatasetImpl::LinkedDataFlag::Receive)) {

                    // If the data has any linked data
                    for (LinkedData& linkedData : points->getLinkedData())
                    {
                        // Check if the linked data has the same original full data, because we don't want to
                        // add data here that belongs to a different dataset
                        if (linkedData.getTargetDataset()->getFullDataset<Points>() == points->getSourceDataset<Points>()->getFullDataset<Points>())
                        {
                            SelectionMap::Indices linkedIndices;

                            linkedData.getMapping().populateMappingIndices(targetPixelIndex, linkedIndices);

                            // Fill in the data for all the linked data indices based on the location of the original id
                            for (unsigned int linkedIndex : linkedIndices)
                                _maskData[linkedIndex] = 255;
                        }
                    }
                }

                _maskData[targetPixelIndex] = 255;
            }
        });
    }

    // Generate mask data for clusters
    if (inputDataset->getDataType() == ClusterType) {

        // Obtain reference to the clusters dataset
        auto clusters = Dataset<Clusters>(inputDataset);

        // Get clusters input points dataset
        auto points = clusters->getParent()->getSourceDataset<Points>();
        auto embedding = clusters->getParent();

        // Global indices into data
        //std::vector<std::uint32_t> globalIndices;

        // Get global indices from points
        //points->getGlobalIndices(globalIndices);

        // Iterate over all clusters
        for (auto& cluster : clusters->getClusters()) {

            if (hasLinkedDataFlag(DatasetImpl::LinkedDataFlag::Receive)) {

                // If the data has any linked data
                for (LinkedData& linkedData : embedding->getLinkedData())
                {
                    // Check if the linked data has the same original full data, because we don't want to
                    // add data here that belongs to a different dataset
                    if (linkedData.getTargetDataset()->getFullDataset<Points>() == embedding->getSourceDataset<Points>()->getFullDataset<Points>())
                    {
                        for (const auto globalPointIndex : cluster.getIndices())
                        {
                            SelectionMap::Indices linkedIndices;

                            linkedData.getMapping().populateMappingIndices(globalPointIndex, linkedIndices);

                            // Fill in the data for all the linked data indices based on the location of the original id
                            for (unsigned int linkedIndex : linkedIndices)
                                _maskData[linkedIndex] = 255;
                        }
                    }
                }
            }

            // Iterate over all indices in the cluster and assign cluster index to scalar data
            for (const auto globalPointIndex : cluster.getIndices())
                _maskData[globalPointIndex] = 255;
        }
    }

    updateVisibleRectangle();
}

void Images::updateVisibleRectangle()
{
    // Initialize visible rectangle with numeric extremes
    _visibleRectangle.setTop(std::numeric_limits<int>::max());
    _visibleRectangle.setBottom(std::numeric_limits<int>::lowest());
    _visibleRectangle.setLeft(std::numeric_limits<int>::max());
    _visibleRectangle.setRight(std::numeric_limits<int>::lowest());

    // Loop over mask elements and compute the visible rectangle
    for (std::int32_t maskIndex = 0; maskIndex < _maskData.size(); maskIndex++) {

        // Only include visible pixels
        if (_maskData[maskIndex] <= 0)
            continue;

        // Compute pixel coordinate from mask index
        const auto pixelCoordinate = QPoint(maskIndex % getImageSize().width(), static_cast<std::int32_t>(floorf(maskIndex / static_cast<float>(getImageSize().width()))));

        // Add pixel coordinate and possibly inflate the visible rectangle
        _visibleRectangle.setLeft(std::min(_visibleRectangle.left(), pixelCoordinate.x()));
        _visibleRectangle.setRight(std::max(_visibleRectangle.right(), pixelCoordinate.x()));
        _visibleRectangle.setTop(std::min(_visibleRectangle.top(), pixelCoordinate.y()));
        _visibleRectangle.setBottom(std::max(_visibleRectangle.bottom(), pixelCoordinate.y()));
    }
}

QPoint Images::getPixelCoordinateFromPixelIndex(const std::int32_t& pixelIndex) const
{
    return QPoint(pixelIndex % getImageSize().width(), static_cast<std::int32_t>(pixelIndex / static_cast<float>(getImageSize().width())));
}

std::int32_t Images::getPixelIndexFromPixelCoordinate(const QPoint& pixelCoordinate) const
{
    return pixelCoordinate.y() * getImageSize().width() + pixelCoordinate.x();
}

void Images::fromVariantMap(const QVariantMap& variantMap)
{
    DatasetImpl::fromVariantMap(variantMap);

    auto imageData = getRawData<ImageData>();

    if (variantMap.contains("TypeIndex"))
        getRawData<ImageData>()->setType(static_cast<ImageData::Type>(variantMap["TypeIndex"].toInt()));

    if (variantMap.contains("NumberOfImages"))
        getRawData<ImageData>()->setNumberImages(variantMap["NumberOfImages"].toInt());

    if (variantMap.contains("ImageSize")) {
        const auto imageSize = variantMap["ImageSize"].toMap();

        setImageSize(QSize(imageSize["Width"].toInt(), imageSize["Height"].toInt()));
    }

    if (variantMap.contains("NumberOfComponentsPerPixel"))
        setNumberOfComponentsPerPixel(variantMap["NumberOfComponentsPerPixel"].toInt());

    if (variantMap.contains("ImageFilePaths"))
        setImageFilePaths(variantMap["ImageFilePaths"].toStringList());

    if (variantMap.contains("MaskData"))
    {
        auto size = _imageData->getImageSize();
        _maskData.resize(static_cast<size_t>(size.width()) * size.height());
        populateDataBufferFromVariantMap(variantMap["MaskData"].toMap(), (char*)_maskData.data());
    }

    if (variantMap.contains("MaskDataGiven"))
        _maskDataGiven = variantMap["VisibleRectangle"].toBool();

    if (variantMap.contains("VisibleRectangle"))
    {
        const auto visibleRectangle = variantMap["VisibleRectangle"].toMap();
        _visibleRectangle = QRect(visibleRectangle["X"].toInt(), visibleRectangle["Y"].toInt(), visibleRectangle["Width"].toInt(), visibleRectangle["Height"].toInt());
    }

    events().notifyDatasetDataChanged(this);
}

QVariantMap Images::toVariantMap() const
{
    auto variantMap = DatasetImpl::toVariantMap();

    variantMap["TypeIndex"]                     = static_cast<std::int32_t>(getType());
    variantMap["TypeName"]                      = ImageData::getTypeName(getType());
    variantMap["NumberOfImages"]                = getNumberOfImages();
    variantMap["ImageSize"]                     = QVariantMap({ { "Width", getImageSize().width() }, { "Height", getImageSize().height() } });
    variantMap["NumberOfComponentsPerPixel"]    = getNumberOfComponentsPerPixel();
    variantMap["ImageFilePaths"]                = getImageFilePaths();
    variantMap["MaskData"]                      = rawDataToVariantMap((char*)_maskData.data(), _maskData.size() * sizeof(std::uint8_t), true);
    variantMap["MaskDataGiven"]                 = _maskDataGiven;
    variantMap["VisibleRectangle"]              = QVariantMap({ { "X", _visibleRectangle.x() }, { "Y", _visibleRectangle.y() },{ "Width", _visibleRectangle.width() }, { "Height", _visibleRectangle.height() } });

    return variantMap;
}
