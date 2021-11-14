#include "Set.h"
#include "DataHierarchyItem.h"

namespace hdps
{

DataHierarchyItem& DataSet::getDataHierarchyItem()
{
    return _core->getDataHierarchyItem(_id);
}

const DataHierarchyItem& DataSet::getDataHierarchyItem() const
{
    return const_cast<DataSet*>(this)->getDataHierarchyItem();
}

void DataSet::addAction(hdps::gui::WidgetAction& widgetAction)
{
    _core->getDataHierarchyItem(_id).addAction(widgetAction);
}

hdps::gui::WidgetActions DataSet::getActions() const
{
    return _core->getDataHierarchyItem(_id).getActions();
}

QMenu* DataSet::getContextMenu(QWidget* parent /*= nullptr*/)
{
    return _core->getDataHierarchyItem(_id).getContextMenu(parent);
}

void DataSet::populateContextMenu(QMenu* contextMenu)
{
    return _core->getDataHierarchyItem(_id).populateContextMenu(contextMenu);
}

}
