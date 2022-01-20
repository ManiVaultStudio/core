#pragma once

#include "actions/ToggleAction.h"
#include "actions/TriggerAction.h"

#include "Dataset.h"

#include <QDialog>

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps {

/**
 * Data remove action class
 *
 * Data removal action class
 *
 * @author Thomas Kroes
 */
class DataRemoveAction : public hdps::gui::TriggerAction
{
public:

    class ConfirmDataRemoveDialog : public QDialog
    {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param datasetsToRemove Dataset(s) to remove
         */
        ConfirmDataRemoveDialog(QWidget* parent, const QVector<Dataset<DatasetImpl>>& datasetsToRemove);

    protected:
        ToggleAction    _showAgainAction;       /** Whether to show this dialog again */
        TriggerAction   _removeAction;          /** Remove dataset action */
        TriggerAction   _cancelAction;          /** Cancel action */
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param dataset Smart pointer to the dataset
     */
    DataRemoveAction(QObject* parent, const Dataset<DatasetImpl>& dataset);

protected:
    Dataset<DatasetImpl>    _dataset;       /** Smart pointer to the dataset */
};

}
