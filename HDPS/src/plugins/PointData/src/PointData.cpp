#ifndef HDPS_ASSUME_FLOAT32_POINT_DATA
// Avoid warnings about calls to deprecated PointData member functions.
#define HDPS_ASSUME_FLOAT32_POINT_DATA
#endif

#include "PointData.h"
#include "InfoAction.h"
#include "event/Event.h"

#include <QtCore>
#include <QtDebug>
#include <QPainter>

#include <cstring>
#include <type_traits>
#include <queue>
#include <set>

#include "graphics/Vector2f.h"
#include "Application.h"

#include <util/Serialization.h>
#include <util/Timer.h>

Q_PLUGIN_METADATA(IID "nl.tudelft.PointData")

// =============================================================================
// PointData
// =============================================================================

using namespace hdps::util;

PointData::~PointData(void)
{
    
}

void PointData::init()
{

}

hdps::Dataset<DatasetImpl> PointData::createDataSet(const QString& guid /*= ""*/) const
{
    return hdps::Dataset<DatasetImpl>(new Points(_core, getName(), guid));
}

unsigned int PointData::getNumPoints() const
{
    return static_cast<unsigned int>(_vectorHolder.size() / _numDimensions);
}

unsigned int PointData::getNumDimensions() const
{
    return _numDimensions;
}

const std::vector<QString>& PointData::getDimensionNames() const
{
    return _dimNames;
}

void PointData::setData(const std::nullptr_t, const std::size_t numPoints, const std::size_t numDimensions)
{
    _vectorHolder.resize(numPoints * numDimensions);
    _numDimensions = static_cast<unsigned int>(numDimensions);
}

void PointData::setDimensionNames(const std::vector<QString>& dimNames)
{
    _dimNames = dimNames;
}

float PointData::getValueAt(const std::size_t index) const
{
    return _vectorHolder.constVisit<float>([index](const auto& vec)
        {
            return vec[index];
        });
}

void PointData::setValueAt(const std::size_t index, const float newValue)
{
    _vectorHolder.visit([index, newValue](auto& vec)
        {
            using value_type = typename std::remove_reference_t<decltype(vec)>::value_type;
            vec[index] = static_cast<value_type>(newValue);
        });
}

void PointData::fromVariantMap(const QVariantMap& variantMap)
{
    const auto data                 = variantMap["Data"].toMap();
    const auto numberOfPoints       = static_cast<size_t>(variantMap["NumberOfPoints"].toInt());
    const auto numberOfDimensions   = static_cast<size_t>(variantMap["NumberOfDimensions"].toInt());
    const auto numberOfElements     = numberOfPoints * numberOfDimensions;
    const auto elementTypeIndex     = static_cast<PointData::ElementTypeSpecifier>(data["TypeIndex"].toInt());
    const auto rawData              = data["Raw"].toMap();

    switch (elementTypeIndex)
    {
        case PointData::ElementTypeSpecifier::float32:
        {
            std::vector<float> pointData;

            pointData.resize(numberOfElements);

            populateDataBufferFromVariantMap(rawData, (char*)pointData.data());
            setData(pointData, numberOfDimensions);
            break;
        }

        case PointData::ElementTypeSpecifier::bfloat16:
        {
            std::vector<biovault::bfloat16_t> pointData;

            pointData.resize(numberOfElements);

            populateDataBufferFromVariantMap(rawData, (char*)pointData.data());
            setData(pointData, numberOfDimensions);

            break;
        }

        case PointData::ElementTypeSpecifier::int16:
        {
            std::vector<std::int16_t> pointData;

            pointData.resize(numberOfElements);

            populateDataBufferFromVariantMap(rawData, (char*)pointData.data());
            setData(pointData, numberOfDimensions);
            break;
        }

        case PointData::ElementTypeSpecifier::uint16:
        {
            std::vector<std::uint16_t> pointData;

            pointData.resize(numberOfElements);

            populateDataBufferFromVariantMap(rawData, (char*)pointData.data());
            setData(pointData, numberOfDimensions);
            break;
        }

        case PointData::ElementTypeSpecifier::int8:
        {
            std::vector<std::uint16_t> pointData;

            pointData.resize(numberOfElements);

            populateDataBufferFromVariantMap(rawData, (char*)pointData.data());
            setData(pointData, numberOfDimensions);
            break;
        }

        case PointData::ElementTypeSpecifier::uint8:
        {
            std::vector<std::uint8_t> pointData;

            pointData.resize(numberOfElements);

            populateDataBufferFromVariantMap(rawData, (char*)pointData.data());
            setData(pointData, numberOfDimensions);
            break;
        }

        default:
            break;
    }
}

QVariantMap PointData::toVariantMap() const
{
    QVariantMap rawData;

    const auto typeSpecifier        = _vectorHolder.getElementTypeSpecifier();
    const auto typeSpecifierName    = _vectorHolder.getElementTypeNames()[static_cast<std::int32_t>(typeSpecifier)];
    const auto typeIndex            = static_cast<std::int32_t>(typeSpecifier);
    const auto numberOfElements     = static_cast<std::uint64_t>(getNumPoints() * getNumDimensions());

    switch (typeSpecifier)
    {
        case ElementTypeSpecifier::float32:
            rawData = rawDataToVariantMap((char*)_vectorHolder.getConstVector<float>().data(), numberOfElements * sizeof(float), true);
            break;

        case ElementTypeSpecifier::bfloat16:
            rawData = rawDataToVariantMap((char*)_vectorHolder.getConstVector<biovault::bfloat16_t>().data(), numberOfElements * sizeof(biovault::bfloat16_t), true);
            break;

        case ElementTypeSpecifier::int16:
            rawData = rawDataToVariantMap((char*)_vectorHolder.getConstVector<std::int16_t>().data(), numberOfElements * sizeof(std::int16_t), true);
            break;

        case ElementTypeSpecifier::uint16:
            rawData = rawDataToVariantMap((char*)_vectorHolder.getConstVector<std::uint16_t>().data(), numberOfElements * sizeof(std::uint16_t), true);
            break;

        case ElementTypeSpecifier::int8:
            rawData = rawDataToVariantMap((char*)_vectorHolder.getConstVector<std::int8_t>().data(), numberOfElements * sizeof(std::int8_t), true);
            break;

        case ElementTypeSpecifier::uint8:
            rawData = rawDataToVariantMap((char*)_vectorHolder.getConstVector<std::uint8_t>().data(), numberOfElements * sizeof(std::uint8_t), true);
            break;

        default:
            break;
    }

    return {
        { "TypeIndex", QVariant::fromValue(typeIndex) },
        { "TypeName", QVariant(typeSpecifierName) },
        { "Raw", QVariant::fromValue(rawData) },
        { "NumberOfElements", QVariant::fromValue(numberOfElements) }
    };
}

void PointData::extractFullDataForDimension(std::vector<float>& result, const int dimensionIndex) const
{
    CheckDimensionIndex(dimensionIndex);

    result.resize(getNumPoints());

    _vectorHolder.constVisit(
        [&result, this, dimensionIndex](const auto& vec)
        {
            const auto resultSize = result.size();

            for (std::size_t i{}; i < resultSize; ++i)
            {
                result[i] = vec[i * _numDimensions + dimensionIndex];
            }
        });
}


void PointData::extractFullDataForDimensions(std::vector<hdps::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const
{
    CheckDimensionIndex(dimensionIndex1);
    CheckDimensionIndex(dimensionIndex2);

    result.resize(getNumPoints());

    _vectorHolder.constVisit(
        [&result, this, dimensionIndex1, dimensionIndex2](const auto& vec)
        {
            const auto resultSize = result.size();

            for (std::size_t i{}; i < resultSize; ++i)
            {
                const auto n = i * _numDimensions;
                result[i].set(vec[n + dimensionIndex1], vec[n + dimensionIndex2]);
            }
        });
}


void PointData::extractDataForDimensions(std::vector<hdps::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, const std::vector<unsigned int>& indices) const
{
    CheckDimensionIndex(dimensionIndex1);
    CheckDimensionIndex(dimensionIndex2);

    result.resize(indices.size());

    _vectorHolder.constVisit(
        [&result, this, dimensionIndex1, dimensionIndex2, indices](const auto& vec)
        {
            const auto resultSize = result.size();

            for (std::size_t i{}; i < resultSize; ++i)
            {
                const auto n = std::size_t{ indices[i] } *_numDimensions;
                result[i].set(vec[n + dimensionIndex1], vec[n + dimensionIndex2]);
            }
        });
}

Points::Points(hdps::CoreInterface* core, QString dataName, const QString& guid /*= ""*/) :
    hdps::DatasetImpl(core, dataName, guid),
    _infoAction()
{
}

Points::~Points()
{
}

void Points::init()
{
    _infoAction = QSharedPointer<InfoAction>::create(this, *this);

    addAction(*_infoAction.get());

    _eventListener.setEventCore(_core);
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataSelectionChanged));
    _eventListener.registerDataEventByType(PointType, [this](DataEvent* dataEvent) {

        // Only process selection changes
        if (dataEvent->getType() != EventType::DataSelectionChanged)
            return;

        // Do not process our own selection changes
        if (dataEvent->getDataset() == Dataset<Points>(this))
            return;

        // Only synchronize when dataset grouping is enabled and our own group index is non-negative
        if (!_core->isDatasetGroupingEnabled() || getGroupIndex() < 0)
            return;

        // Only synchronize of the group indexes match
        if (dataEvent->getDataset()->getGroupIndex() != getGroupIndex())
            return;

        // Get smart pointer to foreign points dataset
        auto foreignPoints = dataEvent->getDataset<Points>();

        // Only synchronize when the number of points matches
        if (foreignPoints->getNumPoints() != getNumPoints())
            return;

        // Get source target indices
        auto& sourceIndices = foreignPoints->getSelection<Points>()->indices;
        auto& targetIndices = getSelection<Points>()->indices;

        // Do nothing if the indices have not changed
        if (sourceIndices == targetIndices)
            return;

        // Copy indices from source to target if the indices have changed
        targetIndices = sourceIndices;

        // Notify others that the cluster selection has changed
        _core->notifyDatasetSelectionChanged(this);
    });
}

// =============================================================================
// Point Set
// =============================================================================


void Points::setData(std::nullptr_t, const std::size_t numPoints, const std::size_t numDimensions)
{
    getRawData<PointData>().setData(nullptr, numPoints, numDimensions);
}

void Points::extractDataForDimension(std::vector<float>& result, const int dimensionIndex) const
{
    // This overload assumes that the data set is "full".
    // Please remove the assert once non-full support is implemented (if necessary).
    assert(isFull());

    if (isProxy()) {
        result.resize(getNumPoints());

        auto pointIndexOffset = 0u;

        for (auto proxyDataset : getProxyDatasets()) {
            auto points = hdps::Dataset<Points>(proxyDataset);

            std::vector<float> proxyPoints;

            proxyPoints.resize(points->getNumPoints());

            points->extractDataForDimension(proxyPoints, dimensionIndex);

            std::copy(proxyPoints.begin(), proxyPoints.end(), result.begin() + pointIndexOffset);

            pointIndexOffset += points->getNumPoints();
        }
    }
    else {
        getRawData<PointData>().extractFullDataForDimension(result, dimensionIndex);
    }
}

void Points::extractDataForDimensions(std::vector<hdps::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const
{
    if (isProxy()) {
        result.resize(getNumPoints());

        auto pointIndexOffset = 0u;

        for (auto proxyDataset : getProxyDatasets()) {
            auto points = hdps::Dataset<Points>(proxyDataset);

            std::vector<hdps::Vector2f> proxyPoints;

            proxyPoints.resize(points->getNumPoints());

            points->extractDataForDimensions(proxyPoints, dimensionIndex1, dimensionIndex2);

            std::copy(proxyPoints.begin(), proxyPoints.end(), result.begin() + pointIndexOffset);

            pointIndexOffset += points->getNumPoints();
        }
    }
    else {
        const auto& rawPointData = getRawData<PointData>();

        if (isFull())
            rawPointData.extractFullDataForDimensions(result, dimensionIndex1, dimensionIndex2);
        else
            rawPointData.extractDataForDimensions(result, dimensionIndex1, dimensionIndex2, indices);
    }
}

void Points::getGlobalIndices(std::vector<unsigned int>& globalIndices) const
{
    //Timer timer(__FUNCTION__);

    if (isProxy()) {
        globalIndices.resize(getNumPoints(), 0);
        std::iota(globalIndices.begin(), globalIndices.end(), 0);
    }
    else {
        auto currentDataset = toSmartPointer<Points>();

        std::queue<Dataset<Points>> subsetChain;

        // Walk back in the chain of derived data until we find the original source
        while (currentDataset->isDerivedData())
        {
            // If the current set is a subset then store it on the stack to traverse later
            if (!isFull())
            {
                subsetChain.push(currentDataset);
            }
            currentDataset = getSourceDataset<Points>();
        }

        // We now have a non-derived dataset bound, push it if its also a subset
        if (!currentDataset->isFull())
            subsetChain.push(currentDataset);

        // Traverse down the stack applying indexing
        globalIndices.resize(getNumPoints(), 0);
        std::iota(globalIndices.begin(), globalIndices.end(), 0);

        while (!subsetChain.empty())
        {
            const Points& subset = *subsetChain.front();
            subsetChain.pop();

            for (int i = 0; i < globalIndices.size(); i++)
            {
                globalIndices[i] = subset.indices[globalIndices[i]];
            }
        }
    }
}

void Points::selectedLocalIndices(const std::vector<unsigned int>& selectionIndices, std::vector<bool>& selected) const
{
    //Timer timer(__FUNCTION__);

    // Find the global indices of this dataset
    std::vector<unsigned int> localGlobalIndices;
    getGlobalIndices(localGlobalIndices);

    if (isProxy()) {
        selected.resize(getNumPoints(), false);

        for (const auto& selectionIndex : selectionIndices) {
            selected[localGlobalIndices[selectionIndex]] = true;
        }
    }
    else {

        // In an array the size of the full raw data, mark selected points as true
        std::vector<bool> globalSelection(getSourceDataset<Points>()->getNumRawPoints(), false);

        for (const unsigned int& selectionIndex : selectionIndices)
            globalSelection[selectionIndex] = true;

        // For all local points find out which are selected
        selected.resize(localGlobalIndices.size(), false);
        for (int i = 0; i < localGlobalIndices.size(); i++)
        {
            if (globalSelection[localGlobalIndices[i]])
                selected[i] = true;
        }
    }
}

void Points::getLocalSelectionIndices(std::vector<unsigned int>& localSelectionIndices) const
{
    //Timer timer(__FUNCTION__);

    if (isProxy()) {
        localSelectionIndices = getSelection<Points>()->indices;
    }
    else {
        auto selection = getSelection<Points>();

        // Find the global indices of this dataset
        std::vector<unsigned int> localGlobalIndices;
        getGlobalIndices(localGlobalIndices);

        // In an array the size of the full raw data, mark selected points as true
        std::vector<bool> globalSelection(getSourceDataset<Points>()->getNumRawPoints(), false);
        for (const unsigned int& selectionIndex : selection->indices)
            globalSelection[selectionIndex] = true;

        // For all local points find out which are selected
        std::vector<bool> selected(localGlobalIndices.size(), false);
        int indexCount = 0;
        for (int i = 0; i < localGlobalIndices.size(); i++)
        {
            if (globalSelection[localGlobalIndices[i]])
            {
                selected[i] = true;
                indexCount++;
            }
        }

        localSelectionIndices.resize(indexCount);
        int c = 0;
        for (int i = 0; i < selected.size(); i++)
        {
            if (selected[i])
                localSelectionIndices[c++] = i;
        }
    }
}

bool Points::mayProxy(const Datasets& proxyDatasets) const
{
    if (!DatasetImpl::mayProxy(proxyDatasets))
        return false;

    QSet<std::int32_t> numberOfDimensionsSet;

    for (auto proxyDataset : proxyDatasets)
        numberOfDimensionsSet.insert(Dataset<Points>(proxyDataset)->getNumDimensions());
    
    if (numberOfDimensionsSet.count() > 1)
        return false;

    return true;
}

Dataset<DatasetImpl> Points::copy() const
{
    auto set = new Points(_core, getRawDataName());

    set->setGuiName(getGuiName());
    set->indices = indices;

    return set;
}

Dataset<DatasetImpl> Points::createSubsetFromSelection(const QString& guiName, const Dataset<DatasetImpl>& parentDataSet /*= Dataset<DatasetImpl>()*/, const bool& visible /*= true*/) const
{
    return _core->createSubsetFromSelection(getSelection(), toSmartPointer(), guiName, parentDataSet, visible);
}

QIcon Points::getIcon(const QColor& color /*= Qt::black*/) const
{
    return hdps::Application::getIconFont("FontAwesome").getIcon("database", color);

    /*
    const auto size = QSize(100, 100);

    QPixmap pixmap(size);

    pixmap.fill(Qt::transparent);

    const auto iconRectangle = QRect(0, 0, size.width(), size.height());

    QPainter painter(&pixmap);

    painter.setRenderHint(QPainter::Antialiasing);

    QPen linepen(Qt::black);
    linepen.setCapStyle(Qt::RoundCap);
    linepen.setWidth(15);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(linepen);

    QVector<QPoint> points;

    QPolygon polygon;

    const auto numSteps     = 3;
    const auto stepSizeX    = static_cast<float>(size.width()) / static_cast<float>(numSteps + 1);
    const auto stepSizeY    = static_cast<float>(size.height()) / static_cast<float>(numSteps + 1);

    for (int x = stepSizeX; x < size.width(); x += stepSizeX)
        for (int y = stepSizeY; y < size.height(); y += stepSizeY)
            polygon << QPoint(x, y);

    painter.drawPoints(polygon);

    return QIcon(pixmap);
    */
}

void Points::setProxyDatasets(const Datasets& proxyDatasets)
{
    DatasetImpl::setProxyDatasets(proxyDatasets);

    auto pointIndexOffset = 0u;

    for (auto proxyDataset : getProxyDatasets()) {
        auto targetPoints = Dataset<Points>(proxyDataset);

        std::vector<std::uint32_t> targetGlobalIndices;

        targetPoints->getGlobalIndices(targetGlobalIndices);

        // Group dataset to member dataset
        {
            SelectionMap selectionMapToTarget;

            for (std::uint32_t pointIndex = 0; pointIndex < targetPoints->getNumPoints(); ++pointIndex)
                selectionMapToTarget[pointIndexOffset + pointIndex] = std::vector<std::uint32_t>({ targetGlobalIndices[pointIndex] });

            addLinkedData(targetPoints, selectionMapToTarget);
        }

        // Member dataset to group dataset
        {
            SelectionMap selectionMapToSource;

            for (std::uint32_t pointIndex = 0; pointIndex < targetPoints->getNumPoints(); ++pointIndex)
                selectionMapToSource[targetGlobalIndices[pointIndex]] = std::vector<std::uint32_t>({ pointIndexOffset + pointIndex });

            targetPoints->addLinkedData(toSmartPointer(), selectionMapToSource);

            if (!targetPoints->isFull())
                targetPoints->getFullDataset<Points>()->addLinkedData(toSmartPointer(), selectionMapToSource);
        }

        pointIndexOffset += targetPoints->getNumPoints();
    }
}

std::vector<std::uint32_t>& Points::getSelectionIndices()
{
    return getSelection<Points>()->indices;
}

const std::vector<QString>& Points::getDimensionNames() const
{
    if (isProxy()) {
        return hdps::Dataset<Points>(getProxyDatasets().first())->getDimensionNames();
    }
    else {
        return getRawData<PointData>().getDimensionNames();
    }
}

void Points::setDimensionNames(const std::vector<QString>& dimNames)
{
    getRawData<PointData>().setDimensionNames(dimNames);
}

float Points::getValueAt(const std::size_t index) const
{
    return getRawData<PointData>().getValueAt(index);
}

void Points::setValueAt(const std::size_t index, const float newValue)
{
    getRawData<PointData>().setValueAt(index, newValue);
}

void resolveLinkedData(LinkedData& ld, const std::vector<std::uint32_t>& indices)
{
    Dataset<Points> sourceDataset   = ld.getSourceDataSet();
    Dataset<Points> targetDataset   = ld.getTargetDataset();
    Dataset<Points> targetSelection = targetDataset->getSelection();

    qDebug() << __FUNCTION__ << sourceDataset->getGuiName() << targetDataset->getGuiName() << targetSelection->indices.size();

    const hdps::SelectionMap& mapping = ld.getMapping();   

    // Create separate vector of additional linked selected points
    std::vector<unsigned int> linkedIndices;
    
    // Reserve at least as much space as required for a 1-1 mapping
    linkedIndices.reserve(indices.size());

    for (const int selectionIndex : indices)
    {
        if (mapping.find(selectionIndex) != mapping.end())
        {
            const std::vector<unsigned int>& mappedSelection = mapping.at(selectionIndex);
            linkedIndices.insert(linkedIndices.end(), mappedSelection.begin(), mappedSelection.end());
        }
    }
    
    std::set<std::uint32_t> targetIndicesSet(targetSelection->indices.begin(), targetSelection->indices.end());

    for (auto& [key, value] : mapping)
        for (const auto& v : value)
            targetIndicesSet.erase(v);

    for (const auto& linkedIndex : linkedIndices)
        targetIndicesSet.insert(linkedIndex);

    targetSelection->indices = std::vector<std::uint32_t>(targetIndicesSet.begin(), targetIndicesSet.end());

    if (targetDataset->isProxy()) {
        
        for (auto targetLd : targetDataset->getLinkedData())
        {
            resolveLinkedData(targetLd, targetSelection->indices);
            Application::core()->notifyDatasetSelectionChanged(targetLd.getTargetDataset());
        }
    }
}

void Points::setSelectionIndices(const std::vector<std::uint32_t>& indices)
{
    //qDebug() << __FUNCTION__ << indices;

    auto selection = getSelection<Points>();

    selection->indices = indices;

    // Check for linked data in this dataset and in potential source data, and resolve it
    for (hdps::LinkedData& linkedData : getLinkedData())
        resolveLinkedData(linkedData, indices);

    // Check if the dataset is derived, and if so, resolve the linked selections of the source dataset as well
    if (isDerivedData())
    {
        Dataset<Points> sourceData = getSourceDataset<Points>();
        for (hdps::LinkedData& linkedData : sourceData->getLinkedData())
            resolveLinkedData(linkedData, indices);
    }
}

bool Points::canSelect() const
{
    return getNumPoints() > 0;
}

bool Points::canSelectAll() const
{
    return getNumPoints() != getSelectionSize();
}

bool Points::canSelectNone() const
{
    return getSelection<Points>()->indices.size() >= 1;
}

bool Points::canSelectInvert() const
{
    return true;
}

void Points::selectAll()
{
    auto& selectionIndices = getSelection<Points>()->indices;

    selectionIndices.clear();
    selectionIndices.resize(getNumPoints());

    if (isFull()) {
        std::iota(selectionIndices.begin(), selectionIndices.end(), 0);
    }
    else {
        for (const auto& index : indices)
            selectionIndices.push_back(index);
    }

    _core->notifyDatasetSelectionChanged(this);
}

void Points::selectNone()
{
    auto& selectionIndices = getSelection<Points>()->indices;

    selectionIndices.clear();

    _core->notifyDatasetSelectionChanged(this);
}

void Points::selectInvert()
{
    auto& selectionIndices = getSelection<Points>()->indices;

    std::set<std::uint32_t> selectionSet(selectionIndices.begin(), selectionIndices.end());

    const auto noPixels = getNumPoints();

    selectionIndices.clear();
    selectionIndices.reserve(noPixels - selectionSet.size());

    for (std::uint32_t i = 0; i < noPixels; i++) {
        if (selectionSet.find(i) == selectionSet.end())
            selectionIndices.push_back(i);
    }

    _core->notifyDatasetSelectionChanged(this);
}

void Points::fromVariantMap(const QVariantMap& variantMap)
{
    DatasetImpl::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Full");
    variantMapMustContain(variantMap, "NumberOfDimensions");

    setAll(variantMap["Full"].toBool());

    if (isFull())
        getRawData<PointData>().fromVariantMap(variantMap);

    if (!isFull()) {
        auto parent = getParent();

        makeSubsetOf(getParent());

        variantMapMustContain(variantMap, "Indices");

        const auto& indicesMap = variantMap["Indices"].toMap();

        indices.resize(indicesMap["Count"].toInt());

        populateDataBufferFromVariantMap(indicesMap["Raw"].toMap(), (char*)indices.data());
    }

    // Fetch dimension names from map
    const auto fetchDimensionNames = [&variantMap]() -> QStringList {
        QStringList dimensionNames;

        // Dimension names in byte array format
        QByteArray dimensionsByteArray;

        // Copy the dimension names raw data into the byte array
        dimensionsByteArray.resize(variantMap["DimensionNames"].toMap()["Size"].value<std::uint64_t>());
        populateDataBufferFromVariantMap(variantMap["DimensionNames"].toMap(), (char*)dimensionsByteArray.data());

        // Open input data stream
        QDataStream dimensionsDataStream(&dimensionsByteArray, QIODevice::ReadOnly);

        // Stream the data to the dimension names
        dimensionsDataStream >> dimensionNames;

        return dimensionNames;
    };

    std::vector<QString> dimensionNames;

    for (const auto dimensionName : fetchDimensionNames())
        dimensionNames.push_back(dimensionName);

    setDimensionNames(dimensionNames);

    _core->notifyDatasetChanged(this);
}

QVariantMap Points::toVariantMap() const
{
    auto variantMap = DatasetImpl::toVariantMap();

    QStringList dimensionNames;

    if (getDimensionNames().size() != getNumPoints()) {
        for (const auto dimensionName : getDimensionNames())
            dimensionNames << dimensionName;
    }
    else {
        for (std::uint32_t dimensionIndex = 0; dimensionIndex < getNumDimensions(); dimensionIndex++)
            dimensionNames << QString("Dim %1").arg(QString::number(dimensionIndex));
    }

    QByteArray dimensionsByteArray;
    QDataStream dimensionsDataStream(&dimensionsByteArray, QIODevice::WriteOnly);

    dimensionsDataStream << dimensionNames;

    QVariantMap indices;

    indices["Count"]    = QVariant::fromValue(this->indices.size());
    indices["Raw"]      = rawDataToVariantMap((char*)this->indices.data(), this->indices.size() * sizeof(std::uint32_t), true);

    variantMap["Data"]                  = isFull() ? getRawData<PointData>().toVariantMap() : QVariantMap();
    variantMap["NumberOfPoints"]        = getNumPoints();
    variantMap["Full"]                  = isFull();
    variantMap["Indices"]               = indices;
    variantMap["DimensionNames"]        = rawDataToVariantMap((char*)dimensionsByteArray.data(), dimensionsByteArray.size(), true);
    variantMap["NumberOfDimensions"]    = getNumDimensions();

    return variantMap;
}

// =============================================================================
// Factory
// =============================================================================

QIcon PointDataFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("circle", color);
}

hdps::plugin::RawData* PointDataFactory::produce()
{
    return new PointData(this);
}
