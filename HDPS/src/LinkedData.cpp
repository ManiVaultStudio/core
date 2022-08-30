#include "LinkedData.h"

#include "Set.h"

namespace hdps
{
    LinkedData::LinkedData(const Dataset<DatasetImpl>& sourceDataSet, const Dataset<DatasetImpl>& targetDataSet) :
        _sourceDataSet(sourceDataSet),
        _targetDataSet(targetDataSet)
    {
        Q_ASSERT(_sourceDataSet.isValid());
        Q_ASSERT(_targetDataSet.isValid());
    }

    //template<typename DatasetType>
    //void LinkedSelection::resolve(const std::vector<unsigned int>& indices)
    //{
    //    Dataset<DatasetType> targetDataset = linkedSelection.getTargetDataset();
    //    Dataset<DatasetType> targetSelection = targetDataset->getSelection();

    //    const hdps::SelectionMap& mapping = linkedSelection.getMapping();

    //    // Create separate vector of additional linked selected points
    //    std::vector<unsigned int> extraSelectionIndices;

    //    // Reserve at least as much space as required for a 1-1 mapping
    //    extraSelectionIndices.reserve(indices.size());

    //    for (const int selectionIndex : indices)
    //    {
    //        if (mapping.find(selectionIndex) != mapping.end())
    //        {
    //            const std::vector<unsigned int>& mappedSelection = mapping.at(selectionIndex);
    //            extraSelectionIndices.insert(extraSelectionIndices.end(), mappedSelection.begin(), mappedSelection.end());
    //        }
    //    }

    //    targetSelection->indices.insert(targetSelection->indices.end(), extraSelectionIndices.begin(), extraSelectionIndices.end());
    //}

    const SelectionMap& LinkedData::getMapping() const
    {
        return _mapping;
    }

    void LinkedData::setMapping(SelectionMap& mapping)
    {
        _mapping = mapping;
    }
}
