// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <models/DataHierarchyModel.h>
#include <models/DataHierarchyFilterModel.h>
#include <actions/TriggerAction.h>
#include <actions/ToggleAction.h>
#include <widgets/HierarchyWidget.h>
#include <DataHierarchyItem.h>
#include <Dataset.h>

#include <QWidget>

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

    /**
     * Get model
     * @return Reference to the data hierarchy model
     */
    mv::DataHierarchyModel& getModel() {
        return _model;
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
    QModelIndex getModelIndexByDataset(const mv::Dataset<mv::DatasetImpl>& dataset);

protected:

    /**
     * Invoked when the dataset grouping action is toggled
     * @param toggled Whether dataset grouping is enabled or not
     */
    void onGroupingActionToggled(const bool& toggled);

    /** Update the visibility of the tree view columns */
    void updateColumnsVisibility();

    void updateItemExpansion(const QModelIndex& filterModelIndex = QModelIndex());
    void initializeExpansion(QModelIndex parentFilterModelIndex = QModelIndex());

private:
    mv::DataHierarchyModel          _model;             /** Model containing data to be displayed in the hierarchy */
    mv::DataHierarchyFilterModel    _filterModel;       /** Data hierarchy filter model */
    mv::gui::HierarchyWidget        _hierarchyWidget;   /** Widget for displaying hierarchy */
    mv::gui::ToggleAction           _groupingAction;    /** Data grouping action */
    mv::gui::TriggerAction          _resetAction;       /** Reset all action */
};
