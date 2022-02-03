#pragma once

#include <Dataset.h>

#include <actions/GroupsAction.h>
#include <actions/StringAction.h>
#include <actions/TriggerAction.h>

#include <QWidget>
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

    /**
     * Updates the state of the toolbar
     */
    void updateToolbar();

    /**
     * Updates the properties
     */
    void updateProperties();

    /**
     * Get whether one (or more) groups can be expanded
     * @return Whether one (or more) groups can be expanded
     */
    bool canExpandAll() const;

    /**
     * Expand all group actions
     */
    void expandAll();

    /**
     * Get whether one (or more) groups can be expanded
     * @return Whether one (or more) groups can be expanded
     */
    bool canCollapseAll() const;

    /**
     * Collapse all group actions
     */
    void collapseAll();

    /**
     * Get groups expansion states
     * @return Vector of booleans, indicating whether a group is expanded or collapsed
     */
    QVector<bool> getExpansions() const;

    /**
     * Get number of expanded group actions
     * @return Number of expanded group actions
     */
    std::int32_t getNumberOfExpandedGroupActions() const;

    /**
     * Get group actions for the loaded dataset
     * @return Vector of group actions
     */
    QVector<GroupAction*> getGroupActions() const;

signals:

    /**
     * Signals that the current dataset gui name changed (used to change the docking widget title)
     * @param datasetGuiName GUI name of the current dataset
     */
    void currentDatasetGuiNameChanged(const QString& datasetGuiName);

protected:
    Dataset<DatasetImpl>    _dataset;                   /** Smart point to current dataset */
    QVBoxLayout             _layout;                    /** Main layout */
    GroupsAction            _groupsAction;              /** Groups action */
    GroupAction             _filteredActionsAction;     /** Group action for filtered actions */
    QWidget                 _toolbarWidget;             /** Toolbar widget */
    QHBoxLayout             _toolbarLayout;             /** Toolbar layout */
    StringAction            _filterAction;              /** Filter action */
    TriggerAction           _expandAllAction;           /** Expand all datasets action */
    TriggerAction           _collapseAllAction;         /** Collapse all datasets action */
    TriggerAction           _loadDefaultAction;         /** Load default action */
    TriggerAction           _saveDefaultAction;         /** Save default action */
    TriggerAction           _factoryDefaultAction;      /** Restore factory default action */
};

}
}
