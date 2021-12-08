#pragma once

#include "Dataset.h"

#include "actions/GroupsAction.h"

#include <QWidget>
#include <QPushButton>
#include <QTreeWidget>

class QTreeWidgetItem;

using namespace hdps::util;
using namespace hdps::gui;

namespace hdps
{

namespace gui
{

/**
 * Data properties widget class
 *
 * Widget class for viewing/editing data
 *
 * @author Thomas Kroes
 */
class DataPropertiesWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    DataPropertiesWidget(QWidget* parent);

    /**
     * Set the ID of the current dataset
     * @param datasetId Globally unique identifier of the dataset
     */
    void setDatasetId(const QString& datasetId);

protected:

    /** Load dataset properties */
    void loadDataset();

signals:

    /**
     * Signals that the current dataset gui name changed (used to change the docking widget title)
     * @param datasetGuiName GUI name of the current dataset
     */
    void currentDatasetGuiNameChanged(const QString& datasetGuiName);

protected:
    Dataset<DatasetImpl>    _dataset;       /** Smart point to curent dataset */
    GroupsAction            _groupsAction;  /** Groups action */
};

}
}
