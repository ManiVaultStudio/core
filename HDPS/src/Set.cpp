// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Set.h"

#include "AnalysisPlugin.h"
#include "CoreInterface.h"
#include "DataHierarchyItem.h"

#include "util/Exception.h"
#include "util/Icon.h"
#include "util/Serialization.h"

#include "Application.h"

using namespace mv::gui;
using namespace mv::util;

namespace mv
{

void DatasetImpl::makeSubsetOf(Dataset<DatasetImpl> fullDataset)
{
    _rawDataName = fullDataset->_rawDataName;

    if (!_rawDataName.isEmpty())
        _rawData = &Application::core()->requestRawData(getRawDataName());

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
    return dataHierarchy().getItem(getId());
}

mv::Dataset<mv::DatasetImpl> DatasetImpl::getParent() const
{
    return getDataHierarchyItem().getParent().getDataset();
}

QVector<Dataset<DatasetImpl>> DatasetImpl::getChildren(const QVector<DataType>& dataTypes /*= QVector<DataType>()*/) const
{
    QVector<Dataset<DatasetImpl>> children;

    for (auto dataHierarchyChild : getDataHierarchyItem().getChildren())
        if (dataTypes.contains(dataHierarchyChild->getDataType()))
            children << dataHierarchyChild->getDataset();

    return children;
}

QVector<mv::Dataset<mv::DatasetImpl>> DatasetImpl::getChildren(const DataType& filterDataType)
{
    return getChildren(QVector<DataType>({ filterDataType }));
}

std::int32_t DatasetImpl::getSelectionSize() const
{
    return static_cast<std::int32_t>(const_cast<DatasetImpl*>(this)->getSelectionIndices().size());
}

void DatasetImpl::lock()
{
    _locked = true;

    events().notifyDatasetLocked(toSmartPointer());

    emit getDataHierarchyItem().lockedChanged(_locked);
}

void DatasetImpl::unlock()
{
    _locked = false;

    events().notifyDatasetUnlocked(toSmartPointer());

    emit getDataHierarchyItem().lockedChanged(_locked);
}

bool DatasetImpl::isLocked() const
{
    return _locked;
}

void DatasetImpl::setLocked(bool locked)
{
    if (locked)
        lock();
    else
        unlock();
}

void DatasetImpl::setAnalysis(plugin::AnalysisPlugin* analysis)
{
    _analysis = analysis;
}

mv::plugin::AnalysisPlugin* DatasetImpl::getAnalysis()
{
    return _analysis;
}

void DatasetImpl::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Name");
    variantMapMustContain(variantMap, "Locked");
    variantMapMustContain(variantMap, "Full");
    variantMapMustContain(variantMap, "Derived");
    variantMapMustContain(variantMap, "LinkedData");
    variantMapMustContain(variantMap, "SourceDatasetGUID");
    variantMapMustContain(variantMap, "FullDatasetGUID");
    variantMapMustContain(variantMap, "GroupIndex");

    setText(variantMap["Name"].toString());
    setLocked(variantMap["Locked"].toBool());

    if (variantMap["Derived"].toBool())
    {
        setSourceDataSet(mv::data().getSet(variantMap["SourceDatasetGUID"].toString());

        assert(_sourceDataset.isValid());
    }

    if (!variantMap["Full"].toBool())
    {
        makeSubsetOf(mv::data().getSet(variantMap["FullDatasetGUID"].toString()));

        assert(variantMap["PluginKind"].toString() == _rawData->getKind());
        assert(variantMap["PluginVersion"].toString() == _rawData->getVersion());

        assert(_fullDataset.isValid());
    }

    assert(variantMap["DataType"].toString() == getDataType().getTypeString());

    if (variantMap["GroupIndex"].toInt() >= 0)
        setGroupIndex(variantMap["GroupIndex"].toInt());

    setStorageType(static_cast<StorageType>(variantMap["StorageType"].toInt()));

    if (getStorageType() == StorageType::Proxy && variantMap.contains("ProxyMembers")) {
        Datasets proxyMembers;

        for (const auto& proxyMemberGuid : variantMap["ProxyMembers"].toStringList())
            proxyMembers << Application::core()->requestDataset(proxyMemberGuid);

        setProxyMembers(proxyMembers);
    }

    for (const auto& linkedDataVariant : variantMap["LinkedData"].toList()) {
        LinkedData linkedData;

        linkedData.fromVariantMap(linkedDataVariant.toMap());

        _linkedData.push_back(linkedData);
    }
}

QVariantMap DatasetImpl::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    QStringList proxyMemberGuids;

    for (const auto& proxyMember : _proxyMembers)
        proxyMemberGuids << proxyMember->getId();

    QVariantList linkedDataList;

    for (const auto& ld : _linkedData)
        linkedDataList.push_back(ld.toVariantMap());

    variantMap.insert({
        { "Name", QVariant::fromValue(text()) },
        { "Locked", QVariant::fromValue(_locked) },
        { "StorageType", QVariant::fromValue(static_cast<std::int32_t>(getStorageType())) },
        { "ProxyMembers", QVariant::fromValue(proxyMemberGuids) },
        { "DataType", QVariant::fromValue(getDataType().getTypeString()) },
        { "PluginKind", QVariant::fromValue(_rawData->getKind()) },
        { "PluginVersion", QVariant::fromValue(_rawData->getVersion()) },
        { "Derived", QVariant::fromValue(isDerivedData()) },
        { "Full", QVariant::fromValue(isFull()) },
        { "SourceDatasetGUID", isDerivedData() ? QVariant::fromValue(_sourceDataset->getId()) : "" },
        { "FullDatasetGUID", isFull() ? "" : QVariant::fromValue(_fullDataset->getId()) },
        { "GroupIndex", QVariant::fromValue(getGroupIndex()) },
        { "LinkedData", linkedDataList }
    });

    return variantMap;
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

DatasetImpl::DatasetImpl(CoreInterface* core, const QString& rawDataName, const QString& id /*= ""*/) :
    WidgetAction(nullptr, "Set"),
    _core(core),
    _storageType(StorageType::Owner),
    _rawData(nullptr),
    _rawDataName(rawDataName),
    _all(true),
    _derived(false),
    _sourceDataset(),
    _properties(),
    _groupIndex(-1),
    _analysis(nullptr),
    _linkedData(),
    _linkedDataFlags(LinkedDataFlag::SendReceive),
    _locked(false),
    _smartPointer(this),
    _task(this, "")
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

mv::DataType DatasetImpl::getDataType() const
{
    return Application::core()->requestRawData(getRawDataName()).getDataType();
}

void DatasetImpl::setSourceDataSet(const Dataset<DatasetImpl>& dataset)
{
    _sourceDataset = dataset;
    _derived = true;
}

mv::Dataset<mv::DatasetImpl> DatasetImpl::getSelection() const
{
    return Application::core()->requestSelection(getSourceDataset<DatasetImpl>()->getRawDataName());
}

mv::Dataset<mv::DatasetImpl>& DatasetImpl::getSmartPointer()
{
    return _smartPointer;
}

mv::Dataset<mv::DatasetImpl> DatasetImpl::toSmartPointer() const
{
    return Dataset<DatasetImpl>(const_cast<DatasetImpl*>(this));
}

QIcon DatasetImpl::getIcon(StorageType storageType, const QColor& color /*= Qt::black*/) const
{
    switch (storageType)
    {
        case StorageType::Owner:
        case StorageType::Proxy:
            return getIcon(color);

        default:
            break;
    }

    return QIcon();
}

const std::vector<mv::LinkedData>& DatasetImpl::getLinkedData() const
{
    return _linkedData;
}

std::vector<mv::LinkedData>& DatasetImpl::getLinkedData()
{
    return _linkedData;
}

std::int32_t DatasetImpl::getLinkedDataFlags()
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

bool DatasetImpl::hasLinkedDataFlag(std::int32_t linkedDataFlag)
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

QString DatasetImpl::getLocation() const
{
    return getDataHierarchyItem().getLocation();
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
