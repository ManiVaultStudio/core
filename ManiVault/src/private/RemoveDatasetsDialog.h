// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DatasetsToRemoveModel.h"
#include "DatasetsToRemoveFilterModel.h"

#include <actions/VerticalGroupAction.h>
#include <actions/StringAction.h>
#include <actions/OptionAction.h>
#include <actions/TriggerAction.h>
#include <actions/HorizontalGroupAction.h>
#include <actions/TreeAction.h>
#include <actions/LabelProxyAction.h>

#include <Dataset.h>

#include <QDialog>
#include <QTimer>

using namespace mv::gui;

/**
 * Remove datasets dialog class
 *
 * Dialog that supports supervised removal of datasets
 *
 * @author Thomas Kroes */
class RemoveDatasetsDialog : public QDialog
{
public:

    /**
     * Construct with \p selectedDatasets and pointer to \p parent widget
     * @param selectedDatasets Selected datasets
     * @param parent Pointer to parent widget
     */
    RemoveDatasetsDialog(mv::Datasets selectedDatasets, QWidget* parent = nullptr);

    /**
     * Get preferred size
     * @return Size hint
     */
    QSize sizeHint() const override {
        return QSize(400, 150);
    }

    /**
     * Get minimum size hint
     * @return Size hint
     */
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

    /**
     * Get datasets to remove (these are the datasets checked/selected in the selection tree)
     * Facade method for RemoveDatasetsDialog#_model
     * @return Datasets to remove (bottom-up)
     */
    mv::Datasets getDatasetsToRemove() const;

protected:
    mv::Datasets                    _selectedDatasets;              /** Selected datasets, these will be used to populate the model */
    mv::Datasets                    _topLevelDatasets;              /** Top-level selected dataset(s) */
    mv::Datasets                    _descendantDatasets;            /** Descendant datasets of the top-level selected dataset(s) */
    mv::Datasets                    _candidateDatasets;             /** All datasets eligible for removal */
    DatasetsToRemoveModel           _model;                         /** Model with all candidate datasets */
    DatasetsToRemoveFilterModel     _filterModel;                   /** Filter model for the model above */
    VerticalGroupAction             _mainGroupAction;               /** Main group action layout */
    StringAction                    _messageAction;                 /** Message action for informing the user about the existence of descendant datasets */
    OptionAction                    _descendantsModeAction;         /** Option action for switching between keeping and removing descendant datasets */
    VerticalGroupAction             _selectDatasetsGroupAction;     /** Vertical group action for configuring selected datasets */
    TreeAction                      _selectDatasetsAction;          /** Tree action for dataset tree visualization and selection */
    TriggerAction                   _selectAllDatasetsAction;       /** Trigger action to select all datasets */
    HorizontalGroupAction           _bottomHorizontalGroupAction;   /** Group action for bottom actions */
    ToggleAction                    _showAgainAction;               /** Toggle action to hide or show the dialog next time */
    TriggerAction                   _removeAction;                  /** Trigger action to remove the dataset (s) */
    TriggerAction                   _cancelAction;                  /** Trigger action to cancel the removal and the dialog */
    QTimer                          _updateRemoveActionTimer;       /** Timer to prevent redundant updates of the remove action */
};
