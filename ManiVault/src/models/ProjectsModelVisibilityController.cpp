// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectsModelVisibilityController.h"

#include <QHash>
#include <QDebug>
#include <algorithm>

#ifdef _DEBUG
    #define PROJECTS_MODEL_VISIBILITY_CONTROLLER _VERBOSE
#endif

using namespace mv::gui;
using namespace mv::util;

namespace mv {

ProjectsModelVisibilityController::ProjectsModelVisibilityController(AbstractProjectsModel* projectsModel, VisibilityRuleFunction visibilityRuleFunction, QObject* parent) :
	QObject(parent),
	_projectsModel(projectsModel)
{
    Q_ASSERT(_projectsModel);

    if (visibilityRuleFunction) {
        _visibilityRuleFunction = std::move(visibilityRuleFunction);
    }
    else {
        _visibilityRuleFunction = [this](const QModelIndexList& rows, QStandardItemModel* model) -> QModelIndex {
            Q_ASSERT(!rows.isEmpty());

            if (rows.isEmpty())
                return {};

            QModelIndexList fromLocal, fromServer;

            for (const auto& row : rows) {
                const auto projectsJsonDsn = row.siblingAtColumn(static_cast<std::int32_t>(AbstractProjectsModel::Column::ProjectsJsonDsn)).data(Qt::EditRole).toString();

                if (QUrl::fromUserInput(projectsJsonDsn).isLocalFile())
                    fromLocal << row;
                else
                    fromServer << row;
            }

            if (!fromServer.isEmpty())
                return fromServer.last();

            if (!fromLocal.isEmpty())
                return fromLocal.last();

            return {};
        };
    }

    connect(_projectsModel, &QStandardItemModel::modelReset, this, &ProjectsModelVisibilityController::recomputeAll);
    connect(_projectsModel, &QStandardItemModel::rowsInserted, this, &ProjectsModelVisibilityController::onRowsInserted);
    connect(_projectsModel, &QStandardItemModel::rowsRemoved, this, &ProjectsModelVisibilityController::onRowsRemoved);
    connect(_projectsModel, &QStandardItemModel::dataChanged, this, &ProjectsModelVisibilityController::onDataChanged);

    recomputeAll();
}

void ProjectsModelVisibilityController::recomputeAll()
{
    if (!_projectsModel)
        return;

    QHash<QString, QModelIndexList> groups;
    QModelIndexList emptyUuidRows;

    walk(QModelIndex{}, [&](const QModelIndex& index) {
        const auto uuidIndex    = index.siblingAtColumn(static_cast<int>(AbstractProjectsModel::Column::UUID));
        const auto uuid         = uuidIndex.data().toString().trimmed();

        if (uuid.isEmpty()) {
            emptyUuidRows << index;
            return;
        }

    	groups[uuid] << index;
    });

    for (const auto& index : emptyUuidRows)
        setVisibility(index, true);

    for (auto it = groups.begin(); it != groups.end(); ++it)
        applyGroupVisibility(it.value());
}

void ProjectsModelVisibilityController::onRowsInserted(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(parent)

	recomputeAll();
}

void ProjectsModelVisibilityController::onRowsRemoved(const QModelIndex& parent, int, int)
{
    Q_UNUSED(parent)

	recomputeAll();
}

void ProjectsModelVisibilityController::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    Q_UNUSED(roles)

    if (_isUpdating)
        return;

	recomputeAll();
}

void ProjectsModelVisibilityController::applyGroupVisibility(const QModelIndexList& rows)
{
    if (rows.isEmpty())
        return;

    if (_isUpdating)
        return;

	_isUpdating = true;

    auto resetGuard = qScopeGuard([&] { _isUpdating = false; });

    if (rows.size() == 1 && rows.first().isValid()) {
	    setVisibility(rows.front(), true);
    	return;
    }

    const auto visibilityWinner = _visibilityRuleFunction(rows, _projectsModel);

	if (!visibilityWinner.isValid() || !rows.contains(visibilityWinner)) {
        setVisibility(rows.front(), true);

        for (int rowIndex = 1; rowIndex < rows.size(); ++rowIndex)
            setVisibility(rows[rowIndex], false);
    } else {
        for (const auto& index : rows) {
            if (!index.isValid())
                continue;

            setVisibility(index, index == visibilityWinner);
        }
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
