// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PointData.h"

#include "DimensionsPickerAction.h"
#include "InfoAction.h"

#include <Application.h>

#include <actions/GroupAction.h>
#include <event/Event.h>
#include <graphics/Vector2f.h>
#include <util/Serialization.h>

#include <QDebug>
#include <QtCore>

#include <cstring>
#include <set>
#include <type_traits>
#include <QMetaType>

Q_PLUGIN_METADATA(IID "studio.manivault.PointData")




// =============================================================================
// PointData
// =============================================================================

using namespace mv::util;

namespace internal
{
    template<typename T>
    std::size_t rawVectorSize(std::vector<T> v)
    {
        return (v.size() * sizeof(T));
    }
}


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

mv::point_size_t PointData::getNumPoints() const
{
    return _numRows;
}

mv::dimension_size_t PointData::getNumDimensions() const
{
    return _numDimensions;
}

mv::element_size_t PointData::getNumberOfElements() const
{
    return (_numRows * _numDimensions);
}


std::uint64_t PointData::getRawDataSize() const
{
    return std::visit([](const auto& matrix) -> std::uint64_t
        {
            return matrix.bytes();
        }, _variantOfMatrices);
}

void* PointData::getDataVoidPtr()
{
    const std::string mesg = __FUNCSIG__ + std::string(" is not implemented");
    throw std::exception(mesg.c_str());
}

const void* PointData::getDataConstVoidPtr() const
{
    const std::string mesg = __FUNCSIG__ + std::string(" is not implemented");
    throw std::exception(mesg.c_str());
}

const std::vector<QString>& PointData::getDimensionNames() const
{
    return _dimNames;
}

void PointData::setData(const std::nullptr_t, const std::size_t numPoints, const std::size_t numDimensions)
{
    assert(false);
    const std::string mesg = __FUNCSIG__ + std::string(" is not implemented");
    throw std::exception(mesg.c_str());
    resizeVector(numPoints * numDimensions);
    _numRows = numPoints;
    _numDimensions = static_cast<unsigned int>(numDimensions);
}

QMap<QString, QStringList> PointData::getSupportedStorageTypes() 
{
    static QMap<QString, QStringList> supportedStorageTypes;
    if (supportedStorageTypes.isEmpty())
    {
        constexpr auto matrixTypeNames = VariantOfMatrices::getMatrixTypeNames();
        constexpr auto elementTypeNames = VariantOfMatrices::getElementTypeNames();
        static_assert(matrixTypeNames.size() == elementTypeNames.size());


        const std::size_t size = elementTypeNames.size();
        for (std::size_t i = 0; i < size; ++i)
        {
            QString elementTypeName = elementTypeNames[i];
            QString matrixTypeName = matrixTypeNames[i];
            if (!supportedStorageTypes.contains(matrixTypeName))
            {
                supportedStorageTypes[matrixTypeName] = StorageConfiguration::getListOfSpecialDataTypeNames();
            }
            supportedStorageTypes[matrixTypeName].append(elementTypeName);
        }
    }
    return supportedStorageTypes;
}

void PointData::setDimensionNames(const std::vector<QString>& dimNames)
{
    if (dimNames.empty())
        return;

    _dimNames = dimNames;

    if (dimNames.size() != _numDimensions)
        qWarning() << "PointData: Number of dimension names does not equal the number of data dimensions";
}

float PointData::getValueAt(const mv::element_size_t index) const
{
    std::size_t row = index / _numDimensions;
    std::size_t column = index % _numDimensions;
    return getValueAt(row, column);
}

float PointData::getValueAt(mv::point_size_t point_idx, mv::dimension_size_t dim_idx) const
{

    return std::visit([point_idx, dim_idx](const auto& matrix) -> float
        {
            return static_cast<float>(matrix(point_idx, dim_idx));
        }, _variantOfMatrices);
}

void PointData::setValueAt(const mv::element_size_t index, const float newValue)
{
    std::size_t row = index / _numDimensions;
    std::size_t column = index % _numDimensions;
    setValueAt(row, column, newValue);
}

void PointData::setValueAt(mv::point_size_t point_idx, mv::dimension_size_t dim_idx, const float newValue)
{
    std::visit([point_idx, dim_idx, newValue](auto& matrix)
        {
            matrix(point_idx, dim_idx) = newValue;
        }, _variantOfMatrices);
}

void PointData::fromVariantMap(const QVariantMap& variantMap)
{
    if (variantMap.contains("MatrixType") && variantMap.contains("ElementType"))
    {
        // new data storage format
        variantMapMustContain(variantMap, "Version");
        const auto version = variantMap["Version"].toUInt();

        assert(version == 1); // currently we only have version 1
       
        _variantOfMatrices.fromVariantMap(variantMap);
        mv::dimension_size_t numberOfDimensions = 0;
        mv::point_size_t numberOfPoints = 0;
        std::visit([&numberOfPoints, &numberOfDimensions](const auto& matrix)
            {
                numberOfPoints = matrix.rows(); 
                numberOfDimensions = matrix.columns();
            }, _variantOfMatrices);
        _numRows = numberOfPoints;
        _numDimensions = numberOfDimensions;
        _isDense = false;
        
    }
    else
    {
        // the old format
        variantMapMustContain(variantMap, "Data");
        variantMapMustContain(variantMap, "NumberOfPoints");
        variantMapMustContain(variantMap, "NumberOfDimensions");

        const auto data = variantMap["Data"].toMap();
        const auto numberOfPoints = static_cast<size_t>(variantMap["NumberOfPoints"].toInt());
        const auto numberOfDimensions = variantMap["NumberOfDimensions"].toUInt();
        const auto numberOfElements = numberOfPoints * numberOfDimensions;
        const auto elementTypeIndex = data["TypeIndex"].toInt();
        const auto rawData = data["Raw"].toMap();

        bool isDense = true;
        if (variantMap.contains("Dense"))
            isDense = variantMap["Dense"].toBool();;

        _isDense = isDense;
        _numDimensions = numberOfDimensions;
        _numRows = numberOfPoints;

        if (_isDense)
        {
           
            _variantOfMatrices.setIndexOfVariant(elementTypeIndex);
            std::visit([numberOfPoints, numberOfDimensions, &rawData](auto& matrix)
                {
                    matrix.resize(numberOfPoints, numberOfDimensions);
                    // next steps can also be done differently
                    std::vector<char> buffer(matrix.bytes());
                    populateDataBufferFromVariantMap(rawData, buffer.data());
                    matrix.fromRawDataBuffer(buffer);
                }, _variantOfMatrices);
        }
        else
        {
           // _variantOfMatrices.setAsCSRMatrixOfElementType(static_cast<std::size_t>(elementTypeIndex));

            variantMapMustContain(variantMap, "NumberOfNonZeroElements");

            const auto numberOfNonZeroElements = variantMap["NumberOfNonZeroElements"].toULongLong();

            std::vector<char> buffer((numberOfPoints + 1) * sizeof(size_t) + numberOfNonZeroElements * (sizeof(size_t) + sizeof(float)));

            populateDataBufferFromVariantMap(rawData, buffer.data());

            std::visit([numberOfPoints, numberOfDimensions, numberOfNonZeroElements, &buffer](auto& matrix)
                {
                    matrix.resize(numberOfPoints, numberOfDimensions, numberOfNonZeroElements);
                    assert(buffer.size() == matrix.bytes());
                    matrix.fromRawDataBuffer(buffer);
                }, _variantOfMatrices);


           

            qDebug() << "Loaded sparse data with" << _numRows << "points and" << _numDimensions << "dimensions.";
        }
    }
    
}

QVariantMap PointData::toVariantMap() const
{
    return this->_variantOfMatrices.toVariantMap();
}

void PointData::extractFullDataForDimension(std::vector<float>& result, const int dimensionIndex, mv::DatasetTask* task) const
{
    CheckDimensionIndex(dimensionIndex);

    const PointData* ptr = this;
    std::visit([&result, dimensionIndex, task](const auto& matrix)
        {
            matrix.extractFullDataForDimension(result, dimensionIndex, task);
        }, _variantOfMatrices);
}


void PointData::extractFullDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, mv::DatasetTask* task) const
{
    CheckDimensionIndex(dimensionIndex1);
    CheckDimensionIndex(dimensionIndex2);

    std::visit([&result, dimensionIndex1, dimensionIndex2,task](const auto& matrix)
        {
            matrix.extractFullDataForDimensions(result, dimensionIndex1, dimensionIndex2, task);
        }, _variantOfMatrices);
}


void PointData::extractDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, const std::vector<unsigned int>& indices, mv::DatasetTask* task) const
{
    CheckDimensionIndex(dimensionIndex1);
    CheckDimensionIndex(dimensionIndex2);

    std::visit([&result, dimensionIndex1, dimensionIndex2, indices, task](const auto& matrix)
        {
            matrix.extractDataForDimensions(result, dimensionIndex1, dimensionIndex2, indices, task);
        }, _variantOfMatrices);
}

DatasetTask* Points::getDatasetTaskPtr(const QString& description) const
{
    if (description.isEmpty())
        return nullptr;

    DatasetTask& task = const_cast<Points*>(this)->getTask();
    task.setName(description);
    task.setProgressDescription(description);
    task.setProgressMode(Task::ProgressMode::Subtasks);
    task.setRunning();
    return &task;
}


Points::Points(QString dataName, bool mayUnderive /*= true*/, const QString& guid /*= ""*/) :
    DatasetImpl(dataName, mayUnderive, guid),
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

    setIconByName("database");
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


QMap<QString, QStringList> Points::getSupportedStorageTypes()
{
    return PointData::getSupportedStorageTypes();
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
float  Points::getValueAt(mv::point_size_t row, mv::dimension_size_t column) const
{
    return getRawData<PointData>()->getValueAt(row,column);
}

void Points::setValueAt(mv::point_size_t row, mv::dimension_size_t column, float newValue)
{
    getRawData<PointData>()->setValueAt(row,column, newValue);
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

    variantMap["Data"] = getRawData<PointData>()->toVariantMap();
    
    return variantMap;
}

// =============================================================================
// Factory
// =============================================================================

PointDataFactory::PointDataFactory()
{
    setIconByName("database");
    setCategoryIconByName("database");
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
