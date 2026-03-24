// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyLoadRecipeBuilder.h"

using namespace mv;

using namespace QtTaskTree;

DataHierarchyLoadRecipeBuilder::DataHierarchyLoadRecipeBuilder()
{
}

Group DataHierarchyLoadRecipeBuilder::makeRecipe(DataHierarchyLoadContextStorage& dataHierarchyLoadContextStorage)
{
    const Storage<int> currentDatasetIndex;

    return Group{
        dataHierarchyLoadContextStorage,

        QSyncTask([this, &dataHierarchyLoadContextStorage] {
            auto& context = *dataHierarchyLoadContextStorage;

            try {
                enumerateDatasets(context);
            }
            catch (const std::exception& e) {
                context._error = QString::fromUtf8(e.what());
            }
        }),

        If([&dataHierarchyLoadContextStorage] { return dataHierarchyLoadContextStorage->_error.isEmpty(); }) >> Then {
            QSyncTask([this, &dataHierarchyLoadContextStorage] {
                auto& context = *dataHierarchyLoadContextStorage;

                try {
                    populateHierarchy(context._hierarchyMap, Dataset<DatasetImpl>());
                }
                catch (const std::exception& e) {
                    context._error = QString::fromUtf8(e.what());
                }
            })
        },

        If([&dataHierarchyLoadContextStorage] { return dataHierarchyLoadContextStorage->_error.isEmpty(); }) >> Then {
            makeDatasetLoadStage(dataHierarchyLoadContextStorage)
        }
    };
}

void DataHierarchyLoadRecipeBuilder::populateHierarchy(const QVariantMap& map, const Dataset<>& parent)
{
    QVector<QVariantMap> sortedItems;

    sortedItems.resize(map.count());

    for (const auto& variant : map.values()) {
        sortedItems[variant.toMap()["SortIndex"].toInt()] = variant.toMap();
    }

    for (const auto& item : sortedItems) {
        auto child = loadDataHierarchyItem(item, item["Name"].toString(), parent);
        populateHierarchy(item["Children"].toMap(), child);
    }
}

QVector<DataHierarchyLoadContext::DatasetEntry> DataHierarchyLoadRecipeBuilder::computeLoadOrder(const DataHierarchyLoadContext& dataHierarchyLoadContext)
{
    QVector<DataHierarchyLoadContext::DatasetEntry> ordered = dataHierarchyLoadContext._datasetEntries;

    // Maintain hierarchy item order within partitions, matching old behavior
    std::reverse(ordered.begin(), ordered.end());

    // First load non-derived datasets, then derived/proxy datasets
    std::stable_partition(ordered.begin(), ordered.end(), [](const DataHierarchyLoadContext::DatasetEntry& entry) {
    	return !entry._isDerived;
    });

    return ordered;
}

void DataHierarchyLoadRecipeBuilder::enumerateDatasets(DataHierarchyLoadContext& dataHierarchyLoadContext)
{
    dataHierarchyLoadContext._datasetEntries.clear();

    const std::function<void(const QVariantMap&)> enumerate = [&](const QVariantMap& map) {
        for (const auto& variant : map.values()) {
            const auto itemMap = variant.toMap();

            enumerate(itemMap["Children"].toMap());

            const auto datasetMap = itemMap["Dataset"].toMap();

            DataHierarchyLoadContext::DatasetEntry entry;

            entry._datasetMap   = datasetMap;
            entry._datasetId    = datasetMap["ID"].toString();
            entry._datasetName  = datasetMap["Name"].toString();
            entry._pluginKind   = datasetMap["PluginKind"].toString();
            entry._isDerived    = datasetMap["Derived"].toBool() || !datasetMap["ProxyMembers"].toStringList().isEmpty();

            dataHierarchyLoadContext._datasetEntries.push_back(std::move(entry));
        }
    };

    enumerate(dataHierarchyLoadContext._hierarchyMap);
}

Dataset<> DataHierarchyLoadRecipeBuilder::loadDataHierarchyItem(const QVariantMap& itemMap, const QString& guiName, const Dataset<>& parent)
{
    const auto datasetMap       = itemMap["Dataset"].toMap();
    const auto datasetId        = datasetMap["ID"].toString();
    const auto pluginKind       = datasetMap["PluginKind"].toString();
    const auto isDerived        = datasetMap["Derived"].toBool();
    const auto isFull           = datasetMap["Full"].toBool();
    const auto sourceDatasetID  = datasetMap["SourceDatasetID"].toString();

    Dataset<> loadedDataset;

    if (isDerived || !isFull)
        loadedDataset = data().createDatasetWithoutSelection(pluginKind, guiName, parent, datasetId);
    else
        loadedDataset = data().createDataset(pluginKind, guiName, parent, datasetId);

    if (!loadedDataset.isValid())
        throw std::runtime_error(QString("Unable to create dataset \"%1\" of plugin kind \"%2\"").arg(datasetId, pluginKind).toStdString());

    if (isDerived)
        loadedDataset->setSourceDataset(sourceDatasetID);

    loadedDataset->getDataHierarchyItem().fromVariantMap(itemMap);

    return loadedDataset;
}

Group DataHierarchyLoadRecipeBuilder::makeDatasetLoadStage(DataHierarchyLoadContextStorage& dataHierarchyLoadContextStorage)
{
    return Group{
        QThreadFunctionTask<void>([this, &dataHierarchyLoadContextStorage](QThreadFunction<void>& task) {
            task.setThreadFunctionData([this, &dataHierarchyLoadContextStorage](QPromise<void>& promise) {
                auto& context = *dataHierarchyLoadContextStorage;

                try {
                    const auto ordered = computeLoadOrder(context);

                    for (const auto& entry : ordered) {
                        if (entry._isDerived)
                            continue;

                        if (promise.isCanceled()) {
                            if (context._error.isEmpty())
                                context._error = "Dataset loading canceled";

                            return;
                        }

                        auto dataset = mv::data().getDataset(entry._datasetId).get();

                        if (!dataset) {
                            throw std::runtime_error(QString("Unable to find dataset \"%1\" for loading").arg(entry._datasetId).toStdString());
                        }

                        dataset->fromVariantMap(entry._datasetMap);
                    }
                }
                catch (const std::exception& e) {
                    if (context._error.isEmpty())
                        context._error = QString::fromUtf8(e.what());

                    promise.future().cancel();
                }
            });
        }),
        If([&dataHierarchyLoadContextStorage] { return dataHierarchyLoadContextStorage->_error.isEmpty(); }) >> Then {
            QThreadFunctionTask<void>([this, &dataHierarchyLoadContextStorage](QThreadFunction<void>& task) {
                task.setThreadFunctionData([this, &dataHierarchyLoadContextStorage](QPromise<void>& promise) {
                    auto& context = *dataHierarchyLoadContextStorage;

                    try {
                        const auto ordered = computeLoadOrder(context);

                        for (const auto& entry : ordered) {
                            if (!entry._isDerived)
                                continue;

                            if (promise.isCanceled()) {
                                if (context._error.isEmpty())
                                    context._error = "Dataset loading canceled";

                                return;
                            }

                            auto dataset = mv::data().getDataset(entry._datasetId).get();

                            if (!dataset) {
                                throw std::runtime_error(QString("Unable to find dataset \"%1\" for loading").arg(entry._datasetId).toStdString());
                            }

                            dataset->fromVariantMap(entry._datasetMap);
                        }
                    }
                    catch (const std::exception& e) {
                        if (context._error.isEmpty())
                            context._error = QString::fromUtf8(e.what());

                        promise.future().cancel();
                    }
                });
            })
        }
    };
}