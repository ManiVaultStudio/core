// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "UnhideAction.h"
#include "StatisticsAction.h"

#include <models/DataHierarchyTreeModel.h>
#include <models/DataHierarchyFilterModel.h>

#include <actions/ToggleAction.h>
#include <actions/TriggerAction.h>

#include <widgets/HierarchyWidget.h>

#include <DataHierarchyItem.h>
#include <Dataset.h>

#include <QWidget>

/**
 * Widget for displaying the data hierarchy
 * 
 * @author Thomas Kroes
 */
class DataHierarchyWidget : public QWidget
{
public:

    /**
     * Constructor
     * @param parent Parent widget
     */
    DataHierarchyWidget(QWidget* parent);

    /**
     * Get tree model
     * @return Reference to the data hierarchy tree model
     */
    mv::DataHierarchyTreeModel& getTreeModel() {
        return _treeModel;
    }

    /**
     * Get filter model
     * @return Reference to the data hierarchy filter model
     */
    mv::DataHierarchyFilterModel& getFilterModel() {
        return _filterModel;
    }

    /**
     * Get hierarchy widget
     * @return Reference to the hierarchy widget
     */
    mv::gui::HierarchyWidget& getHierarchyWidget() {
        return _hierarchyWidget;
    }

protected:

    /**
     * Get model index of the dataset
     * @param dataset Smart pointer to dataset
     * @return Dataset model index
     */
    QModelIndex getModelIndexByDataset(const mv::Dataset<mv::DatasetImpl>& dataset) const;

    /**
     * Update the data hierarchy item to reflect the expansion state of the corresponding model item with \p filterModelIndex
     * @param filterModelIndex Filter model index of the model item to sync with
     */
    void updateDataHierarchyItemExpansion(const QModelIndex& filterModelIndex = QModelIndex());

    /**
     * Initializes the expansion state of child model items recursively
     * @param parentFilterModelIndex The parent filter model index for which to update the child model items
     */
    void initializeChildModelItemsExpansion(QModelIndex parentFilterModelIndex = QModelIndex());

    /**
     * Initializes the model selection from the data hierarchy
     * @param parentFilterModelIndex The parent filter model index for which to update the child model items
     */
    void initializeSelection();

private:
    mv::DataHierarchyTreeModel      _treeModel;             /** Model containing data to be displayed in the hierarchy */
    mv::DataHierarchyFilterModel    _filterModel;           /** Data hierarchy filter model */
    mv::gui::HierarchyWidget        _hierarchyWidget;       /** Widget for displaying hierarchy */
    mv::gui::TriggerAction          _resetAction;           /** Reset all action */
    UnhideAction                    _unhideAction;          /** Action class for un-hiding hidden datasets */
    QPointer<StatisticsAction>      _statisticsAction;      /** Action class for showing data manager statistics */
};
