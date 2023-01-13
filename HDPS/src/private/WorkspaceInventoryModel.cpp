#include "WorkspaceInventoryModel.h"

#include <Application.h>
#include <CoreInterface.h>
#include <Workspace.h>

using namespace hdps;

#ifdef _DEBUG
    #define WORKSPACE_INVENTORY_MODEL_VERBOSE
#endif

QMap<WorkspaceInventoryModel::Column, QPair<QString, QString>> WorkspaceInventoryModel::columns = QMap<WorkspaceInventoryModel::Column, QPair<QString, QString>>({
    { WorkspaceInventoryModel::Column::Summary, { "Summary", "Workspace summary delegate column" }},
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
