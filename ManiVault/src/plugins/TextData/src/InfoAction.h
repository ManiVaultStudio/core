// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "TextData.h"

#include <Dataset.h>

#include <actions/StringAction.h>

/**
 * Info action class
 *
 * Action class for displaying text data info
 *
 * @author Julian Thijssen and Thomas Kroes
 */
class InfoAction : public mv::gui::GroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param points Smart pointer to points dataset
     */
    InfoAction(QObject* parent, const mv::Dataset<Text>& dataset);

public: // Action getters

    mv::gui::StringAction& getDataStorageAction() { return _dataStorageAction; }
    mv::gui::StringAction& getNumberOfPointsAction() { return _numberOfPointsAction; }
    mv::gui::StringAction& getRawDataSizeAction() { return _rawDataSizeAction; }

protected:
    mv::Dataset<Text>       _dataset;                           /** Text dataset reference */
    mv::gui::StringAction   _dataStorageAction;                 /** Type of data storage action */
    mv::gui::StringAction   _numberOfPointsAction;              /** Number of points action */
    mv::gui::StringAction   _rawDataSizeAction;                 /** Amount of memory for raw data */
};
