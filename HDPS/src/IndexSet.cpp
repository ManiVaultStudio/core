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
}
