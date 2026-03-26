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
        QSyncTask([this, &projectLoadContextStorage] {
            auto& context = *projectLoadContextStorage;
            auto& dataHierarchyLoadContext = context._dataHierarchyLoadContext;

            dataHierarchyLoadContext._progress = std::make_shared<DataHierarchyLoadProgress>();
            dataHierarchyLoadContext._progress->_datasetsLoaded.store(0, std::memory_order_relaxed);
            dataHierarchyLoadContext._progress->_totalDatasets =
                dataHierarchyLoadContext._nonDerivedDatasetLoadContexts.size() +
                dataHierarchyLoadContext._derivedDatasetLoadContexts.size();

            const int totalDatasets = dataHierarchyLoadContext._progress->_totalDatasets;

            //task.setSubtasks(totalDatasets);
            //task.setRunning();
            //task.setProgress(.5f, "-------------");
            dataHierarchyLoadContext._progress->_datasetLoadedCallback = [](int completed, int total, const QString& datasetName) {
                mv::projects().getOpenTask().setProgressMode(Task::ProgressMode::Manual);
                mv::projects().getOpenTask().setProgress(static_cast<float>(completed) / total, datasetName);

                qDebug() << "Loaded dataset:" << datasetName << "(" << completed << "/" << total << ")";

                if (total == 0)
                    mv::projects().getOpenTask().setFinished();

                QCoreApplication::processEvents();
            };

            
        }),
		_datasetsLoadRecipeBuilder.makeRecipe(projectLoadContextStorage),
        _workspaceLoadRecipeBuilder.makeRecipe(projectLoadContextStorage)
    };
}

