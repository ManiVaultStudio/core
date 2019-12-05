#include "DataHierarchy.h"

#include "DataHierarchyModel.h"
#include "DataManager.h"

namespace hdps
{
    namespace gui
    {
        struct AggregateStruct { int x; float f; } ;

        AggregateStruct GetAggregateStruct()
        {
            return { 42, 3.14f };
        }

        DataHierarchy::DataHierarchy(DataManager& dataManager) :
            _dataManager(dataManager)
        {
            auto[xx, ff] = GetAggregateStruct(); // TODO C++17 structured binding.

            if (xx > 42 || ff < 3.1)
            {
                throw std::exception{};
            }

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
