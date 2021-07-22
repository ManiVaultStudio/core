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

PluginHierarchyWidget::PluginHierarchyWidget(Core* core) :
    _core(core),
    _model(new PluginHierarchyModel(_core))
{
    setMinimumWidth(500);
    setModel(_model);
    
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QTreeView::customContextMenuRequested, this, &PluginHierarchyWidget::itemContextMenu);

    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setRootIsDecorated(false);
    setItemsExpandable(true);

    header()->resizeSection(static_cast<std::int32_t>(DataHierarchyItem::Column::Progress), 50);
    header()->resizeSection(static_cast<std::int32_t>(DataHierarchyItem::Column::Analyzing), 20);

    header()->setSectionResizeMode(static_cast<std::int32_t>(DataHierarchyItem::Column::Name), QHeaderView::Stretch);
    header()->setSectionResizeMode(static_cast<std::int32_t>(DataHierarchyItem::Column::Progress), QHeaderView::Fixed);
    header()->setSectionResizeMode(static_cast<std::int32_t>(DataHierarchyItem::Column::Analyzing), QHeaderView::Fixed);

    header()->setStretchLastSection(false);
}

void PluginHierarchyWidget::itemContextMenu(const QPoint& pos)
{
     QModelIndex index = indexAt(pos);

    if (index.isValid())
    {
        auto pluginHierarchyItem = _model->getItem(index, Qt::DisplayRole);
        
        QSharedPointer<QMenu> contextMenu(pluginHierarchyItem->getContextMenu());

        contextMenu->exec(viewport()->mapToGlobal(pos));
    }
}

void PluginHierarchyWidget::dataRenamed()
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
