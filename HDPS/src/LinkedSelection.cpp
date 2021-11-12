#include "LinkedSelection.h"

#include "Set.h"

namespace hdps
{
    LinkedSelection::LinkedSelection(DataSet* sourceDataSet, DataSet* targetDataSet) :
        _sourceDataSet(sourceDataSet),
        _targetDataSet(targetDataSet)
    {
        Q_ASSERT(_sourceDataSet != nullptr);
        Q_ASSERT(_targetDataSet != nullptr);
    }

    const SelectionMap& LinkedSelection::getMapping()
    {
        return _mapping;
    }

    void LinkedSelection::setMapping(SelectionMap& mapping)
    {
        _mapping = mapping;
    }

}
