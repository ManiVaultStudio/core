#include "DataHierarchyWidget.h"
#include "DataHierarchyModel.h"
#include "DataHierarchyModelItem.h"
#include "Core.h"

#include "util/Dataset.h"

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
    header()->resizeSection(DataHierarchyModelItem::Column::Analyzing, 20);

    header()->setSectionResizeMode(DataHierarchyModelItem::Column::Name, QHeaderView::Interactive);
    header()->setSectionResizeMode(DataHierarchyModelItem::Column::GUID, QHeaderView::Fixed);
    header()->setSectionResizeMode(DataHierarchyModelItem::Column::Description, QHeaderView::Stretch);
    header()->setSectionResizeMode(DataHierarchyModelItem::Column::Progress, QHeaderView::Fixed);
    header()->setSectionResizeMode(DataHierarchyModelItem::Column::Analyzing, QHeaderView::Fixed);

    header()->setStretchLastSection(false);

    connect(&_selectionModel, &QItemSelectionModel::currentRowChanged, this, [this](const QModelIndex& current, const QModelIndex& previous) {
        if (!current.isValid())
            return;
        
        // Get the globally unique identifier of the selected dataset
        const auto selectedDatasetId = current.siblingAtColumn(static_cast<std::int32_t>(DataHierarchyModelItem::Column::GUID)).data(Qt::DisplayRole).toString();

        // Notify others that a dataset was selected
        emit selectedDatasetChanged(selectedDatasetId);
    });

    const auto numberOfRowsChanged = [this]() {
        const auto dataIsLoaded = _model.rowCount() == 0;

        _noDataOverlayWidget->setVisible(dataIsLoaded);
        setHeaderHidden(dataIsLoaded);
    };

    connect(&_model, &QAbstractItemModel::rowsInserted, this, numberOfRowsChanged);
    connect(&_model, &QAbstractItemModel::rowsRemoved, this, numberOfRowsChanged);

    numberOfRowsChanged();

    const auto getModelIndexForDataset = [this](const Dataset<DatasetImpl>& dataset) -> QModelIndex {
        const auto modelIndex = _model.match(_model.index(0, 1), Qt::DisplayRole, dataset->getGuid(), 1, Qt::MatchFlag::MatchRecursive).first();

        if (!modelIndex.isValid())
            throw new std::runtime_error(QString("Dataset '%1' not found in model").arg(dataset->getGuiName()).toLatin1());

        return modelIndex;
    };

    connect(&Application::core()->getDataHierarchyManager(), &DataHierarchyManager::itemAdded, this, [this, getModelIndexForDataset](DataHierarchyItem* dataHierarchyItem) {
        if (dataHierarchyItem == nullptr || dataHierarchyItem->isHidden())
            return;

        auto dataset = dataHierarchyItem->getDataset();

        QModelIndex parentModelIndex;

        if (!dataHierarchyItem->hasParent())
            parentModelIndex = QModelIndex();
        else
            parentModelIndex = getModelIndexForDataset(dataHierarchyItem->getParent().getDataset());

        _model.addDataHierarchyModelItem(parentModelIndex, dataHierarchyItem);

        connect(dataHierarchyItem, &DataHierarchyItem::taskDescriptionChanged, this, [this, getModelIndexForDataset, dataHierarchyItem](const QString& description) {
            try
            {
                const auto modelIndex = getModelIndexForDataset(dataHierarchyItem->getDataset());

                _model.setData(modelIndex.siblingAtColumn(DataHierarchyModelItem::Column::Description), description);
            }
            catch (std::exception& e)
            {
                QMessageBox::warning(nullptr, "HDPS", QString("Unable to change data hierarchy item description: %1").arg(e.what()));
            }
        });

        connect(dataHierarchyItem, &DataHierarchyItem::taskProgressChanged, this, [this, getModelIndexForDataset, dataHierarchyItem](const float& progress) {
            try
            {
                const auto modelIndex = getModelIndexForDataset(dataHierarchyItem->getDataset());
                
                _model.setData(modelIndex.siblingAtColumn(DataHierarchyModelItem::Column::Progress), progress);
                _model.setData(modelIndex.siblingAtColumn(DataHierarchyModelItem::Column::Analyzing), progress > 0.0f);
            }
            catch (std::exception& e)
            {
                QMessageBox::warning(nullptr, "HDPS", QString("Unable to change data hierarchy item progress: %1").arg(e.what()));
            }
        });

        connect(dataHierarchyItem, &DataHierarchyItem::selectionChanged, this, [this, getModelIndexForDataset, dataHierarchyItem](const bool& selection) {
            try
            {
                const auto modelIndex = getModelIndexForDataset(dataHierarchyItem->getDataset());

                _selectionModel.select(modelIndex, QItemSelectionModel::SelectionFlag::ClearAndSelect | QItemSelectionModel::SelectionFlag::Rows);
            }
            catch (std::exception& e)
            {
                QMessageBox::warning(nullptr, "HDPS", QString("Unable to select data hierarchy item: %1").arg(e.what()));
            }
        });

        if (!isExpanded(parentModelIndex))
            expand(parentModelIndex);

        //_selectionModel.select(paparentModelIndexrent.child(first, 0), QItemSelectionModel::SelectionFlag::ClearAndSelect | QItemSelectionModel::SelectionFlag::Rows);
    });

    /*
    connect(&Application::core()->getDataHierarchyManager(), &DataHierarchyManager::itemAboutToBeRemoved, this, [this, getModelIndexForDatasetName](const QString& datasetName) {
        _selectionModel.clear();
        _model.removeDataHierarchyModelItem(getModelIndexForDatasetName(datasetName));
    });
    */

    connect(&Application::core()->getDataHierarchyManager(), &DataHierarchyManager::itemRelocated, this, [this, getModelIndexForDataset](DataHierarchyItem* relocatedItem) {
        Q_ASSERT(relocatedItem != nullptr);

        //_model.removeDataHierarchyModelItem(getModelIndexForDatasetName(relocatedItem->getDatasetName()));
        //_model.addDataHierarchyModelItem(QModelIndex(), relocatedItem);
    });

    connect(this, &QTreeView::customContextMenuRequested, this, [this](const QPoint& position) {
        QModelIndex index = indexAt(position);

        if (index.isValid())
        {
            auto dataHierarchyModelItem = _model.getItem(index, Qt::DisplayRole);

            QSharedPointer<QMenu> contextMenu(dataHierarchyModelItem->getContextMenu());

            contextMenu->exec(viewport()->mapToGlobal(position));
        }
        else {
            auto contextMenu = _dataImportAction.getContextMenu();

            if (contextMenu)
                contextMenu->exec(viewport()->mapToGlobal(position));
        }
    });
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
