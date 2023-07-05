// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/StringAction.h>
#include <actions/ToggleAction.h>
#include <actions/TriggerAction.h>

#include <Dataset.h>

#include <QDialog>

/**
 * Group data dialog class
 * 
 * Dialog class for configuring new data groups
 *
 * @author Thomas Kroes
 */
class GroupDataDialog : public QDialog
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param datasets Datasets for grouping
     */
    GroupDataDialog(QWidget* parent, const hdps::Datasets& datasets);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(350, 150);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

    /**
     * Get chosen group name
     * @return Group name
     */
    QString getGroupName() const;

protected:
    hdps::Datasets              _datasets;              /** Datasets for grouping */
    hdps::gui::StringAction     _groupNameAction;       /** Group name action */
    hdps::gui::ToggleAction     _showDialogAction;      /** Whether to show the name dialog next time */
};