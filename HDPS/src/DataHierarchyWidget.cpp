#include "DataHierarchyWidget.h"
#include "DataHierarchyModel.h"
#include "DataHierarchyModelItem.h"
#include "Core.h"

#include "util/DatasetRef.h"

#include <QDebug>
#include <QInputDialog>
#include <QHeaderView>
#include <QMessageBox>

#include <stdexcept>

using namespace hdps::util;

namespace hdps
{

namespace gui
{

DataHierarchyWidget::DataHierarchyWidget(QWidget* parent) :
    QTreeView(parent),
    _model(this),
    _selectionModel(&_model)
{
    setMinimumWidth(500);
    setModel(&_model);
    
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionModel(&_selectionModel);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setRootIsDecorated(true);
    setItemsExpandable(true);
    setColumnHidden(2, true);

    header()->resizeSection(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Name), 250);
    //header()->resizeSection(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Description), 100);
    header()->resizeSection(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Progress), 50);
    header()->resizeSection(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Analyzing), 20);

    header()->setSectionResizeMode(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Name), QHeaderView::Interactive);
    header()->setSectionResizeMode(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Description), QHeaderView::Stretch);
    header()->setSectionResizeMode(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Progress), QHeaderView::Fixed);
    header()->setSectionResizeMode(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Analyzing), QHeaderView::Fixed);

    header()->setStretchLastSection(false);

    connect(&_selectionModel, &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection& deselected) {
        if (selected.isEmpty())
            return;

        emit selectedDatasetNameChanged(selected.first().topLeft().data(Qt::DisplayRole).toString());
    });

    const auto getModelIndexForDatasetName = [this](const QString& datasetName) -> QModelIndex {
        const auto modelIndex = _model.match(_model.index(0, 0), Qt::DisplayRole, datasetName, 1, Qt::MatchFlag::MatchRecursive).first();

        if (!modelIndex.isValid())
            throw new std::runtime_error(QString("Dataset '%1' not found in model").arg(datasetName).toLatin1());

        return modelIndex;
    };

    connect(&Application::core()->getDataHierarchyManager(), &DataHierarchyManager::itemAdded, this, [this, getModelIndexForDatasetName](DataHierarchyItem* dataHierarchyItem) {
        if (dataHierarchyItem == nullptr || dataHierarchyItem->isHidden())
            return;

        DatasetRef<DataSet> dataset(dataHierarchyItem->getDatasetName());

        QModelIndex parentModelIndex;

        if (!dataHierarchyItem->hasParent())
            parentModelIndex = QModelIndex();
        else
            parentModelIndex = getModelIndexForDatasetName(dataHierarchyItem->getParent()->getDatasetName());

        _model.addDataHierarchyModelItem(parentModelIndex, dataHierarchyItem);

        connect(dataHierarchyItem, &DataHierarchyItem::taskDescriptionChanged, this, [this, getModelIndexForDatasetName, dataHierarchyItem](const QString& description) {
            try
            {
                const auto modelIndex = getModelIndexForDatasetName(dataHierarchyItem->getDatasetName());

                _model.setData(modelIndex.siblingAtColumn(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Description)), description);
            }
            catch (std::exception& e)
            {
                QMessageBox::warning(nullptr, "HDPS", QString("Unable to change data hierarchy item description: %1").arg(e.what()));
            }
        });

        connect(dataHierarchyItem, &DataHierarchyItem::taskProgressChanged, this, [this, getModelIndexForDatasetName, dataHierarchyItem](const float& progress) {
            try
            {
                const auto modelIndex = getModelIndexForDatasetName(dataHierarchyItem->getDatasetName());
                
                _model.setData(modelIndex.siblingAtColumn(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Progress)), progress);
                _model.setData(modelIndex.siblingAtColumn(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Analyzing)), progress > 0.0f);
            }
            catch (std::exception& e)
            {
                QMessageBox::warning(nullptr, "HDPS", QString("Unable to change data hierarchy item progress: %1").arg(e.what()));
            }
        });

        connect(dataHierarchyItem, &DataHierarchyItem::selectionChanged, this, [this, getModelIndexForDatasetName, dataHierarchyItem](const bool& selection) {
            try
            {
                const auto modelIndex = getModelIndexForDatasetName(dataHierarchyItem->getDatasetName());

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

    connect(&Application::core()->getDataHierarchyManager(), &DataHierarchyManager::itemAboutToBeRemoved, this, [this, getModelIndexForDatasetName](const QString& datasetName) {
        _selectionModel.clear();
        _model.removeDataHierarchyModelItem(getModelIndexForDatasetName(datasetName));
    });

    connect(&Application::core()->getDataHierarchyManager(), &DataHierarchyManager::itemRelocated, this, [this, getModelIndexForDatasetName](DataHierarchyItem* relocatedItem) {
        Q_ASSERT(relocatedItem != nullptr);

        _model.removeDataHierarchyModelItem(getModelIndexForDatasetName(relocatedItem->getDatasetName()));
        _model.addDataHierarchyModelItem(QModelIndex(), relocatedItem);
    });

    connect(this, &QTreeView::customContextMenuRequested, this, [this](const QPoint& position) {
        QModelIndex index = indexAt(position);

        if (index.isValid())
        {
            auto dataHierarchyModelItem = _model.getItem(index, Qt::DisplayRole);

            QSharedPointer<QMenu> contextMenu(dataHierarchyModelItem->getContextMenu());

            contextMenu->exec(viewport()->mapToGlobal(position));
        }
    });
}

}
}
