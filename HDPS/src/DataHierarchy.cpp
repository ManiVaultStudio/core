#include "DataHierarchy.h"

#include "DataHierarchyModel.h"
#include "DataManager.h"

#include <QInputDialog>
#include <QDebug>

namespace hdps
{
    namespace gui
    {
        DataHierarchy::DataHierarchy(DataManager& dataManager) :
            _dataManager(dataManager)
        {
            setMinimumWidth(200);

            // FIXME
            connect(&dataManager, &DataManager::dataChanged, this, &DataHierarchy::updateDataModel);
            setContextMenuPolicy(Qt::CustomContextMenu);
            connect(this, &QTreeView::customContextMenuRequested, this, &DataHierarchy::itemContextMenu);

            // Fill up data hierarchy model with data currently in the system
            _model = new DataHierarchyModel(dataManager);

            // Create a custom context menu
            _contextMenu = new QMenu();

            // Create the tree view that shows all data
            setDragEnabled(true);
            setDragDropMode(QAbstractItemView::DragOnly);
            setSelectionMode(QAbstractItemView::SingleSelection);
            setModel(_model);
        }

        void DataHierarchy::updateDataModel()
        {
            _model = new DataHierarchyModel(_dataManager);
            setModel(_model);
        }

        void DataHierarchy::itemContextMenu(const QPoint& pos)
        {
            QModelIndex index = indexAt(pos);

            if (index.isValid())
            {
                // Get the dataset name of the clicked item
                QString datasetName = _model->getItem(index, Qt::DisplayRole)->getDataAtColumn(0);

                // Clear previous actions from the context menu in order to give them fresh data
                _contextMenu->clear();

                // Rename option
                QAction* renameAction = new QAction("Rename");
                renameAction->setData(datasetName);
                connect(renameAction, &QAction::triggered, this, &DataHierarchy::dataRenamed);
                _contextMenu->addAction(renameAction);

                _contextMenu->exec(viewport()->mapToGlobal(pos));
            }
        }

        void DataHierarchy::dataRenamed()
        {
            // Extract name of item that triggered the context menu action
            QAction* act = qobject_cast<QAction*>(sender());
            QString datasetName = act->data().toString();

            // Pop up a dialog where the user can enter a new name
            bool ok;
            QString newDatasetName = QInputDialog::getText(this, tr("Rename Dataset"),
                tr("Dataset name:"), QLineEdit::Normal,
                nullptr, &ok);
            if (ok && !newDatasetName.isEmpty())
            {
                _dataManager.renameSet(datasetName, newDatasetName);
            }
        }
    }
}
