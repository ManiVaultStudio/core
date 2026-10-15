// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "pointdata_export.h"
#include "PointsViewAction.h"
#include "StorageBackendAction.h"

#include <Dataset.h>
#include <actions/GroupAction.h>

class Points;

/**
 * @brief Point-data action section containing the point-data view actions.
 *
 * PointsAction groups the storage-backend selector and the reusable
 * point-data view action in the PointData action hierarchy. Keeping these as
 * separate child actions lets the standard action system control their layout
 * and keeps the embedded view implementation independent from this group.
 *
 * The action stores dataset handles in its children. Their widgets own the
 * corresponding Qt controls, and the view remains read-only with respect to
 * point values.
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT PointsAction final : public mv::gui::GroupAction
{
    Q_OBJECT

public:
    /** Constructs the point-data action section. */
    PointsAction(QObject* parent, const mv::Dataset<Points>& points);

    /** Returns the storage backend action. */
    StorageBackendAction& getStorageBackendAction();

    /** Returns the point-data view action. */
    PointsViewAction& getPointsViewAction();

private:
    StorageBackendAction _storageBackendAction; /**< Storage backend selector and configuration action. */
    PointsViewAction     _pointsViewAction;     /**< Embedded point-data view action. */
};
