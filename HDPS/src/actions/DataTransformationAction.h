#pragma once

#include "actions/Actions.h"

using namespace hdps::util;
using namespace hdps::gui;

namespace hdps {

/**
 * Data transformation action class
 *
 * Data type dependent data transformation options
 *
 * @author Thomas Kroes
 */
class DataTransformationAction : public WidgetAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param dataset Smart pointer to the dataset
     */
    DataTransformationAction(QObject* parent, const Dataset<DatasetImpl>& dataset);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr);

protected:
    Dataset<DatasetImpl>    _dataset;       /** Smart pointer to the dataset */
    QStringList             _pluginKinds;   /** Compatible data transformation plugins */
};

}
