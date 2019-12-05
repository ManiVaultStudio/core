#pragma once

#include "Set.h"

namespace hdps
{
    class IndexSet : public DataSet
    {
    public:
        IndexSet(hdps::CoreInterface* core, QString dataName) : DataSet(core, dataName) { }
        ~IndexSet() override { }

        DataSet* copy() const override;

        void createSubset() const override;

        std::vector<unsigned int> indices;
    };
} // namespace hdps
