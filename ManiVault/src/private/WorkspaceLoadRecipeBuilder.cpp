// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkspaceLoadRecipeBuilder.h"
#include "ProjectLoadRecipeBuilder.h"

using namespace mv;

using namespace QtTaskTree;

Group WorkspaceLoadRecipeBuilder::makeRecipe(ProjectLoadContextStorage& projectLoadContextStorage)
{
    return Group{
        QSyncTask([this, &projectLoadContextStorage] {
            try {
                auto& workspaceLoadContext = projectLoadContextStorage->_workspaceLoadContext;
                loadWorkspaceJson(workspaceLoadContext);

                qDebug() << __FUNCTION__ << projectLoadContextStorage->_dataHierarchyLoadContext._datasetLoadContexts.count();
            }
            catch (const std::exception& e) {
                projectLoadContextStorage->_error = QString::fromUtf8(e.what());
            }
        }),
        QSyncTask([this, &projectLoadContextStorage] {
            try {
                auto& workspaceLoadContext = projectLoadContextStorage->_workspaceLoadContext;
                loadWorkspace(workspaceLoadContext);
            }
            catch (const std::exception& e) {
                projectLoadContextStorage->_error = QString::fromUtf8(e.what());
            }
        })
    };
}

void WorkspaceLoadRecipeBuilder::loadWorkspaceJson(WorkspaceLoadContext& workspaceLoadContext)
{
    try {
        qDebug() << __FUNCTION__ << workspaceLoadContext._jsonFilePath;

        if (workspaceLoadContext._jsonFilePath.isEmpty())
            throw std::runtime_error("Workspace JSON file path is empty.");

        workspaceLoadContext._workspaceVariantMap = util::loadJsonToVariantMap(workspaceLoadContext._jsonFilePath);
    }
    catch (const std::exception& e) {
        if (workspaceLoadContext._error.isEmpty())
            workspaceLoadContext._error = QString::fromUtf8(e.what());
    }
}

void WorkspaceLoadRecipeBuilder::loadWorkspace(WorkspaceLoadContext& workspaceLoadContext)
{
    qDebug() << __FUNCTION__ << workspaceLoadContext._jsonFilePath;
}