// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectsModelVisibilityController.h"

#include <QHash>
#include <QDebug>
#include <algorithm>
#include <QDateTime>

#ifdef _DEBUG
    #define PROJECTS_MODEL_VISIBILITY_CONTROLLER _VERBOSE
#endif

using namespace mv::gui;
using namespace mv::util;

namespace mv {

ProjectsModelVisibilityController::ProjectsModelVisibilityController(AbstractProjectsModel* projectsModel, VisibilityRuleFunction visibilityRuleFunction, QObject* parent) :
	QObject(parent),
	_projectsModel(projectsModel),
	_visibilityRuleFunction(std::move(visibilityRuleFunction))
{
    Q_ASSERT(_projectsModel);

    connect(_projectsModel, &QStandardItemModel::modelReset, this, &ProjectsModelVisibilityController::recomputeAll);
    connect(_projectsModel, &QStandardItemModel::rowsInserted, this, &ProjectsModelVisibilityController::onRowsInserted);
    connect(_projectsModel, &QStandardItemModel::rowsAboutToBeRemoved, this, &ProjectsModelVisibilityController::onRowsAboutToBeRemoved);
    connect(_projectsModel, &QStandardItemModel::rowsRemoved, this, &ProjectsModelVisibilityController::onRowsRemoved);
    connect(_projectsModel, &QStandardItemModel::dataChanged, this, &ProjectsModelVisibilityController::onDataChanged);

    recomputeAll();
}

void ProjectsModelVisibilityController::recomputeAll()
{
    if (!_projectsModel)
        return;

    QHash<QString, QModelIndexList> groups;

    walk(QModelIndex{}, [&](const QModelIndex& ix) {
        const auto uuid = _projectsModel->index(ix.row(), static_cast<int>(AbstractProjectsModel::Column::UUID), ix.parent()).data().toString();
        groups[uuid] << ix;
	});

    for (auto it = groups.begin(); it != groups.end(); ++it)
        applyGroupVisibility(it.value());
}

void ProjectsModelVisibilityController::onRowsInserted(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(parent)

	recomputeAll();
}

void ProjectsModelVisibilityController::onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
    _uuidsPendingRemove.clear();

    for (int rowIndex = first; rowIndex <= last; ++rowIndex) {
        _uuidsPendingRemove.insert(_projectsModel->index(rowIndex, static_cast<int>(AbstractProjectsModel::Column::UUID), parent).data().toString());
    }
}

void ProjectsModelVisibilityController::onRowsRemoved(const QModelIndex& parent, int, int)
{
    Q_UNUSED(parent)

	recomputeAll();

    _uuidsPendingRemove.clear();
}

void ProjectsModelVisibilityController::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    Q_UNUSED(roles)

	recomputeAll();
}

void ProjectsModelVisibilityController::applyGroupVisibility(const QModelIndexList& rows)
{
    if (rows.isEmpty())
        return;

    if (rows.size() == 1 && rows.first().isValid()) {
	    setVisibility(rows.front(), true);
    	return;
    }

    const auto winner = _visibilityRuleFunction(rows, _projectsModel);

    for (const auto& ix0 : rows) {
        if (!ix0.isValid())
            continue;

        setVisibility(ix0, ix0 == winner);
    }
}

void ProjectsModelVisibilityController::setVisibility(const QModelIndex& rowIndex, bool on)
{
    const auto isVisibleIndex = _projectsModel->index(rowIndex.row(), static_cast<int>(AbstractProjectsModel::Column::IsVisible), rowIndex.parent());

    if (isVisibleIndex.data(Qt::EditRole).toBool() == on)
        return;

	_projectsModel->setData(isVisibleIndex, on, Qt::EditRole);
}

}
