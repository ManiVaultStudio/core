#include "ClustersAction.h"
#include "DataHierarchyItem.h"
#include "ClusterData.h"
#include "PointData.h"

#include <QTreeView>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVariant>
#include <QFile>
#include <QFileDialog>

using namespace hdps;
using namespace hdps::gui;

ClustersAction::ClustersAction(QObject* parent, const QString& datasetName) :
    WidgetAction(parent),
    EventListener(),
    _clusters(datasetName),
    _clustersModel(),
    _importAction(this, "Import"),
    _exportAction(this, "Export")
{
    setText("Clusters");
    setEventCore(Application::core());

    _importAction.setToolTip("Import clusters from file");
    _exportAction.setToolTip("Export clusters to file");

    //_importAction.setIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));
    //_exportAction.setIcon(Application::getIconFont("FontAwesome").getIcon("file-export"));

    registerDataEventByType(ClusterType, [this](hdps::DataEvent* dataEvent) {
        if (!_clusters.isValid())
            return;

        if (dataEvent->dataSetName != _clusters->getName())
            return;

        switch (dataEvent->getType())
        {
            case EventType::DataChanged:
            {
                _clustersModel.setClusters(*getClusters());
                break;
            }

            case EventType::SelectionChanged:
            {
                break;
            }

            default:
                break;
        }
    });

    const auto updateClusters = [this]() -> void {
        _clusters->getClusters() = _clustersModel.getClusters();
        _clusters.notifyDataChanged();
    };

    connect(&_clustersModel, &QAbstractItemModel::dataChanged, this, [this, updateClusters](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>()) {
        if (!_clusters.isValid())
            return;

        updateClusters();
    });

    connect(&_clustersModel, &QAbstractItemModel::layoutChanged, this, [this, updateClusters](const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint) {
        if (!_clusters.isValid())
            return;

        updateClusters();
    });

    connect(&_importAction, &TriggerAction::triggered, this, [this]() {

        try
        {
            QFileDialog fileDialog;

            // Configure file dialog
            fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
            fileDialog.setNameFilters({ "Cluster JSON files (*json)" });
            fileDialog.setDefaultSuffix(".json");

            // Show the dialog
            if (fileDialog.exec() == 0)
                return;

            if (fileDialog.selectedFiles().count() != 1)
                return;

            QFile clustersFile;

            // Load the file
            clustersFile.setFileName(fileDialog.selectedFiles().first());
            clustersFile.open(QIODevice::ReadOnly);

            // Get the cluster data
            QByteArray clustersData = clustersFile.readAll();

            QJsonDocument clusterJsonDocument;

            // Convert to JSON document
            clusterJsonDocument = QJsonDocument::fromJson(clustersData);
            
            // Load in the cluster from variant data
            _clusters->fromVariant(clusterJsonDocument.toVariant());

            // Let others know that the clusters changed
            _clusters.notifyDataChanged();
        }
        catch (std::exception& e)
        {
            QMessageBox::critical(nullptr, QString("Unable to load clusters"), e.what(), QMessageBox::Ok);
        }
    });

    connect(&_exportAction, &TriggerAction::triggered, this, [this]() {

        try
        {
            // Create JSON document from clusters variant map
            QJsonDocument document(QJsonArray::fromVariantList(_clusters->toVariant().toList()));

            // Show the dialog
            QFileDialog fileDialog;

            // Configure file dialog
            fileDialog.setAcceptMode(QFileDialog::AcceptSave);
            fileDialog.setNameFilters({ "Cluster JSON files (*json)" });
            fileDialog.setDefaultSuffix(".json");
            
            if (fileDialog.exec() == 0)
                return;

            // Only save if we have one file
            if (fileDialog.selectedFiles().count() != 1)
                return;

            QFile jsonFile(fileDialog.selectedFiles().first());

            // Save the file
            jsonFile.open(QFile::WriteOnly);
            jsonFile.write(document.toJson());
        }
        catch (std::exception& e)
        {
            QMessageBox::critical(nullptr, QString("Unable to save clusters"), e.what(), QMessageBox::Ok);
        }
    });
}

std::vector<Cluster>* ClustersAction::getClusters()
{
    if (!_clusters.isValid())
        return nullptr;

    return &_clusters->getClusters();
}

DatasetRef<Clusters>& ClustersAction::getClustersDataset()
{
    return _clusters;
}

void ClustersAction::selectPoints(const std::vector<std::uint32_t>& indices)
{
    if (!_clusters.isValid())
        return;
    
    auto dataHierarchyItem          = Application::core()->getDataHierarchyItem(_clusters->getName());
    auto parentDataHierarchyItem    = dataHierarchyItem->getParent();
    auto& points                    = parentDataHierarchyItem->getDataset<Points>();
    auto& selection                 = dynamic_cast<Points&>(points.getSelection());

    selection.indices.clear();
    selection.indices.reserve(indices.size());

    std::vector<std::uint32_t> globalIndices;

    points.getGlobalIndices(globalIndices);

    for (auto index : indices)
        selection.indices.push_back(globalIndices[index]);

    Application::core()->notifySelectionChanged(parentDataHierarchyItem->getDatasetName());
}

void ClustersAction::createSubset(const QString& datasetName)
{
    auto dataHierarchyItem = Application::core()->getDataHierarchyItem(_clusters->getName());
    
    DatasetRef<Points> points(dataHierarchyItem->getParent()->getDatasetName());

    auto& selection = dynamic_cast<Points&>(points->getSelection());

    const auto subsetName = points->createSubset(datasetName, points->getName());
}

void ClustersAction::removeClustersById(const QStringList& ids)
{
    _clustersModel.removeClustersById(ids);
}

ClustersModel& ClustersAction::getClustersModel()
{
    return _clustersModel;
}

ClustersAction::Widget::Widget(QWidget* parent, ClustersAction* clustersAction) :
    WidgetActionWidget(parent, clustersAction),
    _filterModel(),
    _selectionModel(&_filterModel),
    _removeAction(this, "Remove"),
    _mergeAction(this, "Merge"),
    _filterAndSelectAction(this, _filterModel, _selectionModel),
    _subsetAction(this, *clustersAction, _filterModel, _selectionModel)
{
    setEventCore(Application::core());

    _removeAction.setToolTip("Remove the selected clusters");
    _mergeAction.setToolTip("Merge the selected clusters");

    _removeAction.setEnabled(false); 

    //_removeAction.setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("trash-alt"));
    //_mergeAction.setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("object-group"));

    _removeAction.setToolTip("Remove the selected filter(s)");
    _mergeAction.setToolTip("Merge the selected filter(s)");

    _filterModel.setSourceModel(&clustersAction->getClustersModel());
    _filterModel.setDynamicSortFilter(true);
    _filterModel.setFilterKeyColumn(static_cast<std::int32_t>(ClustersModel::Column::Name));

    auto clustersTreeView = new QTreeView();

    // Configure tree view
    clustersTreeView->setModel(&_filterModel);
    clustersTreeView->setSelectionModel(&_selectionModel);
    clustersTreeView->setFixedHeight(150);
    clustersTreeView->setRootIsDecorated(false);
    clustersTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    clustersTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    clustersTreeView->setSortingEnabled(true);
    clustersTreeView->sortByColumn(static_cast<std::int32_t>(ClustersModel::Column::Name), Qt::SortOrder::AscendingOrder);

    // Configure header view
    auto header = clustersTreeView->header();

    header->setStretchLastSection(false);
    header->hideSection(static_cast<std::int32_t>(ClustersModel::Column::ID));

    header->setMinimumSectionSize(20);

    header->resizeSection(static_cast<std::int32_t>(ClustersModel::Column::Color), 20);
    header->resizeSection(static_cast<std::int32_t>(ClustersModel::Column::Name), 200);
    header->resizeSection(static_cast<std::int32_t>(ClustersModel::Column::NumberOfIndices), 100);

    header->setSectionResizeMode(static_cast<std::int32_t>(ClustersModel::Column::Color), QHeaderView::Fixed);
    header->setSectionResizeMode(static_cast<std::int32_t>(ClustersModel::Column::Name), QHeaderView::Stretch);
    header->setSectionResizeMode(static_cast<std::int32_t>(ClustersModel::Column::NumberOfIndices), QHeaderView::Fixed);
    
    const auto selectionChangedHandler = [this, clustersAction, clustersTreeView]() -> void {

        // Get selected row
        const auto selectedRows = clustersTreeView->selectionModel()->selectedRows();

        // Get reference to cluster selection set
        auto& currentClusterSelectionIndices = dynamic_cast<Clusters&>(clustersAction->getClustersDataset()->getSelection()).indices;

        // Clear and reserve the selection indices
        currentClusterSelectionIndices.clear();
        currentClusterSelectionIndices.reserve(clustersAction->getClusters()->size());

        // Gather point indices for selection
        for (auto selectedIndex : selectedRows) {
            auto cluster = static_cast<Cluster*>(_filterModel.mapToSource(selectedIndex).internalPointer());

            // Add selected index
            currentClusterSelectionIndices.push_back(selectedIndex.row());
        }

        // Select points
        clustersAction->selectPoints(clustersAction->getClustersDataset()->getSelectedIndices());

        // Update state of the remove action
        _removeAction.setEnabled(!selectedRows.isEmpty());
        _mergeAction.setEnabled(selectedRows.count() >= 2);

        // Notify others that the cluster selection has changed
        Application::core()->notifySelectionChanged(clustersAction->getClustersDataset()->getName());
    };

    connect(clustersTreeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this, selectionChangedHandler](const QItemSelection& selected, const QItemSelection& deselected) {
        selectionChangedHandler();
    });
    
    // Clear the selection when the layout of the model changes
    connect(clustersTreeView->model(), &QAbstractItemModel::layoutChanged, this, [this, clustersTreeView](const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint) {
        clustersTreeView->selectionModel()->reset();
    });

    connect(clustersTreeView, &QTreeView::doubleClicked, this, [this, clustersAction, clustersTreeView](const QModelIndex& index) {
        const auto colorColumn = static_cast<std::int32_t>(ClustersModel::Column::Color);

        if (index.column() != colorColumn)
            return;

        // Get sibling color model index and the current cluster color
        const auto colorIndex   = _filterModel.mapToSource(index).siblingAtColumn(colorColumn);
        const auto currentColor = colorIndex.data(Qt::EditRole).value<QColor>();
        
        // Update the clusters model with the new color
        clustersAction->getClustersModel().setData(colorIndex, QColorDialog::getColor(currentColor));
    });

    auto toolbarLayout = new QHBoxLayout();

    toolbarLayout->addWidget(_removeAction.createWidget(this), 1);
    toolbarLayout->addWidget(_mergeAction.createWidget(this), 1);
    toolbarLayout->addWidget(clustersAction->getImportAction().createWidget(this), 1);
    toolbarLayout->addWidget(clustersAction->getExportAction().createWidget(this), 1);

    auto mainLayout = new QVBoxLayout();

    mainLayout->setMargin(0);
    mainLayout->addWidget(clustersTreeView);

    mainLayout->addWidget(_filterAndSelectAction.createWidget(this));
    mainLayout->addLayout(toolbarLayout);
    mainLayout->addWidget(_subsetAction.createWidget(this));

    setLayout(mainLayout);

    connect(&_removeAction, &TriggerAction::triggered, this, [this, clustersAction, clustersTreeView]() {
        const auto selectedRows = clustersTreeView->selectionModel()->selectedRows();

        QStringList clusterIds;

        for (auto selectedIndex : selectedRows)
            clusterIds << _filterModel.mapToSource(selectedIndex).siblingAtColumn(static_cast<std::int32_t>(ClustersModel::Column::ID)).data(Qt::DisplayRole).toString();

        clustersAction->removeClustersById(clusterIds);
    });

    connect(&_mergeAction, &TriggerAction::triggered, this, [this, clustersAction, clustersTreeView]() {
        const auto selectedRows = clustersTreeView->selectionModel()->selectedRows();

        QStringList clusterIdsToRemove;

        auto mergeCluster = static_cast<Cluster*>(_filterModel.mapToSource(selectedRows.first()).internalPointer());

        mergeCluster->setName(QString("%1*").arg(mergeCluster->getName()));

        for (auto selectedIndex : selectedRows) {
            auto cluster = static_cast<Cluster*>(_filterModel.mapToSource(selectedIndex).internalPointer());

            if (selectedIndex == selectedRows.first())
                continue;

            clusterIdsToRemove << cluster->getId();

            mergeCluster->getIndices().insert(mergeCluster->getIndices().end(), cluster->getIndices().begin(), cluster->getIndices().end());
        }

        clustersAction->removeClustersById(clusterIdsToRemove);
    });
}
