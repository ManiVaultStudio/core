// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PointData.h"
#include "PointDataLegacySerialization.h"

#include "DimensionsPickerAction.h"
#include "InfoAction.h"
#include "DimensionNamesSerializer.h"

#include <Application.h>

#include <actions/GroupAction.h>
#include <event/Event.h>
#include <graphics/Vector2f.h>

#include <util/Serialization.h>

#include <workflow/WorkflowContextVariantMap.h>

#include <QDebug>
#include <QtCore>

#include <cstring>
#include <set>
#include <type_traits>

Q_PLUGIN_METADATA(IID "studio.manivault.PointData")

// =============================================================================
// PointData
// =============================================================================

using namespace mv;
using namespace mv::plugin;
using namespace mv::util;
using namespace mv::workflow;

namespace
{
    namespace local
    {
        template<typename outT, typename inT>
        inline outT safe_numeric_cast(const inT value)
        {
            static_assert(std::is_integral_v<inT> && std::is_integral_v<outT>);

            assert(value >= std::numeric_limits<outT>::min() && value <= std::numeric_limits<outT>::max());
            return static_cast<outT>(value);
        }
    }

}

PointData::ElementTypeSpecifier PointData::elementTypeSpecifier(const QString& typeName)
{
    if (typeName == "float32")
        return ElementTypeSpecifier::float32;
    else if (typeName == "bfloat16")
        return ElementTypeSpecifier::bfloat16;
    else if (typeName == "int32")
        return ElementTypeSpecifier::int32;
    else if (typeName == "uint32")
        return ElementTypeSpecifier::uint32;
    else if (typeName == "int16")
        return ElementTypeSpecifier::int16;
    else if (typeName == "uint16")
        return ElementTypeSpecifier::uint16;
    else if (typeName == "int8")
        return ElementTypeSpecifier::int8;
    else if (typeName == "uint8")
        return ElementTypeSpecifier::uint8;

    return ElementTypeSpecifier::float32; // Default to float32 if the type name is not recognized
}

QString PointData::elementTypeName(const ElementTypeSpecifier elementTypeSpecifier)
{
    switch (elementTypeSpecifier) 
    {
        case ElementTypeSpecifier::float32:
            return "float32";
        case ElementTypeSpecifier::bfloat16:
            return "bfloat16";
        case ElementTypeSpecifier::int32:
            return "int32";
        case ElementTypeSpecifier::uint32:
            return "uint32";
        case ElementTypeSpecifier::int16:
            return "int16";
        case ElementTypeSpecifier::uint16:
            return "uint16";
        case ElementTypeSpecifier::int8:
            return "int8";
        case ElementTypeSpecifier::uint8:
            return "uint8";
        default:
            return "unknown";
    }

    return "unknown";
}

void PointData::init()
{
}

mv::Dataset<DatasetImpl> PointData::createDataSet(const QString& guid /*= ""*/) const
{
    return mv::Dataset<DatasetImpl>(new Points(getName(), true, guid));
}

std::uint64_t PointData::getNumPoints() const
{
    if (_numDimensions == 0)
    {
        qWarning() << "Number of dimensions is 0 in point data ";
        return 0;
    }

    if (_isDense)
        return static_cast<std::uint64_t>(getSizeOfVector() / _numDimensions);
    else
        return _numRows;
}

std::uint64_t PointData::getNumDimensions() const
{
    return _numDimensions;
}

std::uint64_t PointData::getNumberOfElements() const
{
    return getSizeOfVector();
}


std::uint64_t PointData::getRawDataSize() const
{
    if (_isDense)
    {
        std::uint64_t elementSize = std::visit([](auto& vec) { return vec.empty() ? 0u : sizeof(vec[0]); }, _variantOfVectors);
        return elementSize * getNumberOfElements();
    }
    else
    {
        return static_cast<std::uint64_t>(_numRows + 4) * sizeof(size_t) + _sparseData.getNumNonZeros() * (sizeof(size_t) + sizeof(float));
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
    _numDimensions = static_cast<std::uint64_t>(numDimensions);
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

UniqueWorkflowPlan PointData::fromVariantMapWorkflow(QVariantMap variantMap)
{
    //Plugin::fromVariantMap(variantMap);

    const auto appVersion = mv::projects().getCurrentProject()->getApplicationVersionAction().getVersion();

    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(QString("%1(%2)").arg(getSerializationName()).arg(__FUNCTION__));

    if (appVersion < Version(1, 5, 0)) {
        plan->addSequentialStage("Load legacy point data (< 1.5.0)", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
            legacy::PointDataLegacySerializer::fromVariantMapPre150(*this, variantMap, executionContext);
        });

        return plan;
    }

    plan->addSequentialStage("Allocate storage", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
        variantMapMustContain(variantMap, "Data");
        variantMapMustContain(variantMap, "NumberOfPoints");
        variantMapMustContain(variantMap, "NumberOfDimensions");

        const auto dataMap              = variantMap["Data"].toMap();
        const auto numberOfPoints       = static_cast<std::size_t>(variantMap["NumberOfPoints"].toULongLong());
        const auto numberOfDimensions   = static_cast<std::size_t>(variantMap["NumberOfDimensions"].toULongLong());
        const auto numberOfElements     = numberOfPoints * numberOfDimensions;
        const auto elementTypeIndex     = dataMap.contains("TypeName") ? elementTypeSpecifier(dataMap.value("TypeName").toString()) : static_cast<ElementTypeSpecifier>(dataMap.value("TypeIndex").toInt());

        _isDense        = variantMap.value("Dense", true).toBool();
        _numDimensions  = numberOfDimensions;

        setElementTypeSpecifier(elementTypeIndex);
        resizeVector(numberOfElements);
    });

    plan->addNestedWorkflowStage("Populate data", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) -> UniqueWorkflowPlan {
        if (getNumberOfElements() == 0) {
            return std::make_unique<WorkflowPlan>(QString("%1(%2) - no data to populate").arg(getSerializationName()).arg(__FUNCTION__));
        }

    	const auto dataMap = variantMap["Data"].toMap();
        const auto rawDataMap = dataMap["Raw"].toMap();

        return populateBytesFromBlobMapWorkflow(rawDataMap, static_cast<char*>(getDataVoidPtr()), getRawDataSize(), executionContext->getExecutionOptions());
	});

    return plan;
}

UniqueWorkflowPlan PointData::toVariantMapWorkflow() const
{
    auto plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    //const auto baseSaveStage = plan->addNestedWorkflowStage("Save raw data base", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> UniqueWorkflowPlan {
    //    return this->Plugin::toVariantMapWorkflow();
    //});

    plan->addSequentialStage("Save raw data", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
        QVariantMap outputMap;

        if (_isDense) {
            const auto typeSpecifier        = getElementTypeSpecifier();
            const auto typeSpecifierName    = getElementTypeNames()[static_cast<std::int32_t>(typeSpecifier)];
            const auto typeIndex            = static_cast<std::int32_t>(typeSpecifier);

            outputMap.insert("TypeIndex", QVariant::fromValue(typeIndex));
            outputMap.insert("TypeName", QVariant(typeSpecifierName));
            outputMap.insert("Raw", QVariant::fromValue(bytesToBlobVariantMap(static_cast<const char*>(getDataConstVoidPtr()), getRawDataSize())));
            outputMap.insert("NumberOfElements", QVariant::fromValue(getNumberOfElements()));

            const auto expectedBytes = getRawDataSize();
            const auto blobTotalSize = outputMap["Raw"].toMap()["Size"].toULongLong();

            if (blobTotalSize != expectedBytes) {
                throw ManiVaultException(
                    SeverityLevel::Error,
                    "PointData blob size does not match declared point-data dimensions",
                    QString("Dataset '%1' declares %2 points x %3 dimensions = %4 bytes, but blob total size is %5 bytes")
                    .arg(getGuiName())
                    .arg(getNumPoints())
                    .arg(getNumDimensions())
                    .arg(expectedBytes)
                    .arg(blobTotalSize),
                    __FUNCTION__
                );
            }
        }
        else {
            std::vector<char> bytes;

            const auto& indexPointers   = _sparseData.getIndexPointers();
            const auto& colIndices      = _sparseData.getColIndices();
            const auto& values          = _sparseData.getValues();

            const auto* indexPointersBytes  = reinterpret_cast<const char*>(indexPointers.data());
            const auto* colIndicesBytes     = reinterpret_cast<const char*>(colIndices.data());
            const auto* valuesBytes         = reinterpret_cast<const char*>(values.data());

            bytes.insert(bytes.end(), indexPointersBytes, indexPointersBytes + indexPointers.size() * sizeof(size_t));
            bytes.insert(bytes.end(), colIndicesBytes, colIndicesBytes + colIndices.size() * sizeof(size_t));
            bytes.insert(bytes.end(), valuesBytes, valuesBytes + values.size() * sizeof(float));

            outputMap.insert("Raw", bytesToBlobVariantMap(bytes.data(), bytes.size()));
        }

        executionContext->setOutput(outputMap);
    });

    return plan;
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


void PointData::extractDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, const std::vector<std::uint32_t>& indices) const
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
    DatasetImpl(dataName, mayUnderive, guid),
    _infoAction(nullptr),
    _dimensionsPickerGroupAction(nullptr),
    _dimensionsPickerAction(nullptr)
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
        _dimensionsPickerGroupAction = new gui::GroupAction(this, "Dimensions Group");

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

void Points::handleNumberDimensionsChanged(std::size_t newNumDimensions) {
    std::vector<QString> dimensionNames = getDimensionNames();
    std::size_t oldDimSize = dimensionNames.size();
    dimensionNames.resize(newNumDimensions, "");

    for (std::size_t addedDimension = oldDimSize; addedDimension < newNumDimensions; addedDimension++) {
        dimensionNames[addedDimension] = QString("Dim %1").arg(addedDimension + 1);
    }

    setDimensionNames(dimensionNames);  // calls notifyDatasetDataDimensionsChanged
}

void Points::setData(std::nullptr_t, const std::size_t numPoints, const std::size_t numDimensions)
{
    const auto numDimensionsChanged = numDimensions != getRawData<PointData>()->getNumDimensions();

    getRawData<PointData>()->setData(nullptr, numPoints, numDimensions);

    if (numDimensionsChanged)
        handleNumberDimensionsChanged(numDimensions);
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

    std::uint64_t pointIndexOffset = 0lu;

    QCoreApplication::processEvents();

    for (auto proxyMember : getProxyMembers()) {
        auto targetPoints = Dataset<Points>(proxyMember);

        std::vector<std::uint32_t> targetGlobalIndices;

        targetPoints->getGlobalIndices(targetGlobalIndices);

        // Selection map from proxy to member
        {
            SelectionMap selectionMapToTarget;

            for (std::uint64_t pointIndex = 0; pointIndex < targetPoints->getNumPoints(); ++pointIndex)
                selectionMapToTarget.getMap()[pointIndexOffset + pointIndex] = std::vector<std::uint32_t>({ targetGlobalIndices[pointIndex] });

            addLinkedData(targetPoints, selectionMapToTarget);
        }

        // Selection map from member to proxy
        {
            SelectionMap selectionMapToSource;

            for (std::uint64_t pointIndex = 0; pointIndex < targetPoints->getNumPoints(); ++pointIndex)
                selectionMapToSource.getMap()[targetGlobalIndices[pointIndex]] = std::vector<std::uint32_t>({ ::local::safe_numeric_cast<std::uint32_t>(pointIndexOffset + pointIndex) });

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

void Points::getGlobalIndices(std::vector<std::uint32_t>& globalIndices) const
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
            for (std::uint64_t i = 0; i < globalIndices.size(); i++)
                globalIndices[i] = subset->indices[globalIndices[i]];
        }
    }
}

void Points::selectedLocalIndices(const std::vector<std::uint32_t>& selectionIndices, std::vector<bool>& selected) const
{
    //Timer timer(__FUNCTION__);

    // Find the global indices of this dataset
    std::vector<std::uint32_t> localGlobalIndices;
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

        for (const std::uint32_t& selectionIndex : selectionIndices)
            globalSelection[selectionIndex] = true;

        // For all local points find out which are selected
        selected.resize(localGlobalIndices.size(), false);
        for (std::uint64_t i = 0; i < localGlobalIndices.size(); i++)
        {
            if (globalSelection[localGlobalIndices[i]])
                selected[i] = true;
        }
    }
}

void Points::getLocalSelectionIndices(std::vector<std::uint32_t>& localSelectionIndices) const
{
    if (isProxy())
    {
        localSelectionIndices = getSelection<Points>()->indices;
        return;
    }


    auto selection = getSelection<Points>();

    // Find the global indices of this dataset
    std::vector<std::uint32_t> localGlobalIndices;
    getGlobalIndices(localGlobalIndices);

    // In an array the size of the full raw data, mark selected points as true
    std::vector<bool> globalSelection(getSourceDataset<Points>()->getNumRawPoints(), false);
    for (const std::uint32_t& selectionIndex : selection->indices)
        globalSelection[selectionIndex] = true;

    // For all local points find out which are selected
    std::vector<bool> selected(localGlobalIndices.size(), false);
    int indexCount = 0;
    for (std::uint64_t i = 0; i < localGlobalIndices.size(); i++)
    {
        if (globalSelection[localGlobalIndices[i]])
        {
            selected[i] = true;
            indexCount++;
        }
    }

    localSelectionIndices.resize(indexCount);
    std::uint64_t c = 0;
    for (std::uint64_t i = 0; i < selected.size(); i++)
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
    assert(dimNames.size() == getRawData<PointData>()->getNumDimensions());

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
        std::vector<std::uint32_t> linkedIndices;

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
    std::vector<std::uint32_t> selectionIndices;

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
    std::vector<std::uint32_t> localSelectionIndices;
    getLocalSelectionIndices(localSelectionIndices);

    // Compute the inverse of this
    const auto numberOfPoints = getNumPoints();
    std::set<std::uint32_t> selectionSet(localSelectionIndices.begin(), localSelectionIndices.end());

    std::vector<std::uint32_t> selectionIndices;
    selectionIndices.reserve(numberOfPoints - selectionSet.size());

    for (std::uint64_t i = 0; i < numberOfPoints; i++)
        if (selectionSet.find(i) == selectionSet.end())
            selectionIndices.push_back(i);

    // Convert the inverted indices back to global indices
    std::vector<std::uint32_t> globalIndices;
    getGlobalIndices(globalIndices);

    for (std::uint32_t& index : selectionIndices)
    {
        index = globalIndices[index];
    }

    setSelectionIndices(selectionIndices);

    events().notifyDatasetDataSelectionChanged(this);
}

UniqueWorkflowPlan Points::fromVariantMapWorkflow(QVariantMap variantMap)
{
    auto plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    plan->addNestedWorkflowStage("Load dataset base", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) -> UniqueWorkflowPlan {
        return this->DatasetImpl::fromVariantMapWorkflow(variantMap);
    });

    const auto appVersion = mv::projects().getCurrentProject()->getApplicationVersionAction().getVersion();

    if (appVersion < Version(1, 5, 0)) {
        plan->addSequentialStage("Load legacy points raw data (< 1.5.0)", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
            legacy::PointsLegacySerializer::fromVariantMapPre150(*this, variantMap, executionContext);
        }, WorkflowPlan::JobThreadAffinity::GuiThread);

        return plan;
    }

    if (isFull()) {
        plan->addNestedWorkflowStage("Load raw data", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) mutable -> UniqueWorkflowPlan {
            return getRawData<PointData>()->fromVariantMapWorkflow(variantMap);
        });

        plan->addSequentialStage("Load selection", [this, variantMap](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& executionContext) {
            variantMapMustContain(variantMap, "Selection");

            const auto& selectionMap = variantMap["Selection"].toMap();

            const auto count = selectionMap["Count"].toUInt();

            if (count > 0) {
                auto selectionSet = getSelection<Points>();

                selectionSet->indices.resize(count);

                populateBytesFromBlobMap(selectionMap["Raw"].toMap(), (char*)selectionSet->indices.data(), count * sizeof(uint32_t));

                events().notifyDatasetDataSelectionChanged(this);
            }
        }, WorkflowPlan::JobThreadAffinity::GuiThread);
    }
    else {
        plan->addSequentialStage("Load indices", [this, variantMap](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& executionContext) {
            variantMapMustContain(variantMap, "Indices");

	        const auto& indicesMap = variantMap["Indices"].toMap();

	        indices.resize(indicesMap["Count"].toUInt());

	        populateBytesFromBlobMap(indicesMap["Raw"].toMap(), (char*)indices.data(), indices.size() * sizeof(uint32_t));
        }, WorkflowPlan::JobThreadAffinity::GuiThread);
    }

    const auto serializeDimensionsStage = plan->addNestedWorkflowStage("Load dimensions", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> UniqueWorkflowPlan {
        return DimensionNamesSerializer::fromVariantMapWorkflow(this, variantMap);
    });

    return plan;
}

UniqueWorkflowPlan Points::toVariantMapWorkflow() const
{
    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    const auto saveDatasetBaseStage = plan->addNestedWorkflowStage("Save dataset base", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> UniqueWorkflowPlan {
        return this->DatasetImpl::toVariantMapWorkflow();
    });

    WorkflowHandle encodeRawDataStage;

    if (isFull()) {
        encodeRawDataStage = plan->addNestedWorkflowStage("Encode raw data", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> UniqueWorkflowPlan {
            return getRawData<PointData>()->toVariantMapWorkflow();
        });
    }

    const auto storeRawDataStage = plan->addSequentialStage("Store raw data", [this, saveDatasetBaseStage, encodeRawDataStage](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) -> void {
        auto datasetMap = executionContext->takeOutput(saveDatasetBaseStage).toMap();

    	datasetMap["Data"]                      = isFull() ? executionContext->takeOutput(encodeRawDataStage).toMap() : QVariantMap();
        datasetMap["NumberOfPoints"]            = QVariant::fromValue<std::uint64_t>(getNumPoints());
        datasetMap["Dense"]                     = Experimental::isDense(this);
        datasetMap["NumberOfNonZeroElements"]   = QVariant::fromValue(Experimental::getNumNonZeroElements(this));

        executionContext->setOutput(datasetMap);
    });

    const auto storeIndicesStage = plan->addSequentialStage("Save indices", [this, storeRawDataStage](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& executionContext) {
        auto datasetMap = executionContext->takeOutput(storeRawDataStage).toMap();

        QVariantMap indicesMap;

        indicesMap["Count"] = QVariant::fromValue(this->indices.size());
        indicesMap["Raw"]   = bytesToBlobVariantMap((char*)this->indices.data(), this->indices.size() * sizeof(std::uint32_t));

        datasetMap["Indices"] = indicesMap;

        executionContext->setOutput(datasetMap);
    });

    const auto saveSelectionStage = plan->addSequentialStage("Save selection", [this, storeIndicesStage](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& executionContext) {
        auto datasetMap = executionContext->takeOutput(storeIndicesStage).toMap();

        QVariantMap selection;

        if (isFull()) {
            auto selectionSet = getSelection<Points>();

            selection["Count"]  = QVariant::fromValue(selectionSet->indices.size());
            selection["Raw"]    = bytesToBlobVariantMap((char*)selectionSet->indices.data(), selectionSet->indices.size() * sizeof(std::uint32_t));
        }

        datasetMap["Selection"] = selection;

        executionContext->setOutput(datasetMap);
    });

    const auto serializeDimensionsStage = plan->addNestedWorkflowStage("Serialize dimensions", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> UniqueWorkflowPlan {
        return DimensionNamesSerializer::toVariantMapWorkflow(getRawData<PointData>()->getDimensionNames());
    });

    const auto saveDimensionsStage = plan->addSequentialStage("Save dimensions", [this, saveSelectionStage, serializeDimensionsStage](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& executionContext) {
        auto datasetMap = executionContext->takeOutput(saveSelectionStage).toMap();

        datasetMap["DimensionNames"]        = executionContext->takeOutput(serializeDimensionsStage).toMap();
        datasetMap["NumberOfDimensions"]    = QVariant::fromValue<std::uint64_t>(getNumDimensions());
        datasetMap["Dimensions"]            = _dimensionsPickerAction->toVariantMap();

        executionContext->setOutput(datasetMap);
    });

    return plan;
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
