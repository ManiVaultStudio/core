// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "InfoAction.h"

#include <util/Miscellaneous.h>

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

InfoAction::InfoAction(QObject* parent, const Dataset<Text>& dataset) :
    GroupAction(parent, "Group", true),
    _dataset(dataset),
    _dataStorageAction(this, "Storage type"),
    _numberOfPointsAction(this, "Number of points"),
    _rawDataSizeAction(this, "Raw data size")
{
    setText("Info");

    addAction(&_dataStorageAction);
    addAction(&_numberOfPointsAction);
    addAction(&_rawDataSizeAction);

    _dataStorageAction.setEnabled(false);
    _numberOfPointsAction.setEnabled(false);
    _rawDataSizeAction.setEnabled(false);

    _dataStorageAction.setToolTip("The type of data storage (e.g. owner or proxy)");
    _numberOfPointsAction.setToolTip("The number of text elements");
    _rawDataSizeAction.setToolTip("The amount of memory occupied for raw data by the dataset");

    const auto updateActions = [this]() -> void {
        if (!_dataset.isValid())
            return;

        _dataStorageAction.setString("Owner");
        _numberOfPointsAction.setString(QString::number(_dataset->getNumRows()));
        _rawDataSizeAction.setString(_dataset->getRawDataSizeHumanReadable());
    };

    connect(&_dataset, &Dataset<Text>::dataChanged, this, updateActions);

    updateActions();
}
