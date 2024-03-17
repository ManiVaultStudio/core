// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkspaceInventoryModel.h"

#include <CoreInterface.h>
#include <Workspace.h>

#include <QList>
#include <QStandardItem>

using namespace mv;

#ifdef _DEBUG
    #define WORKSPACE_INVENTORY_MODEL_VERBOSE
#endif

QMap<WorkspaceInventoryModel::Column, QPair<QString, QString>> WorkspaceInventoryModel::columns = QMap<WorkspaceInventoryModel::Column, QPair<QString, QString>>({
    { WorkspaceInventoryModel::Column::SummaryDelegate, { "Summary", "Workspace summary delegate column" }},
    { WorkspaceInventoryModel::Column::Icon, { "Icon", "Icon of the workspace" }},
    { WorkspaceInventoryModel::Column::Name, { "Name", "Name of the workspace" }},
    { WorkspaceInventoryModel::Column::Description, { "Description", "Description of the workspace" }},
    { WorkspaceInventoryModel::Column::Tags, { "Tags", "Workspace tags" }},
    { WorkspaceInventoryModel::Column::FilePath, { "FilePath", "Location of the workspace on disk" }},
    { WorkspaceInventoryModel::Column::Type, { "Type", "Type of workspace (built-in or recent)" }}
});

WorkspaceInventoryModel::WorkspaceInventoryModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    synchronizeWithWorkspaceManager();

    for (const auto& column : columns.keys()) {
        setHeaderData(static_cast<int>(column), Qt::Horizontal, columns[column].first);
        setHeaderData(static_cast<int>(column), Qt::Horizontal, columns[column].second, Qt::ToolTipRole);
    }
}

void WorkspaceInventoryModel::synchronizeWithWorkspaceManager()
{
#ifdef WORKSPACE_INVENTORY_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    for (const auto workspaceLocation : workspaces().getWorkspaceLocations()) {
        Workspace workspace(workspaceLocation.getFilePath());

        const auto workspaceName = QFileInfo(workspaceLocation.getFilePath()).baseName();

        QList<QStandardItem*> workspaceRow = {
            new QStandardItem(),
            new QStandardItem(),
            new QStandardItem(workspaceName),
            new QStandardItem(workspace.getDescriptionAction().getString()),
            new QStandardItem(workspace.getTagsAction().getStrings().join(", ")),
            new QStandardItem(workspaceLocation.getFilePath()),
            new QStandardItem(workspaceLocation.getTypeName())
        };

        for (auto item : workspaceRow)
            item->setEditable(false);

        workspaceRow[static_cast<int>(WorkspaceInventoryModel::Column::Icon)]->setData(workspaces().getIcon(), Qt::DecorationRole);
        workspaceRow[static_cast<int>(WorkspaceInventoryModel::Column::Icon)]->setTextAlignment(Qt::AlignTop);
        workspaceRow[static_cast<int>(WorkspaceInventoryModel::Column::Tags)]->setData(workspace.getTagsAction().getStrings(), Qt::EditRole);

        appendRow(workspaceRow);
    }
}
