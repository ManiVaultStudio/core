// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PointsAction.h"

#include "PointData.h"

PointsAction::PointsAction(QObject* parent, const mv::Dataset<Points>& points) :
    GroupAction(parent, "Points", true),
    _storageBackendAction(this, points),
    _pointsViewAction(this, points)
{
    addAction(&_storageBackendAction);
    addAction(&_pointsViewAction);
}

StorageBackendAction& PointsAction::getStorageBackendAction()
{
    return _storageBackendAction;
}

PointsViewAction& PointsAction::getPointsViewAction()
{
    return _pointsViewAction;
}
