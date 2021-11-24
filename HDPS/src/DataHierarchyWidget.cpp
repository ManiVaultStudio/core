#include "DataHierarchyWidget.h"
#include "DataHierarchyModel.h"
#include "DataHierarchyModelItem.h"
#include "Core.h"
#include "Dataset.h"

#include <QDebug>
#include <QInputDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QResizeEvent>
#include <QVBoxLayout>

#include <stdexcept>

using namespace hdps::util;

namespace hdps
{

namespace gui
{

DataHierarchyWidget::DataHierarchyWidget(QWidget* parent) :
    QTreeView(parent),
    _model(this),
    _selectionModel(&_model),
    _noDataOverlayWidget(new NoDataOverlayWidget(this)),
    _dataImportAction(this)
{
    //setMinimumWidth(500);
    setModel(&_model);
    
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionModel(&_selectionModel);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setRootIsDecorated(true);
    setItemsExpandable(true);

    // Hide columns
    setColumnHidden(DataHierarchyModelItem::Column::GUID, true);
    setColumnHidden(DataHierarchyModelItem::Column::Analysis, true);

    header()->resizeSection(DataHierarchyModelItem::Column::Name, 180);
    //header()->resizeSection(DataHierarchyModelItem::Column::Description, 100);
    header()->resizeSection(DataHierarchyModelItem::Column::Progress, 50);
    header()->resizeSection(DataHierarchyModelItem::Column::Analyzing, 16);
    header()->resizeSection(DataHierarchyModelItem::Column::Locked, 16);

    header()->setSectionResizeMode(DataHierarchyModelItem::Column::Name, QHeaderView::Interactive);
    header()->setSectionResizeMode(DataHierarchyModelItem::Column::GUID, QHeaderView::Fixed);
    header()->setSectionResizeMode(DataHierarchyModelItem::Column::Description, QHeaderView::Stretch);
    header()->setSectionResizeMode(DataHierarchyModelItem::Column::Progress, QHeaderView::Fixed);
    header()->setSectionResizeMode(DataHierarchyModelItem::Column::Analyzing, QHeaderView::Fixed);
    header()->setSectionResizeMode(DataHierarchyModelItem::Column::Locked, QHeaderView::Fixed);

    header()->setStretchLastSection(false);

    // Notify others that the dataset selection changed when the current row in the model changed
    connect(&_selectionModel, &QItemSelectionModel::currentRowChanged, this, [this](const QModelIndex& current, const QModelIndex& previous) {

        // Only proceed with a valid selection
        if (!current.isValid())
            return;
        
        // Get the globally unique identifier of the selected dataset
        const auto selectedDatasetId = current.siblingAtColumn(static_cast<std::int32_t>(DataHierarchyModelItem::Column::GUID)).data(Qt::DisplayRole).toString();

        // Notify others that a dataset was selected
        emit selectedDatasetChanged(selectedDatasetId);
    });

    // Show/hide the overlay and header widget when the number of rows changes
    connect(&_model, &QAbstractItemModel::rowsInserted, this, &DataHierarchyWidget::numberOfRowsChanged);
    connect(&_model, &QAbstractItemModel::rowsRemoved, this, &DataHierarchyWidget::numberOfRowsChanged);

    // Insert new rows expanded
    connect(&_model, &QAbstractItemModel::rowsInserted, this, [&](const QModelIndex& parent, int first, int last) {
        expand(_model.index(first, 0, parent));
    });

    // Add data hierarchy item to the widget when added in the data manager
    connect(&Application::core()->getDataHierarchyManager(), &DataHierarchyManager::itemAdded, this, &DataHierarchyWidget::addDataHierarchyItem);

    /*
    connect(&Application::core()->getDataHierarchyManager(), &DataHierarchyManager::itemAboutToBeRemoved, this, [this, getModelIndexForDatasetName](const QString& datasetName) {
        _selectionModel.clear();
        _model.removeDataHierarchyModelItem(getModelIndexForDatasetName(datasetName));
    });

    connect(&Application::core()->getDataHierarchyManager(), &DataHierarchyManager::itemRelocated, this, [this](DataHierarchyItem* relocatedItem) {
        Q_ASSERT(relocatedItem != nullptr);

        //_model.removeDataHierarchyModelItem(getModelIndexForDatasetName(relocatedItem->getDatasetName()));
        //_model.addDataHierarchyModelItem(QModelIndex(), relocatedItem);
    });
    */

    // Invoked the custom context menu when requested by the tree view
    connect(this, &QTreeView::customContextMenuRequested, this, [this](const QPoint& position) {

        // Get the model index at the required position
        const auto modelIndexBelowCursor = indexAt(position);

        // Show dataset context menu when over a dataset, give options to load data otherwise
        if (modelIndexBelowCursor.isValid())
        {
            // Get pointer to data hierarchy item
            auto dataHierarchyModelItem = _model.getItem(modelIndexBelowCursor, Qt::DisplayRole);

            // Get the data hierarchy item context menu
            QSharedPointer<QMenu> contextMenu(dataHierarchyModelItem->getContextMenu());

            // And show it
            contextMenu->exec(viewport()->mapToGlobal(position));
        }
        else {

            // Get dataset import context menu
            auto contextMenu = _dataImportAction.getContextMenu();

            // Show it
            if (contextMenu)
                contextMenu->exec(viewport()->mapToGlobal(position));
        }
    });

    // Initial visibility of the overlay and header
    numberOfRowsChanged();
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

        // Update the model then the data hierarchy item task description changes
        connect(&dataHierarchyItem, &DataHierarchyItem::taskDescriptionChanged, this, [this, dataset](const QString& description) {
            _model.setData(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Description), description);
        });

        // Update the model then the data hierarchy item task progress changes
        connect(&dataHierarchyItem, &DataHierarchyItem::taskProgressChanged, this, [this, dataset](const float& progress) {
            _model.setData(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Progress), progress);
            _model.setData(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Analyzing), progress > 0.0f);
        });

        // Update the model then the data hierarchy item task selection status changes
        connect(&dataHierarchyItem, &DataHierarchyItem::selectionChanged, this, [this, dataset](const bool& selection) {
            _selectionModel.select(getModelIndexByDataset(dataset), QItemSelectionModel::SelectionFlag::ClearAndSelect | QItemSelectionModel::SelectionFlag::Rows);
        });

        // Update the model then the data hierarchy item locked status changes
        connect(&dataHierarchyItem, &DataHierarchyItem::lockedChanged, this, [this, dataset](const bool& selection) {
            emit _model.dataChanged(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Name), getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Locked));
        });
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to add %1 to the data hierarchy tree widget").arg(dataset->getGuiName()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to add %1 to the data hierarchy tree widget").arg(dataset->getGuiName()));
    }
    // Expand by default
    //if (!isExpanded(parentModelIndex))
    //    expand(parentModelIndex);
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
    const auto dataIsLoaded = _model.rowCount() == 0;

    // Show the no data overlay widget when no data is loaded
    _noDataOverlayWidget->setVisible(dataIsLoaded);

    // Show the header when data is loaded
    setHeaderHidden(dataIsLoaded);
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
        case QEvent::Resize:
        {
            if (dynamic_cast<QWidget*>(target) != parent())
                break;

            const auto parentWidgetSize = static_cast<QResizeEvent*>(event)->size();

            setFixedSize(parentWidgetSize);

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

}
}
