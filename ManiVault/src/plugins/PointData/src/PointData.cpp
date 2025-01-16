// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PointData.h"

#include "DimensionsPickerAction.h"
#include "InfoAction.h"

#include <Application.h>
#include <DataHierarchyItem.h>

#include <actions/GroupAction.h>
#include <event/Event.h>
#include <graphics/Vector2f.h>
#include <util/Serialization.h>
#include <util/Timer.h>

#include <QDebug>
#include <QPainter>
#include <QtCore>

#include <cstring>
#include <set>
#include <type_traits>

Q_PLUGIN_METADATA(IID "studio.manivault.PointData")

// =============================================================================
// PointData
// =============================================================================

using namespace mv::util;

PointData::~PointData(void)
{
    
}

void PointData::init()
{

}

mv::Dataset<DatasetImpl> PointData::createDataSet(const QString& guid /*= ""*/) const
{
    return mv::Dataset<DatasetImpl>(new Points(getName(), true, guid));
}

std::uint32_t PointData::getNumPoints() const
{
    if (_isDense)
        return static_cast<unsigned int>(getSizeOfVector() / _numDimensions);
    else
        return _numRows;
}

std::uint32_t PointData::getNumDimensions() const
{
    return _numDimensions;
}

std::uint64_t PointData::getNumberOfElements() const
{
    return getSizeOfVector();
}


std::uint64_t  PointData::getRawDataSize() const
{
    if (_isDense)
    {
        std::uint64_t elementSize = std::visit([](auto& vec) { return vec.empty() ? 0u : sizeof(vec[0]); }, _variantOfVectors);
        return elementSize * getNumberOfElements();
    }
    else
    {
        return (_numRows + 4) * sizeof(size_t) + _sparseData.getNumNonZeros() * (sizeof(uint32_t) + sizeof(float));
    }
}

void* PointData::getDataVoidPtr()
{
    return std::visit([](auto& vec) { return (void*)vec.data(); }, _variantOfVectors);
}

const void* PointData::getDataConstVoidPtr() const
{
    return std::visit([](const auto& vec) { return (const void*)vec.data(); }, _variantOfVectors);
}

const std::vector<QString>& PointData::getDimensionNames() const
{
    return _dimNames;
}

void PointData::setData(const std::nullptr_t, const std::size_t numPoints, const std::size_t numDimensions)
{
    resizeVector(numPoints * numDimensions);
    _numDimensions = static_cast<unsigned int>(numDimensions);
}

void PointData::setDimensionNames(const std::vector<QString>& dimNames)
{
    if (dimNames.empty())
        return;

    _dimNames = dimNames;

    if (dimNames.size() != _numDimensions)
        qWarning() << "PointData: Number of dimension names does not equal the number of data dimensions";
}

float PointData::getValueAt(const std::size_t index) const
{
    return std::visit([index](const auto& vec)
        {
            return static_cast<float>(vec[index]);
        },
        _variantOfVectors);
}

void PointData::setValueAt(const std::size_t index, const float newValue)
{
    std::visit([index, newValue](auto& vec)
        {
            using value_type = typename std::remove_reference_t<decltype(vec)>::value_type;
            vec[index] = static_cast<value_type>(newValue);
        },
        _variantOfVectors);
}

void PointData::fromVariantMap(const QVariantMap& variantMap)
{
    variantMapMustContain(variantMap, "Data");
    variantMapMustContain(variantMap, "NumberOfPoints");
    variantMapMustContain(variantMap, "NumberOfDimensions");

    const auto data                 = variantMap["Data"].toMap();
    const auto numberOfPoints       = static_cast<size_t>(variantMap["NumberOfPoints"].toInt());
    const auto numberOfDimensions   = variantMap["NumberOfDimensions"].toUInt();
    const auto numberOfElements     = numberOfPoints * numberOfDimensions;
    const auto elementTypeIndex     = static_cast<PointData::ElementTypeSpecifier>(data["TypeIndex"].toInt());
    const auto rawData              = data["Raw"].toMap();

    bool isDense = true;
    if (variantMap.contains("Dense"))
        isDense = variantMap["Dense"].toBool();;

    _isDense = isDense;
    _numDimensions = numberOfDimensions;

    if (_isDense)
    {
        setElementTypeSpecifier(elementTypeIndex);
        resizeVector(numberOfElements);
        populateDataBufferFromVariantMap(rawData, (char*)getDataVoidPtr());
    }
    else
    {
        variantMapMustContain(variantMap, "NumberOfNonZeroElements");

        const auto numberOfNonZeroElements = variantMap["NumberOfNonZeroElements"].toULongLong();

        std::vector<char> bytes((numberOfPoints + 1) * sizeof(size_t) + numberOfNonZeroElements * (sizeof(uint32_t) + sizeof(float)));

        populateDataBufferFromVariantMap(rawData, bytes.data());
        _numRows = static_cast<unsigned int>(numberOfPoints); // FIXME should be redundant

        size_t offset = 0;
        std::vector<size_t> rowPointers(numberOfPoints + 1);
        std::memcpy(rowPointers.data(), bytes.data() + offset, rowPointers.size() * sizeof(size_t));

        offset += rowPointers.size() * sizeof(size_t);
        std::vector<uint32_t> colIndices(numberOfNonZeroElements);
        std::memcpy(colIndices.data(), bytes.data() + offset, colIndices.size() * sizeof(uint32_t));

        offset += colIndices.size() * sizeof(float);
        std::vector<float> values(numberOfNonZeroElements);
        std::memcpy(values.data(), bytes.data() + offset, values.size() * sizeof(float));

        _sparseData.setData(numberOfPoints, numberOfDimensions, std::move(rowPointers), std::move(colIndices), std::move(values));

        qDebug() << "Loaded sparse data with" << _numRows << "points and" << _numDimensions << "dimensions.";
    }
}

QVariantMap PointData::toVariantMap() const
{
    const auto numberOfElements = getNumberOfElements();

    if (_isDense)
    {
        const auto typeSpecifier = getElementTypeSpecifier();
        const auto typeSpecifierName = getElementTypeNames()[static_cast<std::int32_t>(typeSpecifier)];
        const auto typeIndex = static_cast<std::int32_t>(typeSpecifier);

        QVariantMap rawData = rawDataToVariantMap((const char*)getDataConstVoidPtr(), getRawDataSize(), true);

        return {
            { "TypeIndex", QVariant::fromValue(typeIndex) },
            { "TypeName", QVariant(typeSpecifierName) },
            { "Raw", QVariant::fromValue(rawData) },
            { "NumberOfElements", QVariant::fromValue(numberOfElements) }
        };
    }
    else
    {
        std::vector<char> bytes;

        const std::vector<size_t>& indexPointers = _sparseData.getIndexPointers();
        const std::vector<uint32_t>& colIndices = _sparseData.getColIndices();
        const std::vector<float>& values = _sparseData.getValues();

        const char* indexPointersBytes = (const char*) (indexPointers.data());
        const char* colIndicesBytes = (const char*) (colIndices.data());
        const char* valuesBytes = (const char*) (values.data());

        bytes.insert(bytes.end(), indexPointersBytes, indexPointersBytes + indexPointers.size() * sizeof(size_t));
        bytes.insert(bytes.end(), colIndicesBytes, colIndicesBytes + colIndices.size() * sizeof(uint32_t));
        bytes.insert(bytes.end(), valuesBytes, valuesBytes + values.size() * sizeof(float));

        QVariantMap rawData = rawDataToVariantMap(bytes.data(), bytes.size(), true);

        return {
            { "Raw", QVariant::fromValue(rawData) }
        };
    }
}

void PointData::extractFullDataForDimension(std::vector<float>& result, const int dimensionIndex) const
{
    CheckDimensionIndex(dimensionIndex);

    result.resize(getNumPoints());

    std::visit(
        [&result, this, dimensionIndex](const auto& vec)
        {
            const auto resultSize = result.size();

            for (std::size_t i{}; i < resultSize; ++i)
                result[i] = vec[i * _numDimensions + dimensionIndex];

        },
        _variantOfVectors);
}


void PointData::extractFullDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const
{
    if (_isDense)
    {
        CheckDimensionIndex(dimensionIndex1);
        CheckDimensionIndex(dimensionIndex2);

        result.resize(getNumPoints());

        std::visit(
            [&result, this, dimensionIndex1, dimensionIndex2](const auto& vec)
            {
                const auto resultSize = result.size();

                for (std::size_t i{}; i < resultSize; ++i)
                {
                    const auto n = i * _numDimensions;
                    result[i].set(vec[n + dimensionIndex1], vec[n + dimensionIndex2]);
                }
            },
            _variantOfVectors);
    }
    else
    {
        result.resize(getNumPoints());

        std::vector<float> col1 = _sparseData.getDenseCol(dimensionIndex1);
        std::vector<float> col2 = _sparseData.getDenseCol(dimensionIndex2);

        for (size_t i = 0; i < result.size(); i++)
            result[i].set(col1[i], col2[i]);
    }
}


void PointData::extractDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, const std::vector<unsigned int>& indices) const
{
    CheckDimensionIndex(dimensionIndex1);
    CheckDimensionIndex(dimensionIndex2);

    result.resize(indices.size());

    std::visit(
        [&result, this, dimensionIndex1, dimensionIndex2, indices](const auto& vec)
        {
            const auto resultSize = result.size();

            for (std::size_t i{}; i < resultSize; ++i)
            {
                const auto n = std::size_t{ indices[i] } *_numDimensions;
                result[i].set(vec[n + dimensionIndex1], vec[n + dimensionIndex2]);
            }
        },
        _variantOfVectors);
}

Points::Points(QString dataName, bool mayUnderive /*= true*/, const QString& guid /*= ""*/) :
    mv::DatasetImpl(dataName, mayUnderive, guid),
    _infoAction(nullptr),
    _dimensionsPickerGroupAction(nullptr),
    _dimensionsPickerAction(nullptr)
{
}

Points::~Points()
{
}

void Points::init()
{
    DatasetImpl::init();

    _infoAction = new InfoAction(this, *this);

    const auto updateDimensionsPickerAction = [this]() -> void {
        _dimensionsPickerAction->setPointsDataset(*this);
    };

    //if (isFull()) {
        _dimensionsPickerGroupAction = new GroupAction(this, "Dimensions Group");

        _dimensionsPickerGroupAction->setText("Dimensions");
        _dimensionsPickerGroupAction->setShowLabels(false);

        _dimensionsPickerAction = new DimensionsPickerAction(_dimensionsPickerGroupAction, "Dimensions");

        _dimensionsPickerGroupAction->addAction(_dimensionsPickerAction);

        updateDimensionsPickerAction();

        connect(&getSmartPointer(), &Dataset<Points>::dataDimensionsChanged, this, updateDimensionsPickerAction);
    //}


    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetDataSelectionChanged));
    _eventListener.registerDataEventByType(PointType, [this](DatasetEvent* dataEvent)
    {
        switch (dataEvent->getType())
        {
            case EventType::DatasetDataSelectionChanged:
            {
                // Do not process our own selection changes
                if (dataEvent->getDataset() == Dataset<Points>(this))
                    return;

                // Only synchronize when dataset grouping is enabled and our own group index is non-negative
                if (!mv::data().getSelectionGroupingAction()->isChecked() || getGroupIndex() < 0)
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

                events().notifyDatasetDataSelectionChanged(this);

                break;
            }
        }
    });
}

// =============================================================================
// Point Set
// =============================================================================


void Points::setData(std::nullptr_t, const std::size_t numPoints, const std::size_t numDimensions)
{
    const auto notifyDimensionsChanged = numDimensions != getRawData<PointData>()->getNumDimensions();

    getRawData<PointData>()->setData(nullptr, numPoints, numDimensions);

    if (notifyDimensionsChanged)
        events().notifyDatasetDataDimensionsChanged(this);
}

void Points::extractDataForDimension(std::vector<float>& result, const int dimensionIndex) const
{
    // This overload assumes that the data set is "full".
    // Please remove the assert once non-full support is implemented (if necessary).
    assert(isFull());

    if (isProxy()) {
        result.resize(getNumPoints());

        auto pointIndexOffset = 0u;

        for (auto proxyMember : getProxyMembers()) {
            auto points = mv::Dataset<Points>(proxyMember);

            std::vector<float> proxyPoints;

            proxyPoints.resize(points->getNumPoints());

            points->extractDataForDimension(proxyPoints, dimensionIndex);

            std::copy(proxyPoints.begin(), proxyPoints.end(), result.begin() + pointIndexOffset);

            pointIndexOffset += points->getNumPoints();
        }
    }
    else {
        getRawData<PointData>()->extractFullDataForDimension(result, dimensionIndex);
    }
}

void Points::extractDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const
{
    if (isProxy()) {
        result.resize(getNumPoints());

        auto pointIndexOffset = 0u;

        for (auto proxyMember : getProxyMembers()) {
            auto points = mv::Dataset<Points>(proxyMember);

            std::vector<mv::Vector2f> proxyPoints;

            proxyPoints.resize(points->getNumPoints());

            points->extractDataForDimensions(proxyPoints, dimensionIndex1, dimensionIndex2);

            std::copy(proxyPoints.begin(), proxyPoints.end(), result.begin() + pointIndexOffset);

            pointIndexOffset += points->getNumPoints();
        }
    }
    else {
        const auto rawPointData = getRawData<PointData>();

        if (isFull())
            rawPointData->extractFullDataForDimensions(result, dimensionIndex1, dimensionIndex2);
        else
            rawPointData->extractDataForDimensions(result, dimensionIndex1, dimensionIndex2, indices);
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
    auto set = new Points(getRawDataName());

    set->setText(text());
    set->indices = indices;

    return set;
}

Dataset<DatasetImpl> Points::createSubsetFromSelection(const QString& guiName, const Dataset<DatasetImpl>& parentDataSet /*= Dataset<DatasetImpl>()*/, const bool& visible /*= true*/) const
{
    return mv::data().createSubsetFromSelection(getSelection(), toSmartPointer(), guiName, parentDataSet, visible);
}

Dataset<DatasetImpl> Points::createSubsetFromVisibleSelection(const QString& guiName, const Dataset<DatasetImpl>& parentDataSet /*= Dataset<DatasetImpl>()*/, const bool& visible /*= true*/) const
{
    Dataset<Points> subsetSelection = getSelection()->copy();
    std::vector<uint32_t>& selectionIndices = subsetSelection->getSelectionIndices();

    //// Get the global indices of the parent dataset
    //std::vector<uint32_t> globalIndices;
    //getGlobalIndices(globalIndices);

    //// Find the intersection of points that are selected, and the given indices
    //std::vector<uint32_t>& smallVector = (globalIndices.size() < selectionIndices.size()) ? globalIndices : selectionIndices;
    //std::vector<uint32_t>& largeVector = (globalIndices.size() < selectionIndices.size()) ? selectionIndices : globalIndices;
    //
    //// Sort the smallest vector
    //sort(smallVector.begin(), smallVector.end());

    //// Loop over large vector and binary search which values are in the smaller vector
    //std::vector<uint32_t> intersection;
    //std::copy_if(largeVector.begin(), largeVector.end(), std::back_inserter(intersection), [smallVector](uint32_t x)
    //{
    //    return std::binary_search(smallVector.begin(), smallVector.end(), x);
    //});

    // Compute the indices that are selected in this local dataset
    std::vector<uint32_t> localSelectionIndices;
    getLocalSelectionIndices(localSelectionIndices);

    // If we make a subset from a subset, take the locally selected points from the previous subset
    if (!isFull())
    {
        for (uint32_t& localIndex : localSelectionIndices)
            localIndex = indices[localIndex];
    }
    // If the data is full, then the locally selected points are the new subset

    subsetSelection->indices = localSelectionIndices;

    return mv::data().createSubsetFromSelection(subsetSelection, toSmartPointer(), guiName, parentDataSet, visible);
}

QIcon Points::getIcon(const QColor& color /*= Qt::black*/) const
{
    return mv::Application::getIconFont("FontAwesome").getIcon("database", color);

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

void Points::setProxyMembers(const Datasets& proxyMembers)
{
    DatasetImpl::setProxyMembers(proxyMembers);

    getTask().setName("Creating proxy");
    getTask().setVisible(!(projects().isOpeningProject() || projects().isImportingProject()));
    getTask().setRunning();

    auto pointIndexOffset = 0u;

    QCoreApplication::processEvents();

    for (auto proxyMember : getProxyMembers()) {
        auto targetPoints = Dataset<Points>(proxyMember);

        std::vector<std::uint32_t> targetGlobalIndices;

        targetPoints->getGlobalIndices(targetGlobalIndices);

        // Selection map from proxy to member
        {
            SelectionMap selectionMapToTarget;

            for (std::uint32_t pointIndex = 0; pointIndex < targetPoints->getNumPoints(); ++pointIndex)
                selectionMapToTarget.getMap()[pointIndexOffset + pointIndex] = std::vector<std::uint32_t>({ targetGlobalIndices[pointIndex] });

            addLinkedData(targetPoints, selectionMapToTarget);
        }

        // Selection map from member to proxy
        {
            SelectionMap selectionMapToSource;

            for (std::uint32_t pointIndex = 0; pointIndex < targetPoints->getNumPoints(); ++pointIndex)
                selectionMapToSource.getMap()[targetGlobalIndices[pointIndex]] = std::vector<std::uint32_t>({ pointIndexOffset + pointIndex });

            targetPoints->addLinkedData(toSmartPointer(), selectionMapToSource);

            if (!targetPoints->isFull())
                targetPoints->getFullDataset<Points>()->addLinkedData(toSmartPointer(), selectionMapToSource);
        }

        pointIndexOffset += targetPoints->getNumPoints();

        getTask().setProgressDescription(QString("Creating mappings for %1").arg(proxyMember->text()));
        getTask().setProgress(static_cast<float>(getProxyMembers().indexOf(proxyMember)) / static_cast<float>(getProxyMembers().count()));

        QCoreApplication::processEvents();
    }

    if (!getProxyMembers().isEmpty()) {
        setDimensionNames(Dataset<Points>(getProxyMembers().first())->getDimensionNames());
        events().notifyDatasetDataDimensionsChanged(this);
    }

    getTask().setFinished();
}

/* -------------------------------------------------------------------------- */
/*                            Index transformation                            */
/* -------------------------------------------------------------------------- */

void Points::getGlobalIndices(std::vector<unsigned int>& globalIndices) const
{
    if (isProxy())
    {
        globalIndices.resize(getNumPoints(), 0);
        std::iota(globalIndices.begin(), globalIndices.end(), 0);
        return;
    }

    // Traverse the chain of datasets back to the original source data
    // Any subsets traversed along the way are stored in the a subset chain
    std::vector<Dataset<Points>> subsetChain;
    {
        auto currentDataset = toSmartPointer<Points>();

        // Walk back in the chain of derived data until we find the original source
        while (currentDataset->isDerivedData())
        {
            // If the current set is a subset then store it on the stack to traverse later
            if (!currentDataset->isFull())
                subsetChain.push_back(currentDataset);

            currentDataset = currentDataset->getNextSourceDataset<Points>();
        }

        // We now have a non-derived dataset bound, push it if its also a subset
        if (!currentDataset->isFull())
            subsetChain.push_back(currentDataset);
    }

    // Find the original global indices of this dataset by transforming them
    // step by step traversing through the chain of subsets
    {
        globalIndices.resize(getNumPoints(), 0);
        std::iota(globalIndices.begin(), globalIndices.end(), 0);

        for (const Dataset<Points>& subset : subsetChain)
        {
            for (int i = 0; i < globalIndices.size(); i++)
                globalIndices[i] = subset->indices[globalIndices[i]];
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
    if (isProxy())
    {
        localSelectionIndices = getSelection<Points>()->indices;
        return;
    }


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

/* -------------------------------------------------------------------------- */
/*                               Action getters                               */
/* -------------------------------------------------------------------------- */

InfoAction& Points::getInfoAction()
{
    return *_infoAction;
}

mv::gui::GroupAction& Points::getDimensionsPickerGroupAction()
{
    if (!isFull())
        return getFullDataset<Points>()->getDimensionsPickerGroupAction();

    return *_dimensionsPickerGroupAction;
}

DimensionsPickerAction& Points::getDimensionsPickerAction()
{
    if (!isFull())
        return getFullDataset<Points>()->getDimensionsPickerAction();

	return *_dimensionsPickerAction;
}

/* -------------------------------------------------------------------------- */
/*                                 Selection                                  */
/* -------------------------------------------------------------------------- */

std::vector<std::uint32_t>& Points::getSelectionIndices()
{
    return getSelection<Points>()->indices;
}

const std::vector<QString>& Points::getDimensionNames() const
{
    if (isProxy()) {
        return mv::Dataset<Points>(getProxyMembers().first())->getDimensionNames();
    }
    else {
        return getRawData<PointData>()->getDimensionNames();
    }
}

void Points::setDimensionNames(const std::vector<QString>& dimNames)
{
    getRawData<PointData>()->setDimensionNames(dimNames);

    mv::events().notifyDatasetDataDimensionsChanged(this);
}

float Points::getValueAt(const std::size_t index) const
{
    return getRawData<PointData>()->getValueAt(index);
}

void Points::setValueAt(const std::size_t index, const float newValue)
{
    getRawData<PointData>()->setValueAt(index, newValue);
}

static void resolveLinkedPointData(const LinkedData& linkedData, const std::vector<std::uint32_t>& indices, Datasets* ignoreDatasets = nullptr)
{
    Dataset<Points> sourceDataset   = linkedData.getSourceDataSet();
    Dataset<Points> targetDataset   = linkedData.getTargetDataset();
    Dataset<Points> targetSelection = targetDataset->getSelection();

    if (sourceDataset->isLocked() || targetDataset->isLocked())
        return;

    Datasets notified;

    if (ignoreDatasets == nullptr)
        ignoreDatasets = &notified;

    // Do not notify if the dataset has already been notified
    if (ignoreDatasets->contains(targetDataset))
        return;

    //qDebug() << QString("%1, %2, %3").arg(__FUNCTION__, sourceDataset->getGuiName(), targetDataset->getGuiName());

    {
        //Timer timer("Creating maps");

        const SelectionMap& mapping = linkedData.getMapping();

        // Create separate vector of additional linked selected points
        std::vector<unsigned int> linkedIndices;

        // Reserve at least as much space as required for a 1-1 mapping
        linkedIndices.reserve(indices.size());

        for (const auto& selectionIndex : indices)
        {
            if (mapping.hasMappingForPointIndex(selectionIndex))
            {
                std::vector<std::uint32_t> mappedSelection;
                mapping.populateMappingIndices(selectionIndex, mappedSelection);
                linkedIndices.insert(linkedIndices.end(), mappedSelection.begin(), mappedSelection.end());
            }
        }

        if (targetDataset->isProxy()) {
            std::set<std::uint32_t> targetIndicesSet(targetSelection->indices.begin(), targetSelection->indices.end());

            for (auto& [key, value] : const_cast<SelectionMap&>(mapping).getMap())
                for (const auto& v : value)
                    targetIndicesSet.erase(v);

            for (const auto& linkedIndex : linkedIndices)
                targetIndicesSet.insert(linkedIndex);

            targetSelection->indices = std::vector<std::uint32_t>(targetIndicesSet.begin(), targetIndicesSet.end());
        }
        else {
            targetSelection->indices = linkedIndices;
        }
    }
    
    // Add the target of the linked data (of which we updated the selection indices) to the ignore list
    //*ignoreDatasets << sourceDataset << targetDataset;
    *ignoreDatasets << targetDataset;

    // Recursively resolve linked point data
    for (const mv::LinkedData& targetLd : targetDataset->getLinkedData())
        resolveLinkedPointData(targetLd, targetSelection->indices, ignoreDatasets);
}

void Points::resolveLinkedData(bool force /*= false*/)
{
    if (isLocked())
        return;

    // Check for linked data in this dataset and resolve them
    for (const mv::LinkedData& linkedData : getLinkedData())
        resolveLinkedPointData(linkedData, getSelection<Points>()->indices, nullptr);

    // Check for linked data in all source datasets and resolve them
    // This and all source data share the same selection indices
    mv::Dataset<Points> dataset = Dataset<Points>(this);
    while (dataset->isDerivedData())
    {
        dataset = dataset->getSourceDataset<Points>();

        if (dataset.isValid())
        {
            for (const mv::LinkedData& linkedData : dataset->getLinkedData())
                resolveLinkedPointData(linkedData, getSelection<Points>()->indices, nullptr);

        }
        else
            break;
    }

}

void Points::setSelectionIndices(const std::vector<std::uint32_t>& indices)
{
    //qDebug() << QString("%1, %2").arg(__FUNCTION__, getGuiName());

    if (isLocked())
        return;

    auto selection = getSelection<Points>();

    selection->indices = indices;

    resolveLinkedData();

    //events().notifyDatasetDataSelectionChanged(this);
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
    std::vector<unsigned int> selectionIndices;

    selectionIndices.resize(getNumPoints());

    if (isFull())
        std::iota(selectionIndices.begin(), selectionIndices.end(), 0);
    else
        selectionIndices = indices;

    setSelectionIndices(selectionIndices);

    events().notifyDatasetDataSelectionChanged(this);
}

void Points::selectNone()
{
    setSelectionIndices({});

    events().notifyDatasetDataSelectionChanged(this);
}

void Points::selectInvert()
{
    // Get the locally selected indices (the points in the subset that are selected)
    std::vector<unsigned int> localSelectionIndices;
    getLocalSelectionIndices(localSelectionIndices);

    // Compute the inverse of this
    const auto numberOfPoints = getNumPoints();
    std::set<std::uint32_t> selectionSet(localSelectionIndices.begin(), localSelectionIndices.end());

    std::vector<unsigned int> selectionIndices;
    selectionIndices.reserve(numberOfPoints - selectionSet.size());

    for (std::uint32_t i = 0; i < numberOfPoints; i++)
        if (selectionSet.find(i) == selectionSet.end())
            selectionIndices.push_back(i);

    // Convert the inverted indices back to global indices
    std::vector<unsigned int> globalIndices;
    getGlobalIndices(globalIndices);

    for (unsigned int& index : selectionIndices)
    {
        index = globalIndices[index];
    }

    setSelectionIndices(selectionIndices);

    events().notifyDatasetDataSelectionChanged(this);
}

void Points::fromVariantMap(const QVariantMap& variantMap)
{
    DatasetImpl::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "DimensionNames");
    variantMapMustContain(variantMap, "Selection");

    // For backwards compatibility, check PluginVersion
    if (variantMap["PluginVersion"] == "No Version" && !variantMap["Full"].toBool())
    {
        makeSubsetOf(getParent()->getFullDataset<mv::DatasetImpl>());

        qWarning() << "[ManiVault deprecation warning]: This project was saved with an older ManiVault version (<1.0). "
            "Please save the project again to ensure compatibility with newer ManiVault versions. "
            "Future releases may not be able to load this projects otherwise. ";
    }

    // Load raw point data
    if (isFull())
        getRawData<PointData>()->fromVariantMap(variantMap);
    else
    {
        variantMapMustContain(variantMap, "Indices");
    
        const auto& indicesMap = variantMap["Indices"].toMap();
    
        indices.resize(indicesMap["Count"].toInt());
    
        populateDataBufferFromVariantMap(indicesMap["Raw"].toMap(), (char*)indices.data());
    }

    // Load dimension names
    QStringList dimensionNameList;
    std::vector<QString> dimensionNames;

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

    if (variantMap["NumberOfDimensions"].toInt() > 1000)
        dimensionNameList = fetchDimensionNames();
    else
        dimensionNameList = variantMap["DimensionNames"].toStringList();

    if (dimensionNameList.size() == getNumDimensions())
    {
        for (const auto& dimensionName : dimensionNameList)
            dimensionNames.push_back(dimensionName);
    }
    else
    {
        for (std::uint32_t dimensionIndex = 0; dimensionIndex < getNumDimensions(); dimensionIndex++)
            dimensionNames.emplace_back(QString("Dim %1").arg(QString::number(dimensionIndex)));
    }

    setDimensionNames(dimensionNames);

    if (variantMap.contains("Dimensions")) {
        _dimensionsPickerAction->fromParentVariantMap(variantMap);
    }

    events().notifyDatasetDataChanged(this);

    // Handle saved selection
    if (isFull()) {
        const auto& selectionMap = variantMap["Selection"].toMap();

        const auto count = selectionMap["Count"].toInt();

        if (count > 0) {
            auto selectionSet = getSelection<Points>();

            selectionSet->indices.resize(count);

            populateDataBufferFromVariantMap(selectionMap["Raw"].toMap(), (char*)selectionSet->indices.data());

            events().notifyDatasetDataSelectionChanged(this);
        }
    }
}

QVariantMap Points::toVariantMap() const
{
    auto variantMap = DatasetImpl::toVariantMap();

    QStringList dimensionNames;
    QByteArray dimensionsByteArray;
    QDataStream dimensionsDataStream(&dimensionsByteArray, QIODevice::WriteOnly);

    if (getDimensionNames().size() == getNumDimensions()) {
        for (const auto& dimensionName : getDimensionNames())
            dimensionNames << dimensionName;
    }
    else {
        for (std::uint32_t dimensionIndex = 0; dimensionIndex < getNumDimensions(); dimensionIndex++)
            dimensionNames << QString("Dim %1").arg(QString::number(dimensionIndex));
    }

    if (dimensionNames.size() > 1000)
        dimensionsDataStream << dimensionNames;

    QVariantMap indices;

    indices["Count"]    = QVariant::fromValue(this->indices.size());
    indices["Raw"]      = rawDataToVariantMap((char*)this->indices.data(), this->indices.size() * sizeof(std::uint32_t), true);

    QVariantMap selection;

    if (isFull()) {
        auto selectionSet = getSelection<Points>();

        selection["Count"]  = QVariant::fromValue(selectionSet->indices.size());
        selection["Raw"]    = rawDataToVariantMap((char*)selectionSet->indices.data(), selectionSet->indices.size() * sizeof(std::uint32_t), true);
    }

    variantMap["Data"]                  = isFull() ? getRawData<PointData>()->toVariantMap() : QVariantMap();
    variantMap["NumberOfPoints"]        = getNumPoints();
    variantMap["Indices"]               = indices;
    variantMap["Selection"]             = selection;
    variantMap["DimensionNames"]        = (dimensionNames.size() > 1000) ? rawDataToVariantMap((char*)dimensionsByteArray.data(), dimensionsByteArray.size(), true) : QVariant::fromValue(dimensionNames);
    variantMap["NumberOfDimensions"]    = getNumDimensions();
    variantMap["Dimensions"]            = _dimensionsPickerAction->toVariantMap();

    variantMap["Dense"]                 = Experimental::isDense(this);

    if (!Experimental::isDense(this))
        variantMap["NumberOfNonZeroElements"] = QVariant::fromValue(Experimental::getNumNonZeroElements(this));
    
    return variantMap;
}

// =============================================================================
// Factory
// =============================================================================

QIcon PointDataFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("circle", color);
}

QUrl PointDataFactory::getReadmeMarkdownUrl() const
{
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/master/ManiVault/src/plugins/PointData/README.md");
}

QUrl PointDataFactory::getRepositoryUrl() const
{
    return QUrl("https://github.com/ManiVaultStudio/core");
}

mv::plugin::RawData* PointDataFactory::produce()
{
    return new PointData(this);
}
