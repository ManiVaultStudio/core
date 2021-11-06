#pragma once

#include "actions/Actions.h"
#include "util/DatasetRef.h"

using namespace hdps::util;

namespace hdps {

/**
 * Data import action class
 *
 * This action creates an import menu for all available data importers
 *
 * @author Thomas Kroes
 */
class DataImportAction : public hdps::gui::WidgetAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    DataImportAction(QObject* parent);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr);

protected:
    QStringList     _pluginKinds;   /** Compatible importer plugins */
};

}
