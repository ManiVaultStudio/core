#include "Images.h"
#include "ImageData.h"
#include "InfoAction.h"

#include "util/Exception.h"

#include "DataHierarchyItem.h"

#include "PointData.h"

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

QStringList Images::imageFilePaths() const
{
    return _imageData->getImageFilePaths();
}

void Images::setImageFilePaths(const QStringList& imageFilePaths)
{
    _imageData->setImageFilePaths(imageFilePaths);
}

QStringList Images::getDimensionNames() const
{
    return _imageData->getDimensionNames();
}

void Images::setDimensionNames(const QStringList& dimensionNames)
{
    _imageData->setDimensionNames(dimensionNames);
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

void Images::getScalarDataForImageSequence(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange)
{
    auto& points = _core->getDataHierarchyItem(getName())->getParent()->getDataset<Points>();

    points.visitData([this, points, dimensionIndex, &scalarData](auto pointData) {
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
            for (std::uint32_t p = 0; p < noPixels; p++)
                scalarData[p] = pointData[dimensionIndex][p];
        }
    });
}

void Images::getScalarDataForImageStack(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange)
{
    auto& points = _core->getDataHierarchyItem(getName())->getParent()->getDataset<Points>();

    if (points.isDerivedData()) {
        points.visitData([this, &points, dimensionIndex, &scalarData](auto pointData) {
            auto& sourceData = points.getSourceData<Points>(points);

            if (sourceData.isFull()) {
                for (std::uint32_t i = 0; i < points.getNumPoints(); i++)
                    scalarData[i] = pointData[i][dimensionIndex];
            }
            else {
                for (int i = 0; i < sourceData.indices.size(); i++)
                    scalarData[sourceData.indices[i]] = pointData[i][dimensionIndex];
            }
        });
    }
    else {
        points.visitSourceData([this, dimensionIndex, &scalarData](auto pointData) {
            for (auto pointView : pointData)
                scalarData[pointView.index()] = pointView[dimensionIndex];
        });
    }
}
