#include "Set.h"
#include "DataHierarchyItem.h"

namespace hdps
{

DataHierarchyItem& DatasetImpl::getDataHierarchyItem()
{
    return _core->getDataHierarchyItem(_guid);
}

const DataHierarchyItem& DatasetImpl::getDataHierarchyItem() const
{
    return const_cast<DatasetImpl*>(this)->getDataHierarchyItem();
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
