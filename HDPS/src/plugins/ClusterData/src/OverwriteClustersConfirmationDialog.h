// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/ToggleAction.h>
#include <actions/TriggerAction.h>

#include <QDialog>

using namespace hdps::gui;

/**
 * Overwrite clusters confirmation dialog class
 *
 * Dialog for asking confirmation to overwrite clusters
 *
 * @author Thomas Kroes
 */
class OverwriteClustersConfirmationDialog : public QDialog
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param numberOfUserClusters Number of clusters in total
     * @param numberOfUserModifiedClusters Number of user modified clusters
     */
    OverwriteClustersConfirmationDialog(QWidget* parent, std::uint32_t numberOfUserClusters, std::uint32_t numberOfUserModifiedClusters);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(350, 150);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

protected:
    ToggleAction    _alwaysAskPermissionAction;     /** Whether to always ask permission prior to overwriting clusters */
    TriggerAction   _discardAction;                 /** Discard cluster change(s) action */
    TriggerAction   _cancelAction;                  /** Cancel action */
};
