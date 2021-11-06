#pragma once

#include "actions/Actions.h"
#include "util/DatasetRef.h"

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
     * @param datasetName Name of the dataset
     */
    DataExportAction(QObject* parent, const QString& datasetName);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr);

protected:
    DatasetRef<DataSet>     _dataset;       /** Dataset reference */
    QStringList             _pluginKinds;   /** Compatible exporter plugins */
};

}
