#include "Set.h"
#include "DataHierarchyItem.h"

#include <util/Serialization.h>

namespace hdps
{

void DatasetImpl::makeSubsetOf(Dataset<DatasetImpl> fullDataset)
{
    _rawDataName = fullDataset->_rawDataName;

    if (!_rawDataName.isEmpty())
        _rawData = &_core->requestRawData(getRawDataName());

    setAll(false);
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
    // Found children
    QVector<Dataset<DatasetImpl>> children;

    // Loop over all data hierarchy children and add to the list if occur in the data types
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

void DatasetImpl::fromVariantMap(const QVariantMap& variantMap)
{
    variantMapMustContain(variantMap, "Name");
    variantMapMustContain(variantMap, "Derived");

    setGuiName(variantMap["Name"].toString());

    if (variantMap.contains("Derived")) {
        _derived = variantMap["Derived"].toBool();

        if (_derived)
            _sourceDataset = getParent();
    }
}

QVariantMap DatasetImpl::toVariantMap() const
{
    return {
        { "Name", getGuiName() },
        { "DataType", getDataType().getTypeString() },
        { "PluginKind", _rawData->getKind() },
        { "Derived", isDerivedData() },
        { "GroupIndex", getGroupIndex() }
    };
}

std::int32_t DatasetImpl::getGroupIndex() const
{
    return _groupIndex;
}

void DatasetImpl::setGroupIndex(const std::int32_t& groupIndex)
{
    _groupIndex = groupIndex;

    _core->notifyDataSelectionChanged(this);
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

}
