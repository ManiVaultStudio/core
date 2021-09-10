#pragma once

#include "actions/Actions.h"
#include "util/DatasetRef.h"

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
class DataRemoveAction : public hdps::gui::WidgetAction
{
public:

    class ConfirmDataRemoveDialog : public QDialog
    {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param datasetsToRemove Names of the dataset(s) to remove
         */
        ConfirmDataRemoveDialog(QWidget* parent, const QStringList& datasetsToRemove);

    protected:
        ToggleAction    _showAgainAction;       /** Whether to show this dialog again */
        TriggerAction   _removeAction;          /** Remove dataset action */
        TriggerAction   _cancelAction;          /** Cancel action */
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param datasetName Name of the dataset
     */
    DataRemoveAction(QObject* parent, const QString& datasetName);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr);

protected:
    DatasetRef<DataSet>     _dataset;                           /** Dataset reference */
    TriggerAction           _removeSelectedAction;              /** Remove selected dataset only action */
    TriggerAction           _removeSelectedAndChildrenAction;   /** Remove dataset and all its descendants action */
};

}
