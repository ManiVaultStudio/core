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

#include <Dataset.h>

#include <QDialog>
#include <QStandardItemModel>

using namespace mv::gui;

/**
 * Confirm datasets removal dialog class
 *
 * Dialog for asking confirmation prior to removing of datasets
 *
 * @author Thomas Kroes */
class ConfirmDatasetsRemovalDialog : public QDialog
{
public:

    /**
     * Construct with \p selectedDatasets and pointer to \p parent widget
     * @param selectedDatasets Selected datasets
     * @param parent Pointer to parent widget
     */
    ConfirmDatasetsRemovalDialog(mv::Datasets selectedDatasets, QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(400, 0);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

    /**
     * Get datasets to remove (these are the datasets checked/selected in the selection tree)
     * Facade method for ConfirmDatasetsRemovalDialog#_datasetsToRemoveModel
     * @return Datasets to remove (bottom-up)
     */
    mv::Datasets getDatasetsToRemove() const;

protected:
    mv::Datasets                    _selectedDatasets;              /** Selected datasets, these will be used as a basis for the dataset selection tree */
    DatasetsToRemoveModel           _datasetsToRemoveModel;         /** Hierarchical model which contains all datasets which need to be removed */
    DatasetsToRemoveFilterModel     _datasetsToRemoveFilterModel;   /** Filter model for the model above */
    VerticalGroupAction             _mainGroupAction;               /** Main group action layout */
    StringAction                    _messageAction;                 /** Message action */
    HorizontalGroupAction           _modusGroupAction;              /** Group action for selecting and configuring removal modus */
    VerticalGroupAction             _selectDatasetsGroupAction;     /** Vertical group action for configuring the modus */
    TreeAction                      _selectDatasetsAction;          /** Tree action for dataset selection */
    HorizontalGroupAction           _buttonsGroupAction;            /** Group action to layout out the previous buttons horizontally */
    TriggerAction                   _removeAction;                  /** Trigger action to remove the dataset (s) */
    TriggerAction                   _cancelAction;                  /** Trigger action to cancel the removal and the dialog */
};
