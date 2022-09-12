#include "Set.h"
#include "DataHierarchyItem.h"
#include "AnalysisPlugin.h"

#include <util/Serialization.h>
#include <util/Icon.h>

#include <QPainter>

using namespace hdps::gui;

namespace hdps
{

void DatasetImpl::makeSubsetOf(Dataset<DatasetImpl> fullDataset)
{
    _rawDataName = fullDataset->_rawDataName;

    if (!_rawDataName.isEmpty())
        _rawData = &_core->requestRawData(getRawDataName());

    _fullDataset = fullDataset;

    setAll(false);
}

QString DatasetImpl::getRawDataKind() const
{
    return _rawData->getKind();
}

const DataHierarchyItem& DatasetImpl::getDataHierarchyItem() const
{
    return const_cast<DatasetImpl*>(this)->getDataHierarchyItem();
}

DataHierarchyItem& DatasetImpl::getDataHierarchyItem()
{
    return _core->getDataHierarchyItem(_guid);
}

hdps::Dataset<hdps::DatasetImpl> DatasetImpl::getParent() const
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

std::int32_t DatasetImpl::getSelectionSize() const
{
    return static_cast<std::int32_t>(const_cast<DatasetImpl*>(this)->getSelectionIndices().size());
}

void DatasetImpl::lock()
{
    setLocked(true);
}

void DatasetImpl::unlock()
{
    setLocked(false);
}

bool DatasetImpl::isLocked() const
{
    return getDataHierarchyItem().getLocked();
}

void DatasetImpl::setLocked(bool locked)
{
    getDataHierarchyItem().setLocked(locked);
}

void DatasetImpl::setAnalysis(plugin::AnalysisPlugin* analysis)
{
    _analysis = analysis;
}

hdps::plugin::AnalysisPlugin* DatasetImpl::getAnalysis()
{
    return _analysis;
}

void DatasetImpl::fromVariantMap(const QVariantMap& variantMap)
{
    variantMapMustContain(variantMap, "Name");
    variantMapMustContain(variantMap, "GUID");
    variantMapMustContain(variantMap, "Derived");
    variantMapMustContain(variantMap, "HasAnalysis");
    variantMapMustContain(variantMap, "Analysis");

    setGuiName(variantMap["Name"].toString());
    
    _guid = variantMap["GUID"].toString();

    if (variantMap.contains("Derived")) {
        _derived = variantMap["Derived"].toBool();

        if (_derived)
            _sourceDataset = getParent();
    }

    if (variantMap.contains("StorageType"))
        setStorageType(static_cast<StorageType>(variantMap["StorageType"].toInt()));

    if (getStorageType() == StorageType::Proxy && variantMap.contains("ProxyMembers")) {
        Datasets proxyMembers;

        for (const auto& proxyMemberGuid : variantMap["ProxyMembers"].toStringList())
            proxyMembers << _core->requestDataset(proxyMemberGuid);

        setProxyMembers(proxyMembers);
    }
}

QVariantMap DatasetImpl::toVariantMap() const
{
    QVariantMap analysisMap;

    if (_analysis)
        analysisMap = _analysis->toVariantMap();

    QStringList proxyMemberGuids;

    for (auto proxyMember : _proxyMembers)
        proxyMemberGuids << proxyMember->getGuid();

    return {
        { "Name", QVariant::fromValue(getGuiName()) },
        { "GUID", QVariant::fromValue(getGuid()) },
        { "StorageType", QVariant::fromValue(static_cast<std::int32_t>(getStorageType())) },
        { "ProxyMembers", QVariant::fromValue(proxyMemberGuids) },
        { "DataType", QVariant::fromValue(getDataType().getTypeString()) },
        { "PluginKind", QVariant::fromValue(_rawData->getKind()) },
        { "PluginVersion", QVariant::fromValue(_rawData->getVersion()) },
        { "Derived", QVariant::fromValue(isDerivedData()) },
        { "GroupIndex", QVariant::fromValue(getGroupIndex()) },
        { "HasAnalysis", QVariant::fromValue(_analysis != nullptr) },
        { "Analysis", analysisMap }
    };
}

std::int32_t DatasetImpl::getGroupIndex() const
{
    return _groupIndex;
}

void DatasetImpl::setGroupIndex(const std::int32_t& groupIndex)
{
    _groupIndex = groupIndex;

    _core->notifyDatasetSelectionChanged(this);
}

hdps::Datasets DatasetImpl::getProxyMembers() const
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

        for (auto proxyDataset : proxyDatasets)
            if (proxyDataset->getDataType() != getDataType())
                throw std::runtime_error("All datasets should be of the same data type");

        _proxyMembers = proxyDatasets;

        setStorageType(StorageType::Proxy);

        _core->notifyDatasetChanged(this);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to set proxy datasets for " + getGuiName(), e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to set proxy datasets for " + getGuiName());
    }
}

bool DatasetImpl::mayProxy(const Datasets& proxyDatasets) const
{
    for (auto proxyDataset : proxyDatasets)
        if (proxyDataset->getDataType() != getDataType())
            return false;

    return true;
}

bool DatasetImpl::isProxy() const
{
    return !_proxyMembers.isEmpty();
}

void DatasetImpl::addAction(hdps::gui::WidgetAction& widgetAction)
{
    // Re-parent the widget action
    widgetAction.setParent(this);

   // And add to the data hierarchy item
    getDataHierarchyItem().addAction(widgetAction);
}

hdps::gui::WidgetActions DatasetImpl::getActions() const
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

void DatasetImpl::addLinkedData(const hdps::Dataset<DatasetImpl>& targetDataSet, hdps::SelectionMap& mapping)
{
    _linkedData.emplace_back(toSmartPointer(), targetDataSet);
    _linkedData.back().setMapping(mapping);
}

hdps::DatasetImpl::StorageType DatasetImpl::getStorageType() const
{
    return _storageType;
}

void DatasetImpl::setStorageType(const StorageType& storageType)
{
    if (storageType == _storageType)
        return;

    _storageType = storageType;
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

const std::vector<hdps::LinkedData>& DatasetImpl::getLinkedData() const
{
    return _linkedData;
}

std::vector<hdps::LinkedData>& DatasetImpl::getLinkedData()
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

    _linkedDataAction.getMayReceiveAction().setChecked(hasLinkedDataFlag(LinkedDataFlag::Receive));
    _linkedDataAction.getMaySendAction().setChecked(hasLinkedDataFlag(LinkedDataFlag::Send));
}

bool DatasetImpl::hasLinkedDataFlag(std::int32_t linkedDataFlag)
{
    return _linkedDataFlags & linkedDataFlag;
}

}
