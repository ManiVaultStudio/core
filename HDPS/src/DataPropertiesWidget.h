#pragma once

#include "DataHierarchyManager.h"

#include <Dataset.h>

#include <actions/GroupsAction.h>
#include <actions/TriggerAction.h>

#include <QWidget>

using namespace hdps::util;

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

protected:

    /**
     * Callback when is called when the selected items in the data hierarchy changes
     * @param datasetId Globally unique identifier of the dataset
     */
    void selectedItemsChanged(DataHierarchyItems selectedItems);

signals:

    /**
     * Signals that the current dataset gui name changed (used to change the docking widget title)
     * @param datasetGuiName GUI name of the current dataset
     */
    void currentDatasetGuiNameChanged(const QString& datasetGuiName);

protected:
    Dataset<DatasetImpl>    _dataset;           /** Smart point to current dataset */
    QVBoxLayout             _layout;            /** Main layout */
    GroupsAction            _groupsAction;      /** Groups action */
};

}
}
