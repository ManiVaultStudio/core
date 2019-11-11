#pragma once

#include "Set.h"

namespace hdps
{
    class IndexSet : public Set
    {
    public:
        IndexSet(hdps::CoreInterface* core, QString dataName) : Set(core, dataName) { }
        ~IndexSet() override { }

        template <class Data>
        Data& getData() const
        {
            return dynamic_cast<Data&>(_core->requestData(getDataName()));
        }

        Set* copy() const override;

        std::vector<unsigned int> indices;
    };
} // namespace hdps
