#pragma once

#include "Dataset.h"

#include <QString>

#include <map>
#include <vector>

namespace hdps
{
class DatasetImpl;

using SelectionMap = std::map<unsigned int, std::vector<unsigned int>>;

class LinkedSelection
{
public:
    LinkedSelection(const Dataset<DatasetImpl>& sourceDataSet, const Dataset<DatasetImpl>& targetDataSet);

    Dataset<DatasetImpl> getTargetDataset() { return _targetDataSet; }

    const SelectionMap& getMapping();
    void setMapping(SelectionMap& map);

private: 
    Dataset<DatasetImpl>    _sourceDataSet;
    Dataset<DatasetImpl>    _targetDataSet;
    SelectionMap            _mapping;
};

}