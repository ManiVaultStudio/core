#pragma once

#include "actions/Actions.h"

namespace hdps {

class Core;
class DataHierarchyItem;

/**
 * Data analysis action class
 *
 * Data type dependent data analysis options
 *
 * @author Thomas Kroes
 */
class DataAnalysisAction : public hdps::gui::WidgetAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param core Pointer to the core
     * @param dataHierarchyItem Pointer to the data hierarchy item
     */
    DataAnalysisAction(QObject* parent, Core* core, DataHierarchyItem* dataHierarchyItem);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr);

protected:
    DataHierarchyItem*  _dataHierarchyItem;     /** Pointer to data hierarchy item */
    QStringList         _pluginKinds;           /** Compatible exporter plugins */
};

}
