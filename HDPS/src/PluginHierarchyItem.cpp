#include "PluginHierarchyItem.h"
#include "DataImportAction.h"

#include <QDebug>
#include <QMenu>

namespace hdps
{

PluginHierarchyItem::PluginHierarchyItem(plugin::Plugin* plugin /*= nullptr*/, PluginHierarchyItem* parent /*= nullptr*/) :
    QObject(parent),
    _plugin(plugin),
    _parentItem(parent),
    _childItems(),
    _dataImportAction(plugin == nullptr ? new DataImportAction(this) : nullptr)
{
}

PluginHierarchyItem::~PluginHierarchyItem()
{
    qDeleteAll(_childItems);
}

hdps::plugin::Plugin* PluginHierarchyItem::getPlugin()
{
    return _plugin;
}

void PluginHierarchyItem::addChild(PluginHierarchyItem* item)
{
    _childItems.append(item);
}

PluginHierarchyItem* PluginHierarchyItem::getParent()
{
    return _parentItem;
}

PluginHierarchyItem* PluginHierarchyItem::getChild(int row)
{
    if (row < 0 || row >= _childItems.size())
        return nullptr;

    return _childItems[row];
}

QString PluginHierarchyItem::getDataAtColumn(int column) const
{
    if (_plugin == nullptr)
        return "Data";

    return _plugin->getGuiName();
}

int PluginHierarchyItem::row() const
{
    if (_parentItem)
        return _parentItem->_childItems.indexOf(const_cast<PluginHierarchyItem*>(this));

    return 0;
}

int PluginHierarchyItem::childCount() const
{
    return _childItems.count();
}

int PluginHierarchyItem::columnCount() const
{
    return 1;
}

QString PluginHierarchyItem::serialize() const
{
    return "";
    //return _setName + "\n" + _dataType;
}

QIcon PluginHierarchyItem::getIcon()
{
    if (_plugin == nullptr)
        return Application::getIconFont("FontAwesome").getIcon("home");
    else
        return _plugin->getIcon();
}

QMenu* PluginHierarchyItem::getContextMenu()
{
    if (_plugin == nullptr)
        _dataImportAction->getContextMenu();
    else
        return _plugin->getContextMenu();
}

}
