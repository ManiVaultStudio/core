// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectLoadRecipeBuilder.h"
#include "OpenProjectWorkflow.h"

using namespace mv;

using namespace QtTaskTree;

Group ProjectLoadRecipeBuilder::makeRecipe(OpenProjectContextStorage& openProjectContextStorage, ProjectLoadContextStorage& projectLoadContextStorage)
{
    return Group{
        projectLoadContextStorage,

        QSyncTask([&openProjectContextStorage , &projectLoadContextStorage] {
	        auto& context = *projectLoadContextStorage;

        	context = {};

	        context._dataHierarchyLoadContext._jsonFilePath     = openProjectContextStorage->projectJsonPath;
	        context._workspaceLoadContext._jsonFilePath         = openProjectContextStorage->workspaceJsonPath;
	    }),

    	_dataHierarchyLoadRecipeBuilder.makeRecipe(projectLoadContextStorage),
		_datasetsLoadRecipeBuilder.makeRecipe(projectLoadContextStorage),
        _workspaceLoadRecipeBuilder.makeRecipe(projectLoadContextStorage)
    };
}

