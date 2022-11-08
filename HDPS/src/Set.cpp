#include "Set.h"
#include "DataHierarchyItem.h"
#include "AnalysisPlugin.h"

#include <Serialization.h>
#include <util/Icon.h>

#include <QPainter>

using namespace hdps::gui;

namespace hdps
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
    return Application::core()->getDataHierarchyItem(_guid);
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

QVector<hdps::Dataset<hdps::DatasetImpl>> DatasetImpl::getChildren(const DataType& filterDataType)
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

    Application::core()->notifyDatasetLocked(toSmartPointer());

    emit getDataHierarchyItem().lockedChanged(_locked);
}

void DatasetImpl::unlock()
{
    _locked = false;

    Application::core()->notifyDatasetUnlocked(toSmartPointer());

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

hdps::plugin::AnalysisPlugin* DatasetImpl::getAnalysis()
{
    return _analysis;
}

void DatasetImpl::fromVariantMap(const QVariantMap& variantMap)
{
    variantMapMustContain(variantMap, "Name");
    variantMapMustContain(variantMap, "Locked");
    variantMapMustContain(variantMap, "GUID");
    variantMapMustContain(variantMap, "Derived");
    variantMapMustContain(variantMap, "HasAnalysis");
    variantMapMustContain(variantMap, "Analysis");
    variantMapMustContain(variantMap, "LinkedData");

    setGuiName(variantMap["Name"].toString());
    setLocked(variantMap["Locked"].toBool());

    _guid       = variantMap["GUID"].toString();
    _derived    = variantMap["Derived"].toBool();

    if (_derived)
        _sourceDataset = getParent();

    setStorageType(static_cast<StorageType>(variantMap["StorageType"].toInt()));

    if (getStorageType() == StorageType::Proxy && variantMap.contains("ProxyMembers")) {
        Datasets proxyMembers;

        for (const auto& proxyMemberGuid : variantMap["ProxyMembers"].toStringList())
            proxyMembers << Application::core()->requestDataset(proxyMemberGuid);

        setProxyMembers(proxyMembers);
    }

    for (auto linkedDataVariant : variantMap["LinkedData"].toList()) {
        LinkedData linkedData;

        linkedData.fromVariantMap(linkedDataVariant.toMap());

        getLinkedData().push_back(linkedData);
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

    QVariantList linkedData;

    for (const auto& ld : getLinkedData())
        linkedData.push_back(ld.toVariantMap());

    return {
        { "Name", QVariant::fromValue(getGuiName()) },
        { "Locked", QVariant::fromValue(_locked) },
        { "GUID", QVariant::fromValue(getGuid()) },
        { "StorageType", QVariant::fromValue(static_cast<std::int32_t>(getStorageType())) },
        { "ProxyMembers", QVariant::fromValue(proxyMemberGuids) },
        { "DataType", QVariant::fromValue(getDataType().getTypeString()) },
        { "PluginKind", QVariant::fromValue(_rawData->getKind()) },
        { "PluginVersion", QVariant::fromValue(_rawData->getVersion()) },
        { "Derived", QVariant::fromValue(isDerivedData()) },
        { "GroupIndex", QVariant::fromValue(getGroupIndex()) },
        { "HasAnalysis", QVariant::fromValue(_analysis != nullptr) },
        { "Analysis", analysisMap },
        { "LinkedData", linkedData }
    };
}

std::int32_t DatasetImpl::getGroupIndex() const
{
    return _groupIndex;
}

void DatasetImpl::setGroupIndex(const std::int32_t& groupIndex)
{
    _groupIndex = groupIndex;

    Application::core()->notifyDatasetSelectionChanged(this);
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

        Application::core()->notifyDatasetChanged(this);
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
    widgetAction.setParent(this);

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

DatasetImpl::DatasetImpl(CoreInterface* core, const QString& rawDataName, const QString& guid /*= ""*/) :
    WidgetAction(nullptr),
    _storageType(StorageType::Owner),
    _rawData(nullptr),
    _guid(guid.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces) : guid),
    _guiName(),
    _rawDataName(rawDataName),
    _all(false),
    _derived(false),
    _sourceDataset(),
    _properties(),
    _groupIndex(-1),
    _analysis(nullptr),
    _linkedData(),
    _linkedDataFlags(LinkedDataFlag::SendReceive),
    _locked(false),
    _smartPointer(this)
{

}

DatasetImpl::~DatasetImpl()
{
#ifdef DATASET_IMPL_VERBOSE
    qDebug() << _guiName << "destructed";
#endif
}

void DatasetImpl::init()
{

}

QString DatasetImpl::getGuid() const
{
    return _guid;
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

QString DatasetImpl::getGuiName() const
{
    return _guiName;
}

void DatasetImpl::setGuiName(const QString& guiName)
{
    const auto previousGuiName = _guiName;

    _guiName = guiName;

    setText(_guiName);

    Application::core()->notifyDatasetGuiNameChanged(*this, previousGuiName);
}

bool DatasetImpl::isFull() const
{
    return _all;
}

bool DatasetImpl::isDerivedData() const
{
    return _derived;
}

hdps::DataType DatasetImpl::getDataType() const
{
    return Application::core()->requestRawData(getRawDataName()).getDataType();
}

void DatasetImpl::setSourceDataSet(const Dataset<DatasetImpl>& dataset)
{
    _sourceDataset = dataset;
    _derived = true;
}

hdps::Dataset<hdps::DatasetImpl> DatasetImpl::getSelection() const
{
    return Application::core()->requestSelection(getSourceDataset<DatasetImpl>()->getRawDataName());
}

hdps::Dataset<hdps::DatasetImpl>& DatasetImpl::getSmartPointer()
{
    return _smartPointer;
}

hdps::Dataset<hdps::DatasetImpl> DatasetImpl::toSmartPointer() const
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
}

bool DatasetImpl::hasLinkedDataFlag(std::int32_t linkedDataFlag)
{
    return _linkedDataFlags & linkedDataFlag;
}

QString DatasetImpl::getRawDataSizeHumanReadable() const
{
    return util::getNoBytesHumanReadable(getRawDataSize());
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
