#pragma once

#include "actions/Actions.h"

using namespace hdps::gui;

namespace hdps {

class Core;
class DataHierarchyItem;

/**
 * Data export action class
 *
 * Data type dependent data export options
 *
 * @author Thomas Kroes
 */
class DataExportAction : public hdps::gui::WidgetAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param core Pointer to the core
     * @param dataHierarchyItem Pointer to the data hierarchy item
     */
    DataExportAction(QObject* parent, Core* core, DataHierarchyItem* dataHierarchyItem);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu();

protected:
    DataHierarchyItem*  _dataHierarchyItem;     /** Pointer to data hierarchy item */
    QStringList         _pluginKinds;           /** Compatible exporter plugins */
};

}
