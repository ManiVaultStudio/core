#pragma once

#include "DataAnalysisAction.h"
#include "DataExportAction.h"
#include "DataCopyAction.h"
#include "DataRemoveAction.h"

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps {

/**
 * Data action class
 *
 * Data action class
 *
 * @author Thomas Kroes
 */
class DataAction : public hdps::gui::WidgetAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param dataset Smart pointer to the dataset
     */
    DataAction(QObject* parent, const Dataset<DatasetImpl>& dataset);

protected:
    Dataset<DatasetImpl>    _dataset;               /** Smart pointer to the dataset */
    DataAnalysisAction      _dataAnalysisAction;    /** Data analysis action */
    DataExportAction        _dataExportAction;      /** Data export action */
    DataCopyAction          _dataCopyAction;        /** Data copy action */
    DataRemoveAction        _dataRemoveAction;      /** Data remove action */
};

}
