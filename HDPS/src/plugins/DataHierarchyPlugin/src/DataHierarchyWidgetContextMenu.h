#pragma once

#include <Dataset.h>
#include <PluginType.h>

#include <QMenu>

class QAction;

using namespace hdps;
using namespace hdps::plugin;

/**
 * Data hierarchy widget context menu
 * 
 * Constructs a data hierarchy widget context menu based on the current datasets selection
 * 
 * @author Thomas Kroes
 */
class DataHierarchyWidgetContextMenu : public QMenu
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param selectedDatasets Selected datasets in the data hierarchy widget
     */
    DataHierarchyWidgetContextMenu(QWidget* parent, Datasets datasets);

private:

    /**
     * Add menus for \p pluginType
     * Goes over all loaded plugins of \p pluginType and builds a hierarchical menu structure (based on plugin title)
     * @parsm Pointer to menu for \p pluginType
     */
    void addMenusForPluginType(plugin::Type pluginType);

    /**
     * Get action for datasets grouping
     * @return Pointer to action for datasets grouping
     */
    QAction* getGroupAction();

    /**
     * Get action for dataset(s) locking
     * @return Pointer to action for dataset(s) locking
     */
    QMenu* getLockMenu();

    /**
     * Get action for dataset(s) unlocking
     * @return Pointer to action for dataset(s) unlocking
     */
    QMenu* getUnlockMenu();

private:
    Datasets        _datasets;      /** Selected datasets in the data hierarchy widget */
};
