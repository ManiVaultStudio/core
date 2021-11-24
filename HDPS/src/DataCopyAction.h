#pragma once

#include "actions/Actions.h"

using namespace hdps::util;

namespace hdps {

/**
 * Data copy action class
 *
 * Action class for copying datasets
 *
 * @author Thomas Kroes
 */
class DataCopyAction : public hdps::gui::TriggerAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param dataset Smart pointer to the dataset
     */
    DataCopyAction(QObject* parent, const Dataset<DatasetImpl>& dataset);

protected:
    Dataset<DatasetImpl>    _dataset;       /** Smart pointer to the dataset */
};

}
