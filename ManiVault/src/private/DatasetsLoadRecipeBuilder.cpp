// SPDX-License-Identifier: LGPL-3.0-or-later / A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetsLoadRecipeBuilder.h"
#include "ProjectLoadRecipeBuilder.h"

using namespace QtTaskTree;

Group DatasetsLoadRecipeBuilder::makeRecipe(const ProjectLoadContextStorage& projectLoadContextStorage)
{
    return Group{
    	parallel,
        QSyncTask([this, projectLoadContextStorage] {
            try {
                loadDatasets(projectLoadContextStorage);
            }
            catch (const std::exception& e) {
                projectLoadContextStorage->_error = QString::fromUtf8(e.what());
            }
        }),
        QSyncTask([this, projectLoadContextStorage] {
            try {
            }
            catch (const std::exception& e) {
                projectLoadContextStorage->_error = QString::fromUtf8(e.what());
            }
        })
    };
}

void DatasetsLoadRecipeBuilder::loadDatasets(const ProjectLoadContextStorage& projectLoadContextStorage)
{
    auto& dataHierarchyLoadContext = projectLoadContextStorage->_dataHierarchyLoadContext;

    auto progress = dataHierarchyLoadContext._progress;

	const auto& contexts = dataHierarchyLoadContext._datasetLoadContexts;

    QStringList subtaskNames;

    QList<DatasetLoadContext*> workItems;
    workItems.reserve(contexts.size());

    for (const auto& context : contexts) {
        auto* nonConstContext = const_cast<DatasetLoadContext*>(&context);

        if (!nonConstContext)
            throw std::runtime_error("Dataset load context is null.");

        if (!nonConstContext->_dataset.isValid())
            throw std::runtime_error("Dataset is invalid.");

        workItems.append(nonConstContext);

        subtaskNames << nonConstContext->_datasetName;
    }

    constexpr auto concurrencyMode = mv::util::ConcurrencyMode::Parallel;

    if (concurrencyMode == mv::util::ConcurrencyMode::Parallel) {
        static QThreadPool pool;
        pool.setMaxThreadCount(std::max(1, QThread::idealThreadCount() - 1));

        std::atomic<bool> failed = false;
        QString errorString;
        QMutex errorMutex;

        QtConcurrent::blockingMap(&pool, workItems, [&](DatasetLoadContext* ctx) {
            if (failed.load(std::memory_order_relaxed))
                return;

            try {
                ctx->_dataset->fromVariantMap(ctx->_datasetMap);

                //mv::projects().getOpenTask().setSubtaskFinished(ctx->_dataset->getGuiName());
            }
            catch (const std::exception& e) {
                failed.store(true, std::memory_order_relaxed);

                QMutexLocker locker(&errorMutex);
                if (errorString.isEmpty())
                    errorString = QString::fromUtf8(e.what());
            }
        });
    }
    else {
        for (auto& workItem : workItems) {
            workItem->_dataset->fromVariantMap(workItem->_datasetMap);
        }
    }
}
