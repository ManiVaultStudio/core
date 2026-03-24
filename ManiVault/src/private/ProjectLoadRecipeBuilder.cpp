// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectLoadRecipeBuilder.h"

using namespace mv;

using namespace QtTaskTree;

ProjectLoadRecipeBuilder::ProjectLoadRecipeBuilder()
{
}

Group ProjectLoadRecipeBuilder::makeRecipe(ProjectLoadContextStorage& projectLoadContextStorage)
{
    return Group{
        projectLoadContextStorage,

        // --- Stage 1: Load project JSON (metadata only)
        QSyncTask([this, &projectLoadContextStorage] {
            auto& context = *projectLoadContextStorage;

            try {
                loadProjectJson(context);
            }
            catch (const std::exception& e) {
                context._error = QString::fromUtf8(e.what());
            }
        }),

        // --- Stage 2: Load data hierarchy (separate workflow)
        If([&projectLoadContextStorage] {
	        return projectLoadContextStorage->_error.isEmpty();
        }) >> Then {
            makeLoadDataHierarchyStage(projectLoadContextStorage)
        },

        // --- Stage 3: Load workspace
        If([&projectLoadContextStorage] {
	        return projectLoadContextStorage->_error.isEmpty() && projectLoadContextStorage->_loadWorkspace;
        }) >> Then {
            QSyncTask([this, &projectLoadContextStorage] {
                auto& context = *projectLoadContextStorage;

                try {
                    loadWorkspace(context);
                }
                catch (const std::exception& e) {
                    context._error = QString::fromUtf8(e.what());
                }
            })
        }
    };
}

Group ProjectLoadRecipeBuilder::makeLoadDataHierarchyStage(Storage<ProjectLoadContext>& projectLoadContext)
{
    return Group{
        _dataHierarchyLoadContext,

        // Initialize hierarchy context
        QSyncTask([&projectLoadContext, this] {
            auto& hierarchyLoadContext  = *_dataHierarchyLoadContext;

            hierarchyLoadContext = {};
            hierarchyLoadContext._hierarchyMap = projectLoadContext._dataHierarchyVariantMap;
        }),

        // Run hierarchy recipe
        _dataHierarchyLoadRecipeBuilder.makeRecipe(_dataHierarchyLoadContext)
    };
}