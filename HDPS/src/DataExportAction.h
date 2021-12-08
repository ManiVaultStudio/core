#pragma once

#include "actions/Actions.h"

using namespace hdps::util;

namespace hdps {

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
     * @param dataset Smart pointer to the dataset
     */
    DataExportAction(QObject* parent, const Dataset<DatasetImpl>& dataset);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr);

protected:
    Dataset<DatasetImpl>    _dataset;       /** Smart pointer to the dataset */
    QStringList             _pluginKinds;   /** Compatible exporter plugins */
};

}
