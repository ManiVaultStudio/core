#include "LinkedSelection.h"

namespace hdps
{
    LinkedSelection::LinkedSelection(QString source, QString target) :
        _sourceData(source),
        _targetData(target)
    {

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
