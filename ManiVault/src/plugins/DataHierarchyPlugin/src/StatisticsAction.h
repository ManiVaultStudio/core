// SPDX-License-Identifier: LGPL-3.0-or-later Group
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/GroupAction.h>
#include <actions/TreeAction.h>
#include <actions/VerticalGroupAction.h>
#include <actions/HorizontalGroupAction.h>

#include <models/RawDataModel.h>
#include <models/RawDataFilterModel.h>

#include <models/DatasetsFilterModel.h>

#include <models/SelectionsModel.h>
#include <models/SelectionsFilterModel.h>

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
    mv::RawDataModel                    _rawDataModel;              /** Standard model for storing the raw data */
    mv::RawDataFilterModel              _rawDataFilterModel;        /** Sort/filter model for the raw data model */
    mv::DatasetsFilterModel             _datasetsFilterModel;       /** Sort/filter model for the datasets model */
    mv::SelectionsModel                 _selectionsModel;           /** Standard model for storing the selections */
    mv::SelectionsFilterModel           _selectionsFilterModel;     /** Sort/filter model for the selections model */
    mv::gui::VerticalGroupAction        _rawDataGroupAction;        /** Raw data group action */
    mv::gui::HorizontalGroupAction      _rawDataCountGroupAction;   /** Raw data count group action */
    mv::gui::TreeAction                 _rawDataTreeAction;         /** Tree action for displaying the raw data */
    mv::gui::HorizontalGroupAction      _datasetsGroupAction;       /** Datasets group action */
    mv::gui::TreeAction                 _datasetsTreeAction;        /** Tree action for displaying the datasets */
    mv::gui::HorizontalGroupAction      _selectionsGroupAction;     /** Selections group action */
    mv::gui::TreeAction                 _selectionsTreeAction;      /** Tree action for displaying the selections */
};
