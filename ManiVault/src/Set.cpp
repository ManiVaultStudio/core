// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Set.h"

#include "AnalysisPlugin.h"
#include "CoreInterface.h"
#include "DataHierarchyItem.h"
#include "PropertiesSerializer.h"

#include "util/Exception.h"
#include "util/Serialization.h"


#include <algorithm>

#include "SetLegacySerialization.h"

using namespace mv::gui;
using namespace mv::util;
using namespace mv::workflow;

namespace mv
{

namespace
{
    /** Utility class for managing workflow context in a thread-safe manner */
    class ToVariantMapWorkflowContext final : public workflow::WorkflowContextBase
    {
    public:

        /**
         * @brief Gets the indices from the context.
         * @return The indices stored in the context
         */
        QVariantMap getResult() const {
            QMutexLocker locker(&_mutex);
            return _result;
        }

        /**
         * @brief Gets a specific value from the result map of the context using the provided key.
         * @param key The key to look up in the result map
         * @return The value associated with the provided key in the result map, or an invalid QVariant if the key does not exist
         */
        QVariant getResultValue(const QString& key) const {
            QMutexLocker locker(&_mutex);
            return _result.value(key);
        }

        /**
         * @brief Sets a key-value pair in the result map of the context.
         * @param key The key to be set in the result map
         * @param value The value to be associated with the key in the result map
         */
        void setResultValue(const QString& key, const QVariant& value) {
            QMutexLocker locker(&_mutex);
            _result.insert(key, value);
        }

    private:
        mutable QMutex      _mutex;     /** Mutex for synchronizing access to the context */
        QVariantMap         _result;    /** The QVariantMap that will hold the final result of the workflow execution, containing the serialized headers and indices data. This map will be returned at the end of the workflow execution and can be used for further processing or storage. */
    };
}

void DatasetImpl::makeSubsetOf(Dataset<DatasetImpl> fullDataset)
{
    _rawDataName = fullDataset->_rawDataName;

    if (!_rawDataName.isEmpty())
        _rawData = mv::data().getRawData(getRawDataName());

    _fullDataset = fullDataset;

    setAll(false);
}

QString DatasetImpl::getRawDataKind() const
{
    return _rawData->getKind();
}

std::uint64_t DatasetImpl::getRawDataSize() const
{
    return 0;
}

const DataHierarchyItem& DatasetImpl::getDataHierarchyItem() const
{
    return const_cast<DatasetImpl*>(this)->getDataHierarchyItem();
}

DataHierarchyItem& DatasetImpl::getDataHierarchyItem()
{
    return *dataHierarchy().getItem(getId());
}

mv::Dataset<mv::DatasetImpl> DatasetImpl::getParent() const
{
    if (!getDataHierarchyItem().hasParent())
        return {};

    return getDataHierarchyItem().getParent()->getDataset();
}

QVector<Dataset<DatasetImpl>> DatasetImpl::getChildren(const QVector<DataType>& dataTypes /*= QVector<DataType>()*/, bool recursively /*= true*/) const
{
    QVector<Dataset<DatasetImpl>> children;

    for (auto dataHierarchyChild : getDataHierarchyItem().getChildren(recursively)) {
        if (dataTypes.isEmpty()) {
            children << dataHierarchyChild->getDataset();
        }
        else {
            if (dataTypes.contains(dataHierarchyChild->getDataset()->getDataType()))
                children << dataHierarchyChild->getDataset();
        }
    }

    return children;
}

QVector<mv::Dataset<mv::DatasetImpl>> DatasetImpl::getChildren(const DataType& filterDataType) const
{
    return getChildren(QVector<DataType>({ filterDataType }));
}

std::int32_t DatasetImpl::getSelectionSize() const
{
    return static_cast<std::int32_t>(const_cast<DatasetImpl*>(this)->getSelectionIndices().size());
}

void DatasetImpl::lock(bool cache /*= false*/)
{
    if (cache)
        _lockedCache = _locked;

    _locked = true;

    events().notifyDatasetLocked(toSmartPointer());

    emit getDataHierarchyItem().lockedChanged(_locked);
}

void DatasetImpl::unlock(bool cache /*= false*/)
{
    if (cache)
        _lockedCache = _locked;

    _locked = false;

    events().notifyDatasetUnlocked(toSmartPointer());

    emit getDataHierarchyItem().lockedChanged(_locked);
}

bool DatasetImpl::isLocked() const
{
    return _locked;
}

void DatasetImpl::setLocked(bool locked, bool cache /*= false*/)
{
    if (cache)
        _lockedCache = _locked;

    if (locked)
        lock();
    else
        unlock();
}

void DatasetImpl::restoreLockedFromCache()
{
    setLocked(_lockedCache);
}

void DatasetImpl::setAnalysis(plugin::AnalysisPlugin* analysis)
{
    _analysis = analysis;
}

mv::plugin::AnalysisPlugin* DatasetImpl::getAnalysis()
{
    return _analysis;
}

UniqueWorkflowPlan DatasetImpl::fromVariantMapWorkflow(QVariantMap variantMap)
{
    auto plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    plan->addNestedWorkflowStage("Load widget action", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) mutable -> UniqueWorkflowPlan {
	    return this->WidgetAction::fromVariantMapWorkflow(variantMap);
    });

    const auto appVersion = mv::projects().getCurrentProject()->getApplicationVersionAction().getVersion();

    if (appVersion < Version(1, 5, 0)) {
        plan->addSequentialStage("Load legacy points (< 1.5.0)", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
            legacy::SetLegacySerializer::fromVariantMapPre150(*this, variantMap);
        }, WorkflowPlan::JobThreadAffinity::GuiThread);

        return plan;
    }

    plan->addNestedWorkflowStage("Load properties", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) mutable -> UniqueWorkflowPlan {
        return PropertiesSerializer::fromVariantMapWorkflow(variantMap["Properties"].toMap(), _properties, executionContext);
    });

    plan->addSequentialStage("Load", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
        variantMapMustContain(variantMap, "Name");
        variantMapMustContain(variantMap, "Locked");
        variantMapMustContain(variantMap, "StorageType");
        variantMapMustContain(variantMap, "DataType");
        variantMapMustContain(variantMap, "Derived");
        variantMapMustContain(variantMap, "LinkedData");

        setText(variantMap["Name"].toString());
        setLocked(variantMap["Locked"].toBool());
        setStorageType(static_cast<StorageType>(variantMap["StorageType"].toInt()));

        assert(variantMap["DataType"].toString() == getDataType().getTypeString());

        if (variantMap["Derived"].toBool())
        {
            if (variantMap.contains("SourceDatasetID"))
                setSourceDataset(mv::data().getDataset(variantMap["SourceDatasetID"].toString()));
            else
                setSourceDataset(getParent());

            assert(_sourceDataset.isValid());
        }

        // For backwards compatibility, check PluginVersion
        if (!(variantMap["PluginVersion"] == "No Version") && !variantMap["Full"].toBool())
        {
            if (variantMap.contains("FullDatasetID"))
                makeSubsetOf(mv::data().getDataset(variantMap["FullDatasetID"].toString()));
            else
                makeSubsetOf(getParent()->getFullDataset<mv::DatasetImpl>());

            assert(variantMap["PluginKind"].toString() == _rawData->getKind());

            assert(_fullDataset.isValid());
        }

        if (variantMap.contains("GroupIndex") && variantMap["GroupIndex"].toInt() >= 0)
            setGroupIndex(variantMap["GroupIndex"].toInt());

        if (variantMap.contains("MayUnderive"))
            _mayUnderive = variantMap["MayUnderive"].toBool();

        if (getStorageType() == StorageType::Proxy && variantMap.contains("ProxyMembers")) {
            Datasets proxyMembers;

            for (const auto& proxyMemberGuid : variantMap["ProxyMembers"].toStringList())
                proxyMembers << mv::data().getDataset(proxyMemberGuid);

            setProxyMembers(proxyMembers);
        }

        for (const auto& linkedDataVariant : variantMap["LinkedData"].toList()) {
            LinkedData linkedData;

            linkedData.fromVariantMap(linkedDataVariant.toMap());

            _linkedData.push_back(linkedData);
        }
    });

    return plan;
}

UniqueWorkflowPlan DatasetImpl::toVariantMapWorkflow() const
{
    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    const auto saveActionStage = plan->addNestedWorkflowStage("Save action", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> UniqueWorkflowPlan {
        return this->WidgetAction::toVariantMapWorkflow();
    });

    const auto savePropertiesStage = plan->addNestedWorkflowStage("Save properties", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> UniqueWorkflowPlan {
        return PropertiesSerializer::toVariantMapWorkflow(_properties);
    });

    plan->addSequentialStage("Save map", [this, saveActionStage, savePropertiesStage](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) -> void {
        auto resultMap      = executionContext->takeOutput(saveActionStage).toMap();
        auto propertiesMap  = executionContext->takeOutput(savePropertiesStage).toMap();

        QStringList proxyMemberGuids;

        for (const auto& proxyMember : _proxyMembers)
            proxyMemberGuids << proxyMember->getId();

        QVariantList linkedDataList;

        for (const auto& linkedData : _linkedData)
            linkedDataList.push_back(linkedData.toVariantMap());

        resultMap.insert({
	        { "Name", QVariant::fromValue(text()) },
	        { "Locked", QVariant::fromValue(_locked) },
	        { "StorageType", QVariant::fromValue(static_cast<std::int32_t>(getStorageType())) },
	        { "ProxyMembers", QVariant::fromValue(proxyMemberGuids) },
	        { "DataType", QVariant::fromValue(getDataType().getTypeString()) },
	        { "PluginKind", QVariant::fromValue(_rawData->getKind()) },
	        { "PluginVersion", QVariant::fromValue(_rawData->getVersion()) },
	        { "Derived", QVariant::fromValue(isDerivedData()) },
	        { "MayUnderive", QVariant::fromValue(mayUnderive()) },
	        { "Full", QVariant::fromValue(isFull()) },
	        { "SourceDatasetID", isDerivedData() ? QVariant::fromValue(_sourceDataset->getId()) : "" },
	        { "FullDatasetID", isFull() ? "" : QVariant::fromValue(_fullDataset->getId()) },
	        { "GroupIndex", QVariant::fromValue(getGroupIndex()) },
	        { "LinkedData", linkedDataList },
	        { "Properties", propertiesMap }
        });

        executionContext->setOutput(resultMap);
	});

    return plan;
}

std::int32_t DatasetImpl::getGroupIndex() const
{
    return _groupIndex;
}

void DatasetImpl::setGroupIndex(const std::int32_t& groupIndex)
{
    _groupIndex = groupIndex;

    events().notifyDatasetDataSelectionChanged(this);
}

mv::Datasets DatasetImpl::getProxyMembers() const
{
    return _proxyMembers;
}

void DatasetImpl::setProxyMembers(const Datasets& proxyDatasets)
{
    try
    {
        if (proxyDatasets.isEmpty()) {
            setStorageType(StorageType::Owner);
            throw std::runtime_error("Proxy dataset requires at least one input dataset");
        }

        for (const auto& proxyDataset : proxyDatasets)
            if (proxyDataset->getDataType() != getDataType())
                throw std::runtime_error("All datasets should be of the same data type");

        _proxyMembers = proxyDatasets;

        setStorageType(StorageType::Proxy);

        events().notifyDatasetDataChanged(this);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to set proxy datasets for " + text(), e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to set proxy datasets for " + text());
    }
}

bool DatasetImpl::mayProxy(const Datasets& proxyDatasets) const
{
    for (const auto& proxyDataset : proxyDatasets)
        if (proxyDataset->getDataType() != getDataType())
            return false;

    return true;
}

bool DatasetImpl::isProxy() const
{
    return !_proxyMembers.isEmpty();
}

void DatasetImpl::addAction(mv::gui::WidgetAction& widgetAction)
{
    widgetAction.setParent(this);

    getDataHierarchyItem().addAction(widgetAction);
}

mv::gui::WidgetActions DatasetImpl::getActions() const
{
    return getDataHierarchyItem().getActions();
}

QMenu* DatasetImpl::getContextMenu(QWidget* parent /*= nullptr*/)
{
    return getDataHierarchyItem().getContextMenu(parent);
}

void DatasetImpl::populateContextMenu(QMenu* contextMenu)
{
    return getDataHierarchyItem().populateContextMenu(contextMenu);
}

void DatasetImpl::addLinkedData(const mv::Dataset<DatasetImpl>& targetDataSet, mv::SelectionMap& mapping)
{
    _linkedData.emplace_back(toSmartPointer(), targetDataSet);
    _linkedData.back().setMapping(mapping);
}

void DatasetImpl::addLinkedData(const mv::Dataset<DatasetImpl>& targetDataSet, mv::SelectionMap&& mapping)
{
    _linkedData.emplace_back(toSmartPointer(), targetDataSet);
    _linkedData.back().setMapping(std::move(mapping));
}

void DatasetImpl::removeAllLinkedData()
{
    _linkedData.clear();
}

void DatasetImpl::removeLinkedDataset(const mv::Dataset<DatasetImpl>& targetDataSet)
{
    // Erase-remove idiom (https://en.wikibooks.org/wiki/More_C++_Idioms/Erase-Remove) 
    // Removes all mappings to targetDataSet from _linkedData
    _linkedData.erase(std::remove_if(_linkedData.begin(), _linkedData.end(), [targetDataSet](const mv::LinkedData& linkedSel) {
        return linkedSel.getTargetDataset() == targetDataSet;
    }), _linkedData.end());
}

void DatasetImpl::removeLinkedDataMapping(const QString& mappingID)
{
    // Removes specific mapping from _linkedData
    _linkedData.erase(std::remove_if(_linkedData.begin(), _linkedData.end(), [mappingID](const mv::LinkedData& linkedSel) {
        return linkedSel.getId() == mappingID;
    }), _linkedData.end());
}

DatasetImpl::DatasetImpl(const QString& rawDataName, bool mayUnderive /*= true*/, const QString& id /*= ""*/) :
    WidgetAction(nullptr, "Set"),
    _storageType(StorageType::Owner),
    _rawData(nullptr),
    _rawDataName(rawDataName),
    _all(true),
    _derived(false),
    _groupIndex(-1),
    _analysis(nullptr),
    _linkedDataFlags(LinkedDataFlag::SendReceive),
    _locked(false),
    _lockedCache(false),
    _smartPointer(this),
    _task(this, ""),
    _mayUnderive(mayUnderive),
    _aboutToBeRemoved(false),
    _dirtySelection(false)
{
    if (!id.isEmpty())
        Serializable::setId(id);

    _task.setDataset(this);
}

DatasetImpl::~DatasetImpl()
{
}

void DatasetImpl::init()
{
}

mv::DatasetImpl::StorageType DatasetImpl::getStorageType() const
{
    return _storageType;
}

void DatasetImpl::setStorageType(const StorageType& storageType)
{
    if (storageType == _storageType)
        return;

    _storageType = storageType;
}

QString DatasetImpl::getGuiName() const
{
    return text();
}

void DatasetImpl::setGuiName(const QString& guiName)
{
    setText(guiName);
}

//void DatasetImpl::setText(const QString& text)
//{
//    WidgetAction::setText(text);
//
//    _datasetTask.setName(text);
//}

bool DatasetImpl::isFull() const
{
    return _all;
}

bool DatasetImpl::isDerivedData() const
{
    return _derived;
}

bool DatasetImpl::mayUnderive() const
{
    return _mayUnderive;
}

mv::DataType DatasetImpl::getDataType() const
{
    return mv::data().getRawData(getRawDataName())->getDataType();
}

void DatasetImpl::setSourceDataset(Dataset<DatasetImpl> dataset)
{
    _sourceDataset = dataset;
    _derived = _sourceDataset.isValid();
}

void DatasetImpl::setSourceDataset(const QString& datasetId)
{
    _sourceDataset._datasetId = datasetId;
    _derived = true;
}

mv::Dataset<mv::DatasetImpl> DatasetImpl::getSelection() const
{
    return mv::data().getSelection(getSourceDataset<DatasetImpl>()->getRawDataName());
}

mv::Dataset<mv::DatasetImpl>& DatasetImpl::getSmartPointer()
{
    return _smartPointer;
}

mv::Dataset<mv::DatasetImpl> DatasetImpl::toSmartPointer() const
{
    return { const_cast<DatasetImpl*>(this) };
}

const std::vector<mv::LinkedData>& DatasetImpl::getLinkedData() const
{
    return _linkedData;
}

std::vector<mv::LinkedData>& DatasetImpl::getLinkedData()
{
    return _linkedData;
}

std::int32_t DatasetImpl::getLinkedDataFlags() const
{
    return _linkedDataFlags;
}

void DatasetImpl::setLinkedDataFlags(std::int32_t linkedDataFlags)
{
    _linkedDataFlags = linkedDataFlags;
}

void DatasetImpl::setLinkedDataFlag(std::int32_t linkedDataFlag, bool set /*= true*/)
{
    if (set)
        _linkedDataFlags |= linkedDataFlag;
    else
        _linkedDataFlags &= ~linkedDataFlag;
}

bool DatasetImpl::hasLinkedDataFlag(std::int32_t linkedDataFlag) const
{
    return _linkedDataFlags & linkedDataFlag;
}

QString DatasetImpl::getRawDataSizeHumanReadable() const
{
    return util::getNoBytesHumanReadable(getRawDataSize());
}

DatasetTask& DatasetImpl::getTask()
{
    return _task;
}

void DatasetImpl::setAboutToBeRemoved(bool aboutToBeRemoved /*= true*/)
{
    _aboutToBeRemoved = aboutToBeRemoved;

    getDataHierarchyItem().deselect();
}

bool DatasetImpl::isAboutToBeRemoved() const
{
    return _aboutToBeRemoved;
}

QString DatasetImpl::getLocation(bool recompute /*= false*/) const
{
    return getDataHierarchyItem().getLocation(recompute);
}

QVariant DatasetImpl::getProperty(const QString& name, const QVariant& defaultValue /*= QVariant()*/) const
{
    if (!hasProperty(name))
        return defaultValue;

    return _properties[name];
}

void DatasetImpl::setProperty(const QString& name, const QVariant& value)
{
    _properties[name] = value;
}

bool DatasetImpl::hasProperty(const QString& name) const
{
    return _properties.contains(name);
}

QStringList DatasetImpl::propertyNames() const
{
    return _properties.keys();
}

QString DatasetImpl::getRawDataName() const
{
    return _rawDataName;
}

void DatasetImpl::setAll(bool all)
{
    _all = all;
}

}
