// SPDX-License-Identifier: LGPL-3.0-or-later Group
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/GroupAction.h>
#include <actions/TreeAction.h>
#include <actions/HorizontalGroupAction.h>
#include <actions/StringAction.h>
#include <actions/TriggerAction.h>

#include <QStandardItemModel>

/**
 * Statistics action class
 *
 * Action class for showing datasets statistics:
 * - Raw data overview
 * - Overall raw data size
 * - Datasets overview
 * - Selection sets
 *
 * @author Thomas Kroes
 */
class StatisticsAction : public mv::gui::GroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent widget and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE StatisticsAction(QObject* parent, const QString& title);

private:

    void refreshRawData();
    void refreshOverallRawDataSize();
    void refreshDatasets();
    void refreshSelections();

private:
    QStandardItemModel              _rawDataModel;              /** Standard model for storing the raw data */
    QStandardItemModel              _datasetsModel;             /** Standard model for storing the dataset */
    QStandardItemModel              _selectionsModel;           /** Standard model for storing the selections */
    mv::gui::TreeAction             _rawDataTreeAction;         /** Tree action for displaying the raw data */
    mv::gui::HorizontalGroupAction  _rawDataGroupAction;        /** Raw data group action */
    mv::gui::StringAction           _overallRawDataSizeAction;  /** String action for displaying the overall raw data size */
    mv::gui::TriggerAction          _refreshRawDataAction;      /** Triggers refresh of the raw data */
    mv::gui::TreeAction             _datasetsTreeAction;        /** Tree action for displaying the datasets */
    mv::gui::TreeAction             _selectionsTreeAction;      /** Tree action for displaying the selections */
};
