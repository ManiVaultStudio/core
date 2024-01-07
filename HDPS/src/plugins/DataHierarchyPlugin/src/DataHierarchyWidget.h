// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <DataHierarchyItem.h>
#include <Dataset.h>

#include <actions/ToggleAction.h>
#include <actions/TriggerAction.h>
#include <models/DataHierarchyFilterModel.h>
#include <models/DataHierarchyModel.h>
#include <widgets/HierarchyWidget.h>

#include <QWidget>

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
    void addDataHierarchyItem(mv::DataHierarchyItem& dataHierarchyItem);

    /**
     * Get model index of the dataset
     * @param dataset Smart pointer to dataset
     * @return Dataset model index
     */
    QModelIndex getModelIndexByDataset(const mv::Dataset<mv::DatasetImpl>& dataset);

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
    mv::gui::HierarchyWidget  _hierarchyWidget;       /** Widget for displaying hierarchy */
    mv::gui::ToggleAction     _groupingAction;        /** Data grouping action */
    mv::gui::TriggerAction    _resetAction;           /** Reset all action */
};
