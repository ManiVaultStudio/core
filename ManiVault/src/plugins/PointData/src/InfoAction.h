// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PointData.h"

#include "ProxyDatasetsAction.h"
#include "NumberOfSelectedPointsAction.h"
#include "SelectedIndicesAction.h"
#include "CreateSetFromSelectionAction.h"

#include <actions/StringAction.h>

/**
 * Info action class
 *
 * Action class for displaying basic points info
 *
 * @author Thomas Kroes
 */
class InfoAction : public mv::gui::GroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param points Smart pointer to points dataset
     */
    InfoAction(QObject* parent, const mv::Dataset<Points>& points);

public: // Action getters

    mv::gui::StringAction& getDataStorageAction() { return _dataStorageAction; }
    ProxyDatasetsAction& getProxyDatasetsAction() { return _proxyDatasetsAction; }
    mv::gui::StringAction& getNumberOfPointsAction() { return _numberOfPointsAction; }
    mv::gui::StringAction& getNumberOfDimensionsAction() { return _numberOfDimensionsAction; }
    mv::gui::StringAction& getRawDataSizeAction() { return _rawDataSizeAction; }
    NumberOfSelectedPointsAction& getNumberOfSelectedPointsAction() { return _numberOfSelectedPointsAction; }
    SelectedIndicesAction& getSelectedIndicesAction() { return _selectedIndicesAction; }
    CreateSetFromSelectionAction& getCreateSetFromSelection() { return _createSetFromSelection; }

protected:
    mv::Dataset<Points>             _points;                            /** Points dataset reference */
    mv::gui::StringAction           _dataStorageAction;                 /** Type of data storage action */
    ProxyDatasetsAction             _proxyDatasetsAction;               /** Proxy datasets action */
    mv::gui::StringAction           _numberOfPointsAction;              /** Number of points action */
    mv::gui::StringAction           _numberOfDimensionsAction;          /** Number of dimensions action */
    mv::gui::StringAction           _rawDataSizeAction;                 /** Amount of memory for raw data */
    NumberOfSelectedPointsAction    _numberOfSelectedPointsAction;      /** Number of selected points action */
    SelectedIndicesAction           _selectedIndicesAction;             /** Selected indices action */
    CreateSetFromSelectionAction    _createSetFromSelection;            /** Create set from selection action */
};