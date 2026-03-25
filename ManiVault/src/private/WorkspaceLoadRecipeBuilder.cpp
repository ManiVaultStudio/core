// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkspaceLoadRecipeBuilder.h"

using namespace mv;

using namespace QtTaskTree;

Group WorkspaceLoadRecipeBuilder::makeRecipe(WorkspaceLoadContextStorage& workspaceLoadContextStorage)
{
    return Group{
        workspaceLoadContextStorage,

        QSyncTask([this, &workspaceLoadContextStorage] {
            auto& context = *workspaceLoadContextStorage;
            try {
                loadWorkspace(context);
            }
            catch (const std::exception& e) {
                context._error = QString::fromUtf8(e.what());
            }
        })
    };
}

void WorkspaceLoadRecipeBuilder::loadWorkspace(const WorkspaceLoadContext& workspaceLoadContext)
{
    qDebug() << "Loading workspace from JSON file:" << workspaceLoadContext._workspaceJsonPath;
}