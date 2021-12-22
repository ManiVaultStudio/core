#pragma once

#include "DataAnalysisAction.h"
#include "DataViewAction.h"
#include "DataGroupingAction.h"
#include "DataExportAction.h"
#include "DataTransformationAction.h"

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
    Dataset<DatasetImpl>        _dataset;                       /** Smart pointer to the dataset */
    DataAnalysisAction          _dataAnalysisAction;            /** Data analysis action */
    DataViewAction              _dataViewAction;                /** Data view action */
    DataExportAction            _dataExportAction;              /** Data export action */
    DataTransformationAction    _dataTransformationAction;      /** Data transformation action */
    DataGroupingAction          _dataGroupingAction;            /** Data grouping action */
};

}
