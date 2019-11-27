#include "IndexSet.h"

namespace hdps
{
    hdps::Set* IndexSet::copy() const
    {
        IndexSet* set = new IndexSet(_core, getDataName());
        set->setName(getName());
        set->indices = indices;
        return set;
    }

    void IndexSet::createSubset() const
    {
        const hdps::Set& selection = _core->requestSelection(getDataName());

        _core->createSubsetFromSelection(selection, getDataName(), "Subset");
    }
}
