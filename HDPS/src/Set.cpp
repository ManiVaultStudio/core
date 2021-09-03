#include "Set.h"
#include "DataHierarchyItem.h"

namespace hdps
{

SharedDataHierarchyItem DataSet::getHierarchyItem()
{
    return _core->getDataHierarchyItem(_name);
}

const SharedDataHierarchyItem DataSet::getHierarchyItem() const
{
    return const_cast<DataSet*>(this)->getHierarchyItem();
}

void DataSet::addAction(hdps::gui::WidgetAction& widgetAction)
{
    Q_ASSERT(!_name.isEmpty());
    _core->getDataHierarchyItem(_name)->addAction(widgetAction);
}

hdps::gui::WidgetActions DataSet::getActions() const
{
    Q_ASSERT(!_name.isEmpty());
    return _core->getDataHierarchyItem(_name)->getActions();
}

QMenu* DataSet::getContextMenu(QWidget* parent /*= nullptr*/)
{
    Q_ASSERT(!_name.isEmpty());
    return _core->getDataHierarchyItem(_name)->getContextMenu(parent);
}

void DataSet::populateContextMenu(QMenu* contextMenu)
{
    Q_ASSERT(!_name.isEmpty());
    return _core->getDataHierarchyItem(_name)->populateContextMenu(contextMenu);
}

} // namespace hdps