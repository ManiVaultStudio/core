// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyLoadRecipeBuilder.h"
#include "ProjectLoadRecipeBuilder.h"

using namespace mv;

using namespace QtTaskTree;

Group DataHierarchyLoadRecipeBuilder::makeRecipe(ProjectLoadContextStorage& projectLoadContextStorage)
{
    const Storage<int> currentDatasetIndex;

    return Group{
        QSyncTask([this, &projectLoadContextStorage] {
            try {
                auto& dataHierarchyLoadContext = projectLoadContextStorage->_dataHierarchyLoadContext;
                loadProjectJson(dataHierarchyLoadContext);
            }
            catch (const std::exception& e) {
                auto& dataHierarchyLoadContext = projectLoadContextStorage->_dataHierarchyLoadContext;
                dataHierarchyLoadContext._error = QString::fromUtf8(e.what());
            }
        }),
        QSyncTask([this, &projectLoadContextStorage] {
            try {
                auto& dataHierarchyLoadContext = projectLoadContextStorage->_dataHierarchyLoadContext;
                enumerateDatasets(dataHierarchyLoadContext);
            }
            catch (const std::exception& e) {
                auto& dataHierarchyLoadContext = projectLoadContextStorage->_dataHierarchyLoadContext;
                dataHierarchyLoadContext._error = QString::fromUtf8(e.what());  
            }
        }),
        QSyncTask([this, &projectLoadContextStorage] {
            try {
                auto& dataHierarchyLoadContext = projectLoadContextStorage->_dataHierarchyLoadContext;
                partitionDatasetLoadContexts(dataHierarchyLoadContext);
            }
            catch (const std::exception& e) {
                auto& dataHierarchyLoadContext = projectLoadContextStorage->_dataHierarchyLoadContext;
                dataHierarchyLoadContext._error = QString::fromUtf8(e.what());
            }
        }),
        QSyncTask([this, &projectLoadContextStorage] {
            try {
                auto& dataHierarchyLoadContext = projectLoadContextStorage->_dataHierarchyLoadContext;
                populateHierarchy(dataHierarchyLoadContext);
            }
            catch (const std::exception& e) {
                auto& dataHierarchyLoadContext = projectLoadContextStorage->_dataHierarchyLoadContext;
                dataHierarchyLoadContext._error = QString::fromUtf8(e.what());
            }
        })
    };
}

void DataHierarchyLoadRecipeBuilder::loadProjectJson(DataHierarchyLoadContext& dataHierarchyLoadContext)
{
    try {
        qDebug() << __FUNCTION__;

        if (dataHierarchyLoadContext._jsonFilePath.isEmpty())
            throw std::runtime_error("Project JSON file path is empty.");

        const auto jsonVariantMap       = util::loadJsonToVariantMap(dataHierarchyLoadContext._jsonFilePath);
        const auto projectVariantMap    = jsonVariantMap["Project"].toMap();

        dataHierarchyLoadContext._dataHierarchyVariantMap = projectVariantMap["DataHierarchy"].toMap();
    }
    catch (const std::exception& e) {
        if (dataHierarchyLoadContext._error.isEmpty())
            dataHierarchyLoadContext._error = QString::fromUtf8(e.what());
    }
}

void DataHierarchyLoadRecipeBuilder::enumerateDatasets(DataHierarchyLoadContext& dataHierarchyLoadContext)
{
    try {
        qDebug() << __FUNCTION__;

        dataHierarchyLoadContext._datasetLoadContexts.clear();

        const std::function<void(const QVariantMap&)> enumerate = [&](const QVariantMap& map) {
            for (const auto& variant : map.values()) {
                const auto itemMap = variant.toMap();

                enumerate(itemMap["Children"].toMap());

                const auto datasetMap = itemMap["Dataset"].toMap();

                DatasetLoadContext datasetLoadContext;

                datasetLoadContext._datasetMap   = datasetMap;
                datasetLoadContext._datasetId    = datasetMap["ID"].toString();
                datasetLoadContext._datasetName  = datasetMap["Name"].toString();
                datasetLoadContext._pluginKind   = datasetMap["PluginKind"].toString();
                datasetLoadContext._isDerived    = datasetMap["Derived"].toBool() || !datasetMap["ProxyMembers"].toStringList().isEmpty();

                dataHierarchyLoadContext._datasetLoadContexts.push_back(std::move(datasetLoadContext));
            }
        };

        enumerate(dataHierarchyLoadContext._dataHierarchyVariantMap);
    }
    catch (const std::exception& e) {
        if (dataHierarchyLoadContext._error.isEmpty())
            dataHierarchyLoadContext._error = QString::fromUtf8(e.what());
    }
}

void DataHierarchyLoadRecipeBuilder::partitionDatasetLoadContexts(DataHierarchyLoadContext& dataHierarchyLoadContext)
{
    try {
        qDebug() << __FUNCTION__;

        dataHierarchyLoadContext._derivedDatasetLoadContexts.clear();
        dataHierarchyLoadContext._nonDerivedDatasetLoadContexts.clear();

        auto& orderedDatasetLoadContexts = dataHierarchyLoadContext._datasetLoadContexts;

        // Maintain hierarchy item order within partitions, matching old behavior
        std::reverse(orderedDatasetLoadContexts.begin(), orderedDatasetLoadContexts.end());

        // First load non-derived datasets, then derived/proxy datasets
        std::stable_partition(orderedDatasetLoadContexts.begin(), orderedDatasetLoadContexts.end(), [](const DatasetLoadContext& datasetLoadContext) {
            return !datasetLoadContext._isDerived;
        });

        for (const auto& orderedDatasetLoadContext : orderedDatasetLoadContexts)
            if (orderedDatasetLoadContext._isDerived)
                dataHierarchyLoadContext._derivedDatasetLoadContexts.push_back(const_cast<DatasetLoadContext*>(&orderedDatasetLoadContext));
            else
                dataHierarchyLoadContext._nonDerivedDatasetLoadContexts.push_back(const_cast<DatasetLoadContext*>(&orderedDatasetLoadContext));
    }
    catch (const std::exception& e) {
        if (dataHierarchyLoadContext._error.isEmpty())
            dataHierarchyLoadContext._error = QString::fromUtf8(e.what());
    }
}

void DataHierarchyLoadRecipeBuilder::populateHierarchy(DataHierarchyLoadContext& dataHierarchyLoadContext)
{
    try {
        qDebug() << __FUNCTION__;

        const auto& rootMap = dataHierarchyLoadContext._dataHierarchyVariantMap;

        populateHierarchy(dataHierarchyLoadContext, rootMap, {});
    }
    catch (const std::exception& e) {
        if (dataHierarchyLoadContext._error.isEmpty())
            dataHierarchyLoadContext._error = QString::fromUtf8(e.what());
    }
}

void DataHierarchyLoadRecipeBuilder::populateHierarchy(DataHierarchyLoadContext& dataHierarchyLoadContext, const QVariantMap& map, const Dataset<>& parent)
{
    qDebug() << __FUNCTION__;

    QVector<QVariantMap> sortedItems;

    sortedItems.resize(map.count());

    for (const auto& variant : map.values()) {
        sortedItems[variant.toMap()["SortIndex"].toInt()] = variant.toMap();
    }

    for (const auto& item : sortedItems) {
        auto child = loadDataHierarchyItem(dataHierarchyLoadContext, item, item["Name"].toString(), parent);
        populateHierarchy(dataHierarchyLoadContext, item["Children"].toMap(), child);
    }
}

Dataset<> DataHierarchyLoadRecipeBuilder::loadDataHierarchyItem(DataHierarchyLoadContext& dataHierarchyLoadContext, const QVariantMap& itemMap, const QString& guiName, const Dataset<>& parent)
{
    try {
        qDebug() << __FUNCTION__;

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

        auto& datasetEntries = dataHierarchyLoadContext._datasetLoadContexts;

        auto it = std::find_if(datasetEntries.begin(), datasetEntries.end(), [&datasetId](const DatasetLoadContext& entry) {
            return entry._datasetId == datasetId;
        });

        if (it != datasetEntries.end()) {
            DatasetLoadContext& entry = *it;

            entry._dataset = loadedDataset;
        }
        else {
            throw std::runtime_error(QString("Dataset entry with ID \"%1\" not found in dataset entries").arg(datasetId).toStdString());
        }

        return loadedDataset;
    }
    catch (const std::exception& e) {
        if (dataHierarchyLoadContext._error.isEmpty())
            dataHierarchyLoadContext._error = QString::fromUtf8(e.what());

        return {};
    }
}

Group DataHierarchyLoadRecipeBuilder::makeDatasetLoadStage(DataHierarchyLoadContextStorage& dataHierarchyLoadContextStorage)
{
    qDebug() << "Making dataset load stage for" << dataHierarchyLoadContextStorage->_datasetLoadContexts.size() << "datasets";
    return {};
   
}