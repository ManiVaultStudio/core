#include "DataHierarchyWidget.h"
#include "DataHierarchyModel.h"
#include "DataHierarchyItem.h"
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
    _model(new PluginHierarchyModel(_core)),
    _selectionModel(new QItemSelectionModel(_model))
{
    setMinimumWidth(500);
    setModel(_model);
    
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QTreeView::customContextMenuRequested, this, &DataHierarchyWidget::itemContextMenu);

    setSelectionModel(_selectionModel);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setRootIsDecorated(false);
    setItemsExpandable(true);
    setHeaderHidden(true);

    header()->resizeSection(static_cast<std::int32_t>(DataHierarchyItem::Column::Progress), 50);
    header()->resizeSection(static_cast<std::int32_t>(DataHierarchyItem::Column::Analyzing), 20);

    header()->setSectionResizeMode(static_cast<std::int32_t>(DataHierarchyItem::Column::Name), QHeaderView::Stretch);
    header()->setSectionResizeMode(static_cast<std::int32_t>(DataHierarchyItem::Column::Progress), QHeaderView::Fixed);
    header()->setSectionResizeMode(static_cast<std::int32_t>(DataHierarchyItem::Column::Analyzing), QHeaderView::Fixed);

    header()->setStretchLastSection(false);

    connect(_selectionModel, &QItemSelectionModel::currentRowChanged, this, [this](const QModelIndex& current, const QModelIndex& previous) {
        const auto selectedDatasetName = current.siblingAtColumn(static_cast<std::int32_t>(DataHierarchyItem::Column::Name)).data(Qt::DisplayRole).toString();
        emit selectedDatasetNameChanged(selectedDatasetName);
    });

    connect(_model, &QAbstractItemModel::rowsInserted, [this](const QModelIndex& parent, int first, int last)
    {
        if (!isExpanded(parent))
            expand(parent);

        _selectionModel->select(parent.child(first, 0), QItemSelectionModel::SelectionFlag::ClearAndSelect | QItemSelectionModel::SelectionFlag::Rows);

        emit selectedDatasetNameChanged(parent.child(first, 0).data(Qt::DisplayRole).toString());
    });
}

void DataHierarchyWidget::itemContextMenu(const QPoint& pos)
{
     QModelIndex index = indexAt(pos);

    if (index.isValid())
    {
        auto pluginHierarchyItem = _model->getItem(index, Qt::DisplayRole);
        
        QSharedPointer<QMenu> contextMenu(pluginHierarchyItem->getContextMenu());

        contextMenu->exec(viewport()->mapToGlobal(pos));
    }
}

void DataHierarchyWidget::dataRenamed()
{
    /*
    // Extract name of item that triggered the context menu action
    QAction* act = qobject_cast<QAction*>(sender());
    QString datasetName = act->data().toString();

    // Pop up a dialog where the user can enter a new name
    bool ok;
    QString newDatasetName = QInputDialog::getText(this, tr("Rename Dataset"),
        tr("Dataset name:"), QLineEdit::Normal,
        datasetName, &ok);
    if (ok && !newDatasetName.isEmpty())
    {
        _dataManager.renameSet(datasetName, newDatasetName);
    }
    */
}

}
}
