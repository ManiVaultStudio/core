// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "InfoAction.h"

#include <util/Miscellaneous.h>

using namespace hdps;
using namespace hdps::gui;
using namespace hdps::util;

InfoAction::InfoAction(QObject* parent, const Dataset<Points>& points) :
    GroupAction(parent, "Group", true),
    _points(points),
    _dataStorageAction(this, "Storage type"),
    _proxyDatasetsAction(this, points),
    _numberOfPointsAction(this, "Number of points"),
    _numberOfDimensionsAction(this, "Number of dimensions"),
    _rawDataSizeAction(this, "Raw data size"),
    _numberOfSelectedPointsAction(this, points),
    _selectedIndicesAction(this, points),
    _createSetFromSelection(this, points)
{
    setText("Info");

    addAction(&_dataStorageAction);
    addAction(&_proxyDatasetsAction);
    addAction(&_numberOfPointsAction);
    addAction(&_numberOfDimensionsAction);
    addAction(&_rawDataSizeAction);
    addAction(&_numberOfSelectedPointsAction);
    addAction(&_selectedIndicesAction);
    addAction(&_createSetFromSelection);

    _dataStorageAction.setEnabled(false);
    _numberOfPointsAction.setEnabled(false);
    _numberOfDimensionsAction.setEnabled(false);
    _rawDataSizeAction.setEnabled(false);

    _dataStorageAction.setToolTip("The type of data storage (e.g. owner or proxy)");
    _proxyDatasetsAction.setToolTip("Proxy datasets");
    _numberOfPointsAction.setToolTip("The number of points");
    _numberOfDimensionsAction.setToolTip("The number of dimensions in the point data");
    _rawDataSizeAction.setToolTip("The amount of memory occupied for raw data by the dataset");

    _createSetFromSelection.setVisible(false);

    const auto updateActions = [this]() -> void {
        if (!_points.isValid())
            return;

        _dataStorageAction.setString(_points->isProxy() ? "Group" : "Owner");
        _numberOfPointsAction.setString(QString::number(_points->getNumPoints()));
        _numberOfDimensionsAction.setString(QString::number(_points->getNumDimensions()));
        _rawDataSizeAction.setString(_points->getRawDataSizeHumanReadable());
    };

    connect(&_points, &Dataset<Points>::dataChanged, this, updateActions);

    updateActions();
}
