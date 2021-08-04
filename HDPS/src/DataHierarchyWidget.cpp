#include "DataHierarchyWidget.h"
#include "DataHierarchyModel.h"
#include "DataHierarchyModelItem.h"
#include "Core.h"

#include <QDebug>
#include <QInputDialog>
#include <QHeaderView>

namespace hdps
{

namespace gui
{

DataHierarchyWidget::DataHierarchyWidget(QWidget* parent, Core* core) :
    QTreeView(parent),
    _core(core),
    _model(_core),
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

    header()->resizeSection(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Description), 200);
    header()->resizeSection(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Progress), 50);
    header()->resizeSection(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Analyzing), 20);

    header()->setSectionResizeMode(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Name), QHeaderView::Interactive);
    header()->setSectionResizeMode(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Description), QHeaderView::Stretch);
    header()->setSectionResizeMode(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Progress), QHeaderView::Fixed);
    header()->setSectionResizeMode(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Analyzing), QHeaderView::Fixed);

    header()->setStretchLastSection(false);

    connect(&_selectionModel, &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection& deselected) {
        const auto selectedDatasetName = selected.first().topLeft().data(Qt::DisplayRole).toString();
        emit selectedDatasetNameChanged(selectedDatasetName);
    });

    connect(&_model, &QAbstractItemModel::rowsInserted, [this](const QModelIndex& parent, int first, int last)
    {
        if (!isExpanded(parent))
            expand(parent);

        //_selectionModel.select(parent.child(first, 0), QItemSelectionModel::SelectionFlag::ClearAndSelect | QItemSelectionModel::SelectionFlag::Rows);
    });

    connect(this, &QTreeView::customContextMenuRequested, this, [this](const QPoint& position) {
        QModelIndex index = indexAt(position);

        if (index.isValid())
        {
            auto dataHierarchyItem = _model.getItem(index, Qt::DisplayRole);

            QSharedPointer<QMenu> contextMenu(dataHierarchyItem->getContextMenu());

            /*
            auto contextMenu = _dataset->getContextMenu();

            // Extract name of item that triggered the context menu action
            QAction* act = qobject_cast<QAction*>(sender());

            QString datasetName = act->data().toString();

            // Pop up a dialog where the user can enter a new name
            bool ok;

            QString newDatasetName = QInputDialog::getText(this, tr("Rename Dataset"), tr("Dataset name:"), QLineEdit::Normal, datasetName, &ok);

            if (ok && !newDatasetName.isEmpty())
                _core->getDataManager().renameSet(datasetName, newDatasetName);
            */

            contextMenu->exec(viewport()->mapToGlobal(position));
        }
    });
}

}
}
