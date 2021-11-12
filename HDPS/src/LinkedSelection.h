#ifndef HDPS_LINKED_SELECTION_H
#define HDPS_LINKED_SELECTION_H

#include <QString>
#include <map>
#include <vector>

namespace hdps
{
    class DataSet;

    using SelectionMap = std::map<unsigned int, std::vector<unsigned int>>;

    class LinkedSelection
    {
    public:
        LinkedSelection(DataSet* sourceDataSet, DataSet* targetDataSet);

        DataSet* getTargetDataset() { return _targetDataSet; }
        const SelectionMap& getMapping();
        void setMapping(SelectionMap& map);

    private:
        DataSet*    _sourceDataSet;
        DataSet*    _targetDataSet;

        SelectionMap _mapping;
    };
}

#endif // HDPS_LINKED_SELECTION_H
