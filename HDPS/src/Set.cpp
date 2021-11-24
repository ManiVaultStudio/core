#include "Set.h"
#include "DataHierarchyItem.h"

namespace hdps
{

const DataHierarchyItem& DatasetImpl::getDataHierarchyItem() const
{
    return const_cast<DatasetImpl*>(this)->getDataHierarchyItem();
}

DataHierarchyItem& DatasetImpl::getDataHierarchyItem()
{
    return _core->getDataHierarchyItem(_guid);
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

void DatasetImpl::lock()
{
    getDataHierarchyItem().setLocked(true);
}

void DatasetImpl::unlock()
{
    getDataHierarchyItem().setLocked(false);
}

bool DatasetImpl::isLocked() const
{
    return getDataHierarchyItem().getLocked();
}

void DatasetImpl::addAction(hdps::gui::WidgetAction& widgetAction)
{
    _core->getDataHierarchyItem(_guid).addAction(widgetAction);
}

hdps::gui::WidgetActions DatasetImpl::getActions() const
{
    return _core->getDataHierarchyItem(_guid).getActions();
}

QMenu* DatasetImpl::getContextMenu(QWidget* parent /*= nullptr*/)
{
    return _core->getDataHierarchyItem(_guid).getContextMenu(parent);
}

void DatasetImpl::populateContextMenu(QMenu* contextMenu)
{
    return _core->getDataHierarchyItem(_guid).populateContextMenu(contextMenu);
}

}
