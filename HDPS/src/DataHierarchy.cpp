#include "DataHierarchy.h"

#include "DataHierarchyModel.h"
#include "DataManager.h"

namespace hdps
{
    namespace gui
    {
        DataHierarchy::DataHierarchy(DataManager& dataManager) :
            _dataManager(dataManager)
        {
            setFixedWidth(200);

            // FIXME
            connect(&dataManager, &DataManager::dataChanged, this, &DataHierarchy::updateDataModel);

            // Fill up data hierarchy model with data currently in the system
            _model = new DataHierarchyModel(dataManager);

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
    }
}
