#pragma once

#include <Dataset.h>
#include <model/DataHierarchyModel.h>
#include <model/DataHierarchyFilterModel.h>
#include <actions/StringAction.h>
#include <actions/TriggerAction.h>
#include <actions/ToggleAction.h>
#include <widgets/HierarchyWidget.h>

#include <QWidget>
#include <QTreeView>
#include <QGraphicsOpacityEffect>

namespace hdps {
    class DataHierarchyItem;
}

class DataHierarchyPlugin;

/**
 * Widget for displaying the data hierarchy
 * 
 * @author Thomas Kroes
 */
class DataHierarchyWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Parent widget
     */
    DataHierarchyWidget(QWidget* parent);

protected:

    /**
     * Add a data hierarchy to the tree widget
     * @param dataHierarchyItem Reference to the data hierarchy item
     */
    void addDataHierarchyItem(hdps::DataHierarchyItem& dataHierarchyItem);

    /**
     * Get model index of the dataset
     * @param dataset Smart pointer to dataset
     * @return Dataset model index
     */
    QModelIndex getModelIndexByDataset(const hdps::Dataset<hdps::DatasetImpl>& dataset);

protected:

    /**
     * Invoked when the dataset grouping action is toggled
     * @param toggled Whether dataset grouping is enabled or not
     */
    void onGroupingActionToggled(const bool& toggled);

    /** Update the visibility of the tree view columns */
    void updateColumnsVisibility();

private:
    DataHierarchyModel          _model;                 /** Model containing data to be displayed in the hierarchy */
    DataHierarchyFilterModel    _filterModel;           /** Data hierarchy filter model */
    HierarchyWidget             _hierarchyWidget;       /** Widget for displaying hierarchy */
    ToggleAction                _groupingAction;        /** Data grouping action */
};