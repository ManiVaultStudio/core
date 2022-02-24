#include "DataHierarchyWidget.h"
#include "DataHierarchyModel.h"
#include "DataHierarchyModelItem.h"
#include "Core.h"
#include "Dataset.h"

#include <widgets/Divider.h>

#include <QDebug>
#include <QInputDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QMenu>

#include <stdexcept>

using namespace hdps::util;

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
    _dataImportAction(this),
    _datasetNameFilterAction(this, "Dataset name filter"),
    _expandAllAction(this, "Expand all"),
    _collapseAllAction(this, "Collapse all"),
    _groupingAction(this, "Grouping", Application::core()->isDatasetGroupingEnabled(), Application::core()->isDatasetGroupingEnabled())
{
    // Set filter model input model
    _filterModel.setSourceModel(&_model);

    // Set tree view input model
    _treeView.setModel(&_model);

    _treeView.setContextMenuPolicy(Qt::CustomContextMenu);
    _treeView.setSelectionModel(&_selectionModel);
    _treeView.setDragEnabled(true);
    _treeView.setDragDropMode(QAbstractItemView::DragOnly);
    _treeView.setSelectionMode(QAbstractItemView::SingleSelection);
    _treeView.setRootIsDecorated(true);
    _treeView.setItemsExpandable(true);

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

    // Remove the layout margin and spacing
    layout->setMargin(3);
    layout->setSpacing(3);

    // Create tool bar layout
    auto toolbarLayout = new QHBoxLayout();

    // Add toolbar items
    toolbarLayout->setContentsMargins(0, 3, 0, 3);
    toolbarLayout->setSpacing(4);
    //toolbarLayout->addWidget(_datasetNameFilterAction.createWidget(this), 1);
    toolbarLayout->addStretch(1);
    toolbarLayout->addWidget(_expandAllAction.createWidget(this, ToggleAction::PushButtonIcon));
    toolbarLayout->addWidget(_collapseAllAction.createWidget(this, ToggleAction::PushButtonIcon));
    toolbarLayout->addWidget(_groupingAction.createWidget(this, ToggleAction::Text));

    // Add tool bar layout and tree view widget
    layout->addLayout(toolbarLayout);
    layout->addWidget(&_treeView);

    // Apply layout
    setLayout(layout);

    // Update the data hierarchy filter model filter when the dataset filter name action changes
    connect(&_datasetNameFilterAction, &StringAction::stringChanged, this, [this](const QString& value) {
        _filterModel.setFilterRegExp(value);
    });

    // Notify others that the dataset selection changed when the current row in the model changed
    connect(&_selectionModel, &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection& deselected) {

        // Only proceed with a valid selection
        if (selected.indexes().isEmpty())
            return;
        
        // Get the globally unique identifier of the selected dataset
        const auto selectedDatasetId = selected.indexes().first().siblingAtColumn(static_cast<std::int32_t>(DataHierarchyModelItem::Column::GUID)).data(Qt::DisplayRole).toString();

        // Notify others that a dataset was selected
        emit selectedDatasetChanged(selectedDatasetId);
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

        // Notify others that a dataset was selected
        emit selectedDatasetChanged("");
    });

    // Invoked the custom context menu when requested by the tree view
    connect(&_treeView, &QTreeView::customContextMenuRequested, this, [this](const QPoint& position) {

        // Get the model index at the required position
        const auto modelIndexBelowCursor = _treeView.indexAt(position);

        // Show dataset context menu when over a dataset, give options to load data otherwise
        if (modelIndexBelowCursor.isValid())
        {
            // Get pointer to data hierarchy item
            auto dataHierarchyModelItem = _model.getItem(modelIndexBelowCursor, Qt::DisplayRole);

            // Get the data hierarchy item context menu
            QSharedPointer<QMenu> contextMenu(dataHierarchyModelItem->getContextMenu());

            // And show it
            contextMenu->exec(_treeView.viewport()->mapToGlobal(position));
        }
        else {

            // Get dataset import context menu
            auto contextMenu = _dataImportAction.getContextMenu();

            // Show it
            if (contextMenu)
                contextMenu->exec(_treeView.viewport()->mapToGlobal(position));
        }
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

            // Notify others that the description changed
            _model.setData(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Description), description);
        });

        // Update the model when the data hierarchy item task progress changes
        connect(&dataHierarchyItem, &DataHierarchyItem::taskProgressChanged, this, [this, dataset](const float& progress) {

            //qDebug() << "DataHierarchyItem::taskProgressChanged" << progress;

            // Notify others that the progress changed
            _model.setData(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Progress), progress);
            _model.setData(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Analyzing), progress > 0.0f);
        });

        // Update the model when the data hierarchy item selection status changes
        connect(&dataHierarchyItem, &DataHierarchyItem::selectionChanged, this, [this, dataset](const bool& selection) {
            _selectionModel.select(getModelIndexByDataset(dataset), QItemSelectionModel::SelectionFlag::ClearAndSelect | QItemSelectionModel::SelectionFlag::Rows);
        });

        // Update the model when the data hierarchy item locked status changes
        connect(&dataHierarchyItem, &DataHierarchyItem::lockedChanged, this, [this, dataset](const bool& locked) {

            // Notify others that the locked state changed
            emit _model.dataChanged(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Name), getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Locked));
        });

        // Update the model when the data hierarchy item expanded status changes
        connect(&dataHierarchyItem, &DataHierarchyItem::expandedChanged, this, [this, dataset](const bool& expanded) {

            // Get the model index for the dataset
            const auto modelIndex = getModelIndexByDataset(dataset);

            // No point in updating the tree view when either the model index is invalid or the expansion state did not change
            if (!modelIndex.isValid() || expanded == _treeView.isExpanded(modelIndex))
                return;

            // Set model item expanded
            _treeView.setExpanded(modelIndex, expanded);
        });

        //_selectionModel.select(getModelIndexByDataset(dataset), QItemSelectionModel::SelectionFlag::ClearAndSelect | QItemSelectionModel::SelectionFlag::Rows);

        // Notify others that a dataset was selected
        emit selectedDatasetChanged(dataset->getGuid());
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

    layout->setMargin(0);
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
