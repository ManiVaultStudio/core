// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ClusterData.h"
#include "InfoAction.h"

#include "Dataset.h"
#include "DataHierarchyItem.h"
#include "event/Event.h"
#include "PointData/PointData.h"

#include "Application.h"

#include <util/Serialization.h>
#include <util/VariantMapWorkflowContext.h>
#include <util/WorkflowRuntimeScoped.h>

#include <QtCore>
#include <QtDebug>

#include <set>

#include "ClustersSerializer.h"

Q_PLUGIN_METADATA(IID "studio.manivault.ClusterData")using namespace mv::util;

ClusterData::ClusterData(const mv::plugin::PluginFactory* factory) :
    mv::plugin::RawData(factory, ClusterType)
{
}

ClusterData::~ClusterData(void)
{
}

void ClusterData::init()
{
}

Dataset<DatasetImpl> ClusterData::createDataSet(const QString& guid /*= ""*/) const
{
    return Dataset<DatasetImpl>(new Clusters(getName(), false, guid));
}

QVector<Cluster>& ClusterData::getClusters()
{
    return _clusters;
}

const QVector<Cluster>& ClusterData::getClusters() const
{
    return _clusters;
}

void ClusterData::setClusters(const QVector<Cluster>&clusters)
{
    _clusters = clusters;
}

void ClusterData::addCluster(Cluster& cluster)
{
    _clusters.push_back(cluster);
}

void ClusterData::setClusterNames(const std::vector<QString>& clusterNames)
{
    if (clusterNames.empty())
        return;

    if (clusterNames.size() != _clusters.size())
    {
        qWarning() << "ClusterData: Number of cluster names does not equal the number of data cluster. No cluster names assigned.";
        return;
    }

    for (size_t i = 0; i < clusterNames.size(); i++)
        _clusters[i].setName(clusterNames[i]);
}

std::vector<QString> ClusterData::getClusterNames()
{
    std::vector<QString> clusterNames;
    clusterNames.reserve(_clusters.size());

    for (const auto& clusters : _clusters)
        clusterNames.push_back(clusters.getName());

    return clusterNames;
}

void ClusterData::removeClusterById(const QString& id)
{
    _clusters.erase(std::remove_if(_clusters.begin(), _clusters.end(), [id](const Cluster& cluster) -> bool
    {
        return cluster.getId() == id;
    }), _clusters.end());
}

void ClusterData::removeClustersById(const QStringList& ids)
{
    for (auto& clusterId : ids)
        removeClusterById(clusterId);
}

std::int32_t ClusterData::getClusterIndex(const QString& clusterName) const
{
    std::int32_t clusterIndex = 0;

    // Loop over all clusters and see if the name matches
    for (const auto& cluster : _clusters){

        if (clusterName == cluster.getName())
            return clusterIndex;

        clusterIndex++;
    }

    return -1;
}

UniqueWorkflowPlan ClusterData::fromVariantMapWorkflow(const QVariantMap& variantMap)
{
    auto plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    const auto dataMap = variantMap["Data"].toMap();

    plan->addNestedWorkflowStage("Load", [this, dataMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) -> UniqueWorkflowPlan {
        return ClustersSerializer::fromVariantMapWorkflow(dataMap, _clusters, executionContext);
    });

	return plan;
}

UniqueWorkflowPlan ClusterData::toVariantMapWorkflow() const
{
    auto plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    const auto baseSaveStage = plan->addNestedWorkflowStage("Save raw data base", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> UniqueWorkflowPlan {
        return this->Plugin::toVariantMapWorkflow();
    });

    plan->addSequentialStage("Preflight", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
        if (getClusters().size() > 500000) {
            executionContext->warning(QString("%1 clusters dataset contains approximately %2 clusters; very large numbers of clusters can take considerable time to save and load. Consider reducing the number of clusters if project serialization performance becomes a concern.").arg(getGuiName()).arg(getIntegerCountHumanReadable(getClusters().size())));
        }
    });

    const auto serializeClustersStage = plan->addNestedWorkflowStage("Serialize clusters", [this, baseSaveStage](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) -> UniqueWorkflowPlan {
        return ClustersSerializer::toVariantMapWorkflow(_clusters);
	});

    plan->addSequentialStage("Save data", [this, baseSaveStage, serializeClustersStage](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& executionContext) {
        auto outputMap = executionContext->takeOutput(baseSaveStage).toMap();

        outputMap["Data"] = executionContext->takeOutput(serializeClustersStage).toMap();

        executionContext->setOutput(outputMap);
    });

    return plan;
}

void Clusters::init()
{
    _infoAction = QSharedPointer<InfoAction>::create(nullptr, *this);

    addAction(*_infoAction.get());

    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetDataSelectionChanged));
    _eventListener.registerDataEventByType(ClusterType, [this](DatasetEvent* dataEvent) {

        // Only process selection changes
        if (dataEvent->getType() != EventType::DatasetDataSelectionChanged)
            return;

        // Do not process our own selection changes
        if (dataEvent->getDataset() == Dataset<Clusters>(this))
            return;

        // Only synchronize when our own group index is non-negative
        if (!mv::data().getSelectionGroupingAction()->isChecked() || getGroupIndex() < 0)
            return;

        if (dataEvent->getDataset()->getGroupIndex() == getGroupIndex() && dataEvent->getDataset()->getDataType() == ClusterType) {

            // Get smart pointer to foreign clusters dataset
            auto foreignClusters = dataEvent->getDataset<Clusters>();

            // Get names of the selected clusters in the foreign dataset
            const auto foreignSelectedClusterNames = foreignClusters->getSelectionNames();

            // Attempt to select clusters (fails if the clusters are the same)
            setSelectionNames(foreignSelectedClusterNames);
        }
    });

    setIconByName("table-cells-large");
}

void Clusters::addCluster(Cluster& cluster)
{
    getRawData<ClusterData>()->addCluster(cluster);
}

void Clusters::removeClusterById(const QString& id)
{
    getRawData<ClusterData>()->removeClusterById(id);
}

void Clusters::removeClustersById(const QStringList& ids)
{
    getRawData<ClusterData>()->removeClustersById(ids);
}

std::vector<std::uint32_t> Clusters::getSelectedIndices() const
{
    // Indices that are selected
    std::vector<std::uint32_t> selectedIndices;

    // Gather indices
    for (const auto& clusterIndex : getSelection<Clusters>()->indices) {

        // Get reference to selected cluster
        const auto selectedCluster = getClusters()[clusterIndex];

        // Add cluster indices to selected indices
        selectedIndices.insert(selectedIndices.end(), selectedCluster.getIndices().begin(), selectedCluster.getIndices().end());
    }

    // Remove duplicates
    std::sort(selectedIndices.begin(), selectedIndices.end());
    selectedIndices.erase(unique(selectedIndices.begin(), selectedIndices.end()), selectedIndices.end());

    return selectedIndices;
}

UniqueWorkflowPlan Clusters::fromVariantMapWorkflow(const QVariantMap& variantMap)
{
    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    plan->addNestedWorkflowStage("Load common", [this, variantMap](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& executionContext) mutable -> UniqueWorkflowPlan {
        return this->DatasetImpl::fromVariantMapWorkflow(variantMap);
    });

    plan->addNestedWorkflowStage("Load raw data", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) mutable -> UniqueWorkflowPlan {
		return getRawData<ClusterData>()->fromVariantMapWorkflow(variantMap);
    });

    return plan;
}

UniqueWorkflowPlan Clusters::toVariantMapWorkflow() const
{
    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    const auto saveDatasetBaseStage = plan->addNestedWorkflowStage("Save dataset base", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> UniqueWorkflowPlan {
        return this->DatasetImpl::toVariantMapWorkflow();
    });

    const auto encodeRawDataStage = plan->addNestedWorkflowStage("Encode raw data", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> UniqueWorkflowPlan {
        return getRawData<ClusterData>()->toVariantMapWorkflow();
    });

    const auto storeIndicesStage = plan->addSequentialStage("Save data", [this, saveDatasetBaseStage, encodeRawDataStage](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& executionContext) {
        auto outputMap = executionContext->takeOutput(saveDatasetBaseStage).toMap();

        outputMap.insert("Data", executionContext->takeOutput(encodeRawDataStage));

        executionContext->setOutput(outputMap);
    });

    return plan;
}

std::vector<std::uint32_t>& Clusters::getSelectionIndices()
{
    return getSelection<Clusters>()->indices;
}

void Clusters::setSelectionIndices(const std::vector<std::uint32_t>& indices)
{
    getSelection<Clusters>()->indices = indices;

    events().notifyDatasetDataSelectionChanged(this);

    if (!getDataHierarchyItem().getParent())
        return;

    // Get reference to input dataset
    auto points             = getDataHierarchyItem().getParent()->getDataset<Points>();
    auto selection          = points->getSelection<Points>();
    auto selectionIndices   = selection->indices;

    selectionIndices.clear();
    selectionIndices.reserve(indices.size());

    std::vector<std::uint32_t> globalIndices;

    points->getGlobalIndices(globalIndices);

    // Append point indices per cluster
    for (auto clusterSelectionIndex : getSelection<Clusters>()->indices) {
        const auto cluster = getClusters().at(clusterSelectionIndex);
        selectionIndices.insert(selectionIndices.end(), cluster.getIndices().begin(), cluster.getIndices().end());
    }

    // Remove duplicates
    std::sort(selectionIndices.begin(), selectionIndices.end());
    selectionIndices.erase(unique(selectionIndices.begin(), selectionIndices.end()), selectionIndices.end());

    points->setSelectionIndices(selectionIndices);

    events().notifyDatasetDataSelectionChanged(points);
}

QStringList Clusters::getSelectionNames() const
{
    QStringList clusterNames;

    for (const auto& selectedIndex : getSelection<Clusters>()->indices)
        clusterNames << getClusters()[selectedIndex].getName();

    return clusterNames;
}

void Clusters::setSelectionNames(const QStringList& clusterNames)
{
    // Exit if nothing changed
    if (clusterNames == getSelectionNames())
        return;

    // New selection indices
    std::vector<std::uint32_t> selectionIndices;

    for (const auto& clusterName : clusterNames) {

        // Get cluster index
        const auto clusterIndex = getRawData<ClusterData>()->getClusterIndex(clusterName);

        if (clusterIndex >= 0)
            selectionIndices.push_back(clusterIndex);
    }

    // Set the selection
    setSelectionIndices(selectionIndices);
}

bool Clusters::canSelect() const
{
    return getClusters().size() >= 1;
}

bool Clusters::canSelectAll() const
{
    return getSelectionSize() < getClusters().size();
}

bool Clusters::canSelectNone() const
{
    return getSelectionSize() >= 1;
}

bool Clusters::canSelectInvert() const
{
    return true;
}

void Clusters::selectAll()
{
    auto& selectionIndices = getSelectionIndices();

    selectionIndices.clear();
    selectionIndices.resize(getClusters().size());

    std::iota(selectionIndices.begin(), selectionIndices.end(), 0);

    events().notifyDatasetDataSelectionChanged(this);
}

void Clusters::selectNone()
{
    getSelectionIndices().clear();

    events().notifyDatasetDataSelectionChanged(this);
}

void Clusters::selectInvert()
{
    auto& selectionIndices = getSelectionIndices();

    std::set<std::uint32_t> selectionSet(selectionIndices.begin(), selectionIndices.end());

    const auto numberOfClusters = getClusters().size();

    selectionIndices.clear();
    selectionIndices.reserve(numberOfClusters - selectionSet.size());

    for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(numberOfClusters); i++) {
        if (selectionSet.find(i) == selectionSet.end())
            selectionIndices.push_back(i);
    }

    events().notifyDatasetDataSelectionChanged(this);
}

ClusterDataFactory::ClusterDataFactory()
{
    setIconByName("table-cells-large");
}

QUrl ClusterDataFactory::getReadmeMarkdownUrl() const
{
#ifdef ON_LEARNING_CENTER_FEATURE_BRANCH
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/feature/learning_center/ManiVault/src/plugins/ClusterData/README.md");
#else
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/master/ManiVault/src/plugins/ClusterData/README.md");
#endif
}

QUrl ClusterDataFactory::getRepositoryUrl() const
{
    return QUrl("https://github.com/ManiVaultStudio/core");
}

mv::plugin::RawData* ClusterDataFactory::produce()
{
    return new ClusterData(this);
}
