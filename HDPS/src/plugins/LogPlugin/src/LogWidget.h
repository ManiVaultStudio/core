#pragma once

#include <DataHierarchyItem.h>
#include <Dataset.h>

#include <actions/GroupsAction.h>

#include <QWidget>

using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;

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

protected:
    Dataset<DatasetImpl>    _dataset;               /** Smart point to current dataset */
    QVBoxLayout             _layout;                /** Main layout */
    GroupsAction            _groupsAction;          /** Groups action */
    GroupsAction::Widget*   _groupsActionWidget;    /** Pointer to groups action widget (used to change label sizing) */
};
