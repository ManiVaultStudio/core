#pragma once

#include "actions/ToggleAction.h"
#include "actions/TriggerAction.h"

#include "Dataset.h"

#include <QDialog>
#include <QStringListModel>

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

    /**
     * Datasets string list model class
     *
     * Convenience class which disables all string entries
     *
     */
    class DatasetsStringListModel : QStringListModel
    {
    public:

        /**
         * Constructor
         * @param strings Input strings
         * @param parent Pointer to parent object
         */
        explicit DatasetsStringListModel(const QStringList& strings, QObject* parent = nullptr) :
            QStringListModel(strings, parent)
        {
        }

        /**
         * Get item flags
         * @param index Model index
         * @return Item flags
         */
        Qt::ItemFlags flags(const QModelIndex& index) const override
        {
            return Qt::NoItemFlags;
        }
    };

    /**
     * Confirm data remove dialog class
     *
     * Dialog class for asking confirmation prior to dataset(s) removal
     *
     */
    class ConfirmDataRemoveDialog : public QDialog
    {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param title Title of the dialog
         * @param datasetsToRemove Dataset(s) to remove
         */
        ConfirmDataRemoveDialog(QWidget* parent, const QString& windowTitle, const QVector<Dataset<DatasetImpl>>& datasetsToRemove);

        QSize sizeHint() const override
        {
            return QSize(400, 250);
        }

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
