#include "LinkedSelection.h"
#include "Set.h"

namespace hdps
{
    LinkedSelection::LinkedSelection(const Dataset<DatasetImpl>& sourceDataSet, const Dataset<DatasetImpl>& targetDataSet) :
        _sourceDataSet(sourceDataSet),
        _targetDataSet(targetDataSet)
    {
        Q_ASSERT(_sourceDataSet.isValid());
        Q_ASSERT(_targetDataSet.isValid());
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
