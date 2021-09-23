#ifndef HDPS_LINKED_SELECTION_H
#define HDPS_LINKED_SELECTION_H

#include <QString>
#include <map>
#include <vector>

namespace hdps
{
    using SelectionMap = std::map<unsigned int, std::vector<unsigned int>>;

    class LinkedSelection
    {
    public:
        LinkedSelection(QString source, QString target);

        QString getTargetDataset() { return _targetData; }
        const SelectionMap& getMapping();
        void setMapping(SelectionMap& map);

    private:
        QString _sourceData;
        QString _targetData;

        SelectionMap _mapping;
    };
}

#endif // HDPS_LINKED_SELECTION_H
