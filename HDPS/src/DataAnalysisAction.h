#pragma once

#include "actions/Actions.h"
#include "util/DatasetRef.h"

using namespace hdps::util;

namespace hdps {

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
     * @param dataset Reference to dataset
     */
    DataAnalysisAction(QObject* parent, DataSet& dataset);

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
