// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "TextData.h"

#include <Dataset.h>

#include <actions/StringAction.h>

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

/**
 * Info action class
 *
 * Action class for displaying text data info
 *
 * @author Julian Thijssen and Thomas Kroes
 */
class InfoAction : public GroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param points Smart pointer to points dataset
     */
    InfoAction(QObject* parent, const Dataset<Text>& dataset);

public: // Action getters

    StringAction& getDataStorageAction() { return _dataStorageAction; }
    StringAction& getNumberOfPointsAction() { return _numberOfPointsAction; }
    StringAction& getRawDataSizeAction() { return _rawDataSizeAction; }

protected:
    Dataset<Text>                   _dataset;                           /** Text dataset reference */
    StringAction                    _dataStorageAction;                 /** Type of data storage action */
    StringAction                    _numberOfPointsAction;              /** Number of points action */
    StringAction                    _rawDataSizeAction;                 /** Amount of memory for raw data */
};
