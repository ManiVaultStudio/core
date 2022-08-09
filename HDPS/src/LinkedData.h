#pragma once

#include "Dataset.h"

#include <QString>

#include <map>
#include <vector>

namespace hdps
{
class DatasetImpl;

using SelectionMap = std::map<unsigned int, std::vector<unsigned int>>;

class LinkedData
{
public:
    LinkedData(const Dataset<DatasetImpl>& sourceDataSet, const Dataset<DatasetImpl>& targetDataSet);

    const Dataset<DatasetImpl> getTargetDataset() const { return _targetDataSet; }

    const SelectionMap& getMapping() const;
    void setMapping(SelectionMap& map);

private: 
    Dataset<DatasetImpl>    _sourceDataSet;
    Dataset<DatasetImpl>    _targetDataSet;

    SelectionMap _mapping;
};

class IndexLinkedData
{

};

}
