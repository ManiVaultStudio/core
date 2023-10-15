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

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

/**
 * Info action class
 *
 * Action class for displaying basic points info
 *
 * @author Thomas Kroes
 */
class InfoAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param points Smart pointer to points dataset
     */
    InfoAction(QObject* parent, const Dataset<Points>& points);

public: // Action getters

    StringAction& getDataStorageAction() { return _dataStorageAction; }
    ProxyDatasetsAction& getProxyDatasetsAction() { return _proxyDatasetsAction; }
    StringAction& getNumberOfPointsAction() { return _numberOfPointsAction; }
    StringAction& getNumberOfDimensionsAction() { return _numberOfDimensionsAction; }
    StringAction& getRawDataSizeAction() { return _rawDataSizeAction; }
    NumberOfSelectedPointsAction& getNumberOfSelectedPointsAction() { return _numberOfSelectedPointsAction; }
    SelectedIndicesAction& getSelectedIndicesAction() { return _selectedIndicesAction; }
    CreateSetFromSelectionAction& getCreateSetFromSelection() { return _createSetFromSelection; }

protected:
    Dataset<Points>                 _points;                            /** Points dataset reference */
    StringAction                    _dataStorageAction;                 /** Type of data storage action */
    ProxyDatasetsAction             _proxyDatasetsAction;               /** Proxy datasets action */
    StringAction                    _numberOfPointsAction;              /** Number of points action */
    StringAction                    _numberOfDimensionsAction;          /** Number of dimensions action */
    StringAction                    _rawDataSizeAction;                 /** Amount of memory for raw data */    
    NumberOfSelectedPointsAction    _numberOfSelectedPointsAction;      /** Number of selected points action */
    SelectedIndicesAction           _selectedIndicesAction;             /** Selected indices action */
    CreateSetFromSelectionAction    _createSetFromSelection;            /** Create set from selection action */
};