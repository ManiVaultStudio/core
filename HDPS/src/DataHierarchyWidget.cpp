#include "DataHierarchyWidget.h"
#include "DataHierarchyModel.h"
#include "DataHierarchyModelItem.h"
#include "Core.h"
#include "Dataset.h"
#include "PluginFactory.h"

#include <widgets/Divider.h>

#include <QDebug>
#include <QInputDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QMenu>
#include <QLabel>

#include <stdexcept>

using namespace hdps::util;
using namespace hdps::plugin;

namespace hdps
{

namespace gui
{

DataHierarchyWidget::DataHierarchyWidget(QWidget* parent) :
    QWidget(parent),
    _model(this),
    _filterModel(this),
    _treeView(this),
    _selectionModel(&_model),
    _noDataOverlayWidget(new NoDataOverlayWidget(this)),
    _datasetNameFilterAction(this, "Dataset name filter"),
    _expandAllAction(this, "Expand all"),
    _collapseAllAction(this, "Collapse all"),
    _groupingAction(this, "Selection grouping", Application::core()->isDatasetGroupingEnabled(), Application::core()->isDatasetGroupingEnabled())
{
    // Set filter model input model
    _filterModel.setSourceModel(&_model);

    // Set tree view input model
    _treeView.setModel(&_model);

    _treeView.setContextMenuPolicy(Qt::CustomContextMenu);
    _treeView.setSelectionModel(&_selectionModel);
    _treeView.setDragEnabled(true);
    _treeView.setDragDropMode(QAbstractItemView::DragOnly);
    _treeView.setSelectionBehavior(QAbstractItemView::SelectRows);
    _treeView.setSelectionMode(QAbstractItemView::ExtendedSelection);
    _treeView.setRootIsDecorated(true);
    _treeView.setItemsExpandable(true);
    _treeView.setIconSize(QSize(14, 14));

    _treeView.header()->setStretchLastSection(false);
    _treeView.header()->setMinimumSectionSize(18);

    _treeView.header()->resizeSection(DataHierarchyModelItem::Column::Name, 180);
    //header()->resizeSection(DataHierarchyModelItem::Column::GUID, 100);
    _treeView.header()->resizeSection(DataHierarchyModelItem::Column::GroupIndex, 60);
    //header()->resizeSection(DataHierarchyModelItem::Column::Description, 100);
    _treeView.header()->resizeSection(DataHierarchyModelItem::Column::Progress, 45);
    _treeView.header()->resizeSection(DataHierarchyModelItem::Column::Analyzing, _treeView.header()->minimumSectionSize());
    _treeView.header()->resizeSection(DataHierarchyModelItem::Column::Locked, _treeView.header()->minimumSectionSize());

    _treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::Name, QHeaderView::Interactive);
    _treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::GUID, QHeaderView::Fixed);
    _treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::GroupIndex, QHeaderView::Fixed);
    _treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::Description, QHeaderView::Stretch);
    _treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::Progress, QHeaderView::Fixed);
    _treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::Analyzing, QHeaderView::Fixed);
    _treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::Locked, QHeaderView::Fixed);

    _datasetNameFilterAction.setPlaceHolderString("Search by name...");

    // Configure expand all action
    _expandAllAction.setIcon(Application::getIconFont("FontAwesome").getIcon("angle-double-down"));
    _expandAllAction.setToolTip("Expand all datasets in the hierarchy");

    // Expand all items when the expand all action is triggered
    connect(&_expandAllAction, &TriggerAction::triggered, this, &DataHierarchyWidget::expandAll);

    // Configure collapse all action
    _collapseAllAction.setIcon(Application::getIconFont("FontAwesome").getIcon("angle-double-up"));
    _collapseAllAction.setToolTip("Collapse all datasets in the hierarchy");

    // Collapse all items when the collapse all action is triggered
    connect(&_collapseAllAction, &TriggerAction::triggered, this, &DataHierarchyWidget::collapseAll);

    // Configure grouping action
    _groupingAction.setIcon(Application::getIconFont("FontAwesome").getIcon("object-group"));
    _groupingAction.setToolTip("Enable/disable dataset grouping");

    // Update columns visibility when grouping is editable/disabled
    connect(&_groupingAction, &ToggleAction::toggled, this, &DataHierarchyWidget::onGroupingActionToggled);

    // Create layout that will contain the toolbar and the tree view
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(3);

    // Create tool bar layout
    auto toolbarLayout = new QHBoxLayout();

    // Add toolbar items
    toolbarLayout->setContentsMargins(0, 3, 0, 3);
    toolbarLayout->setSpacing(4);
    //toolbarLayout->addWidget(_datasetNameFilterAction.createWidget(this), 1);
    toolbarLayout->addWidget(_expandAllAction.createWidget(this, ToggleAction::PushButtonIcon));
    toolbarLayout->addWidget(_collapseAllAction.createWidget(this, ToggleAction::PushButtonIcon));
    toolbarLayout->addStretch(1);
    toolbarLayout->addWidget(_groupingAction.createWidget(this, ToggleAction::CheckBox));

    // Add tool bar layout and tree view widget
    layout->addLayout(toolbarLayout);
    layout->addWidget(&_treeView);

    // Apply layout
    setLayout(layout);

    // Update the data hierarchy filter model filter when the dataset filter name action changes
    connect(&_datasetNameFilterAction, &StringAction::stringChanged, this, [this](const QString& value) {
        _filterModel.setFilterRegularExpression(value);
    });

    connect(&_selectionModel, &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection& deselected) {
        /*
        if (selected.indexes().isEmpty())
            return;

        QSet<DataHierarchyItem*> dataHierarchyItems;

        // Gather selected data hierarchy items
        for (const auto& index : selected.indexes())
            dataHierarchyItems.insert(_model.getItem(selected.indexes().first(), Qt::DisplayRole)->getDataHierarchyItem());

        // Select the items in the data hierarchy
        auto selectedItems = QVector<DataHierarchyItem*>(dataHierarchyItems.begin(), dataHierarchyItems.end());
        Application::core()->getDataHierarchyManager().selectItems(selectedItems);
        */
    });

    // Show/hide the overlay and header widget when the number of rows changes
    connect(&_model, &QAbstractItemModel::rowsInserted, this, &DataHierarchyWidget::numberOfRowsChanged);
    connect(&_model, &QAbstractItemModel::rowsRemoved, this, &DataHierarchyWidget::numberOfRowsChanged);

    // Handle item expanded/collapsed
    connect(&_treeView, &QTreeView::expanded, this, &DataHierarchyWidget::expanded);
    connect(&_treeView, &QTreeView::collapsed, this, &DataHierarchyWidget::collapsed);

    // Add data hierarchy item to the widget when added in the data manager
    connect(&Application::core()->getDataHierarchyManager(), &DataHierarchyManager::itemAdded, this, &DataHierarchyWidget::addDataHierarchyItem);

    // Remove model item when a data hierarchy item is removed
    connect(&Application::core()->getDataHierarchyManager(), &DataHierarchyManager::itemAboutToBeRemoved, this, [this](const Dataset<DatasetImpl>& dataset) {

        // Clear the selection
        _selectionModel.clear();

        // Remove the item from the data hierarchy
        _model.removeDataHierarchyModelItem(getModelIndexByDataset(dataset));
    });

    // Invoked the custom context menu when requested by the tree view
    connect(&_treeView, &QTreeView::customContextMenuRequested, this, [this](const QPoint& position) {

        const auto selectedRows = _selectionModel.selectedRows();

        Datasets selectedDatasets;

        for (const auto& selectedRow : selectedRows)
            selectedDatasets << _model.getItem(selectedRow, Qt::DisplayRole)->getDataHierarchyItem()->getDataset();

        if (selectedDatasets.isEmpty())
            return;

        auto contextMenu = new QMenu();

        const auto addMenu = [contextMenu, selectedDatasets](const plugin::Type& pluginType) -> void {
            auto menu = new QMenu();

            switch (pluginType)
            {
                case plugin::Type::ANALYSIS:
                {
                    menu->setTitle("Analyze");
                    menu->setIcon(Application::getIconFont("FontAwesome").getIcon("square-root-alt"));
                    break;
                }

                case plugin::Type::LOADER:
                {
                    menu->setTitle("Import");
                    menu->setIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));
                    break;
                }

                case plugin::Type::WRITER:
                {
                    menu->setTitle("Export");
                    menu->setIcon(Application::getIconFont("FontAwesome").getIcon("file-export"));
                    break;
                }

                case plugin::Type::TRANSFORMATION:
                {
                    menu->setTitle("Transform");
                    menu->setIcon(Application::getIconFont("FontAwesome").getIcon("random"));
                    break;
                }

                case plugin::Type::VIEW:
                {
                    menu->setTitle("View");
                    menu->setIcon(Application::getIconFont("FontAwesome").getIcon("eye"));
                    break;
                }

                default:
                    break;
            }

            for (auto pluginProducerAction : Application::core()->getPluginActionsByPluginTypeAndDatasets(pluginType, selectedDatasets))
                menu->addAction(pluginProducerAction);

            if (!menu->actions().isEmpty())
                contextMenu->addMenu(menu);
        };

        addMenu(plugin::Type::ANALYSIS);
        addMenu(plugin::Type::LOADER);
        addMenu(plugin::Type::WRITER);
        addMenu(plugin::Type::TRANSFORMATION);
        addMenu(plugin::Type::VIEW);
        
        auto groupDataAction = new QAction("Group");

        groupDataAction->setToolTip("Group datasets into one");
        groupDataAction->setIcon(Application::getIconFont("FontAwesome").getIcon("layer-group"));

        connect(groupDataAction, &QAction::triggered, [this, selectedDatasets]() -> void {
            Application::core()->groupData(selectedDatasets, "Group");
        });

        contextMenu->addAction(groupDataAction);

        contextMenu->exec(_treeView.viewport()->mapToGlobal(position));
    });

    // Update tool bar when items got expanded/collapsed
    connect(&_treeView, &QTreeView::expanded, this, &DataHierarchyWidget::updateToolBar);
    connect(&_treeView, &QTreeView::collapsed, this, &DataHierarchyWidget::updateToolBar);

    // Initial visibility of the overlay and header
    numberOfRowsChanged();

    // Initial update of tree view columns visibility
    updateColumnsVisibility();
}

void DataHierarchyWidget::addDataHierarchyItem(DataHierarchyItem& dataHierarchyItem)
{
    // Do not add hidden data hierarchy items
    if (dataHierarchyItem.isHidden())
        return;

    // Get smart pointer to the dataset
    auto dataset = dataHierarchyItem.getDataset();

    try {

        // Model index of the parent
        QModelIndex parentModelIndex;

        // Establish parent model index
        if (!dataHierarchyItem.hasParent())
            parentModelIndex = QModelIndex();
        else
            parentModelIndex = getModelIndexByDataset(dataHierarchyItem.getParent().getDataset());;

        // Add the data hierarchy item to the model
        _model.addDataHierarchyModelItem(parentModelIndex, dataHierarchyItem);

        // Update the model when the data hierarchy item task description changes
        connect(&dataHierarchyItem, &DataHierarchyItem::taskDescriptionChanged, this, [this, dataset](const QString& description) {
            _model.setData(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Description), description);
        });

        // Update the model when the data hierarchy item task progress changes
        connect(&dataHierarchyItem, &DataHierarchyItem::taskProgressChanged, this, [this, dataset](const float& progress) {
            _model.setData(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Progress), progress);
            _model.setData(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Analyzing), progress > 0.0f);
        });

        // Update the model when the data hierarchy item selection status changes
        connect(&dataHierarchyItem, &DataHierarchyItem::selectionChanged, this, [this, dataset](const bool& selection) {
            if (!selection)
                return;

            if (_selectionModel.selectedRows().contains(getModelIndexByDataset(dataset)))
                return;

            _selectionModel.select(getModelIndexByDataset(dataset), QItemSelectionModel::SelectionFlag::ClearAndSelect | QItemSelectionModel::SelectionFlag::Rows);
        });

        // Update the model when the data hierarchy item locked status changes
        connect(&dataHierarchyItem, &DataHierarchyItem::lockedChanged, this, [this, dataset](const bool& locked) {
            emit _model.dataChanged(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Name), getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Locked));
        });

        // Update the model when the data hierarchy item icon changes
        connect(&dataHierarchyItem, &DataHierarchyItem::iconChanged, this, [this, dataset](const QString& name, const QIcon& icon) {
            emit _model.dataChanged(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Name), getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Name));
        });

        // Set model item expansion
        const auto setModelItemExpansion = [this](const QModelIndex& modelIndex, const bool& expanded) -> void {

            // No point in updating the tree view when either the model index is invalid or the expansion state did not change
            if (!modelIndex.isValid() || expanded == _treeView.isExpanded(modelIndex))
                return;

            // Set model item expanded
            _treeView.setExpanded(modelIndex, expanded);
        };

        // Update the model when the data hierarchy item expansion changes
        connect(&dataHierarchyItem, &DataHierarchyItem::expandedChanged, this, [this, dataset, setModelItemExpansion](const bool& expanded) {
            setModelItemExpansion(getModelIndexByDataset(dataset), expanded);
        });

        // Wait for the item to be added to the tree view
        QCoreApplication::processEvents();

        // And then set the model item expansion
        setModelItemExpansion(getModelIndexByDataset(dataset), dataHierarchyItem.isExpanded());
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to add %1 to the data hierarchy tree widget").arg(dataset->getGuiName()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to add %1 to the data hierarchy tree widget").arg(dataset->getGuiName()));
    }
}

QModelIndex DataHierarchyWidget::getModelIndexByDataset(const Dataset<DatasetImpl>& dataset)
{
    // Search for the model index
    const auto modelIndices = _model.match(_model.index(0, 1), Qt::DisplayRole, dataset->getGuid(), 1, Qt::MatchFlag::MatchRecursive);

    // Except if not found
    if (modelIndices.isEmpty())
        throw new std::runtime_error(QString("'%1' not found in the data hierarchy model").arg(dataset->getGuiName()).toLatin1());

    // Return first match
    return modelIndices.first();
}

void DataHierarchyWidget::numberOfRowsChanged()
{
    // Establish whether any data is loaded
    const auto dataIsLoaded = _model.rowCount() >= 1;

    // Show the no data overlay widget when no data is loaded
    _noDataOverlayWidget->setVisible(!dataIsLoaded);

    // Show the header when data is loaded
    _treeView.setHeaderHidden(!dataIsLoaded);

    // Update the toolbar
    updateToolBar();
}

void DataHierarchyWidget::onGroupingActionToggled(const bool& toggled)
{
    // Set in the core whether datasets may be grouped or not
    Application::core()->setDatasetGroupingEnabled(toggled);

    // Update the visibility of the tree view columns
    updateColumnsVisibility();
}

void DataHierarchyWidget::updateColumnsVisibility()
{
    _treeView.setColumnHidden(DataHierarchyModelItem::Column::GUID, true);
    _treeView.setColumnHidden(DataHierarchyModelItem::Column::Analysis, true);
    _treeView.setColumnHidden(DataHierarchyModelItem::Column::GroupIndex, !_groupingAction.isChecked());
}

QModelIndexList DataHierarchyWidget::getModelIndexList(QModelIndex parent /*= QModelIndex()*/) const
{
    QModelIndexList modelIndexList;

    if (parent.isValid())
        modelIndexList << parent;

    // Loop over all children of parent
    for (int rowIndex = 0; rowIndex < _model.rowCount(parent); ++rowIndex) {

        // Get model index of child
        QModelIndex index = _model.index(rowIndex, 0, parent);

        // Recursive
        if (_model.hasChildren(index))
            modelIndexList << getModelIndexList(index);
    }

    return modelIndexList;
}

bool DataHierarchyWidget::mayExpandAll() const
{
    // Get all model indices in the model
    const auto allModelIndices = getModelIndexList();

    // Loop over all indices and return true if one item is collapsed
    for (const auto& modelIndex : allModelIndices) {
        if (_model.rowCount(modelIndex) > 0 && !_treeView.isExpanded(modelIndex))
            return true;
    }

    return false;
}

void DataHierarchyWidget::expandAll()
{
    // Get all model indices in the model
    const auto allModelIndices = getModelIndexList();

    // Loop over all indices and expand them
    for (const auto& modelIndex : allModelIndices)
        _treeView.setExpanded(modelIndex, true);
}

bool DataHierarchyWidget::mayCollapseAll() const
{
    // Get all model indices in the model
    const auto allModelIndices = getModelIndexList();

    // Loop over all indices and return true if one item is expanded
    for (const auto& modelIndex : allModelIndices)
        if (_model.rowCount(modelIndex) > 0 && _treeView.isExpanded(modelIndex))
            return true;

    return false;
}

void DataHierarchyWidget::collapseAll()
{
    // Get all model indices in the model
    const auto allModelIndices = getModelIndexList();

    // Loop over all indices and collapse them
    for (const auto& modelIndex : allModelIndices)
        _treeView.setExpanded(modelIndex, false);
}

void DataHierarchyWidget::expanded(const QModelIndex& index)
{
    // Only process valid model index
    if (!index.isValid())
        return;

    // Get pointer to data hierarchy item
    auto dataHierarchyItem = _model.getItem(index, Qt::DisplayRole)->getDataHierarchyItem();

    // Expand the data hierarchy item
    dataHierarchyItem->setExpanded(true);
}

void DataHierarchyWidget::collapsed(const QModelIndex& index)
{
    // Only process valid model index
    if (!index.isValid())
        return;

    // Get pointer to data hierarchy item
    auto dataHierarchyItem = _model.getItem(index, Qt::DisplayRole)->getDataHierarchyItem();

    // Collapse the data hierarchy item
    dataHierarchyItem->setExpanded(false);
}

void DataHierarchyWidget::updateToolBar()
{
    // Establish whether any data is loaded
    const auto dataIsLoaded = _model.rowCount() >= 1;

    // Enable/disable the items
    _expandAllAction.setEnabled(dataIsLoaded && mayExpandAll());
    _collapseAllAction.setEnabled(dataIsLoaded && mayCollapseAll());
    _groupingAction.setEnabled(dataIsLoaded);
}

DataHierarchyWidget::NoDataOverlayWidget::NoDataOverlayWidget(QWidget* parent) :
    QWidget(parent),
    _opacityEffect(new QGraphicsOpacityEffect(this))
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setGraphicsEffect(_opacityEffect);

    _opacityEffect->setOpacity(0.35);

    auto layout             = new QVBoxLayout();
    auto iconLabel          = new QLabel();
    auto titleLabel         = new QLabel("No data loaded");
    auto descriptionLabel   = new QLabel("Right-click to import data");

    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setFont(fontAwesome.getFont(14));
    iconLabel->setText(fontAwesome.getIconCharacter("database"));
    iconLabel->setStyleSheet("QLabel { padding: 10px; }");

    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold");

    descriptionLabel->setAlignment(Qt::AlignCenter);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignCenter);

    layout->addStretch(1);
    layout->addWidget(iconLabel);
    layout->addWidget(titleLabel);
    layout->addWidget(descriptionLabel);
    layout->addStretch(1);

    setLayout(layout);

    // Install event filter for synchronizing widget size
    parent->installEventFilter(this);

    setObjectName("NoDataOverlayWidget");
    setStyleSheet("QWidget#NoDataOverlayWidget > QLabel { color: gray; }");
}

bool DataHierarchyWidget::NoDataOverlayWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        // Resize event
        case QEvent::Resize:
        {
            if (dynamic_cast<QWidget*>(target) != parent())
                break;

            // Set fixed overlay size
            setFixedSize(static_cast<QResizeEvent*>(event)->size());

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

}
}
