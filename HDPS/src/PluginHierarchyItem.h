#ifndef HDPS_PLUGIN_HIERARCHY_ITEM_H
#define HDPS_PLUGIN_HIERARCHY_ITEM_H

#include <QVector>

#include "Plugin.h"

class DataImportAction;

namespace hdps
{

class PluginHierarchyItem : public QObject
{
public:
    using PluginHierarchyItems = QVector<PluginHierarchyItem*>;

public:
    explicit PluginHierarchyItem(plugin::Plugin* plugin = nullptr, PluginHierarchyItem* parent = nullptr);

    ~PluginHierarchyItem();

    plugin::Plugin* getPlugin();

    void addChild(PluginHierarchyItem* item);

    PluginHierarchyItem* getParent();
    PluginHierarchyItem* getChild(int row);

    QString getDataAtColumn(int column) const;
    int row() const;

    int childCount() const;
    int columnCount() const;

    QString serialize() const;

    QIcon getIcon();

    QMenu* getContextMenu();

protected:
    plugin::Plugin*         _plugin;            /** Pointer to plugin */
    PluginHierarchyItem*    _parentItem;        /** Keep a pointer to parent node */   
    PluginHierarchyItems    _childItems;        /** Keep a list of pointers to child nodes */
    DataImportAction*       _dataImportAction;  /**  */
};

}

#endif // HDPS_PLUGIN_HIERARCHY_ITEM_H
