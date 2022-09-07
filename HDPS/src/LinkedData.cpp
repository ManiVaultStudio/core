#include "LinkedData.h"
#include "util/Serialization.h"
#include "Set.h"

using namespace hdps::util;

namespace hdps
{
    LinkedData::LinkedData() :
        Serializable("LinkedData"),
        _sourceDataSet(),
        _targetDataSet()
    {
    }

    LinkedData::LinkedData(const Dataset<DatasetImpl>& sourceDataSet, const Dataset<DatasetImpl>& targetDataSet) :
        Serializable("LinkedData"),
        _sourceDataSet(sourceDataSet),
        _targetDataSet(targetDataSet)
    {
        Q_ASSERT(_sourceDataSet.isValid());
        Q_ASSERT(_targetDataSet.isValid());
    }

    const SelectionMap& LinkedData::getMapping() const
    {
        return _mapping;
    }

    void LinkedData::setMapping(SelectionMap& mapping)
    {
        _mapping = mapping;
    }

    void LinkedData::fromVariantMap(const QVariantMap& variantMap)
    {
        variantMapMustContain(variantMap, "SourceDataSet");
        variantMapMustContain(variantMap, "TargetDataSet");
        variantMapMustContain(variantMap, "Indices");
        variantMapMustContain(variantMap, "IndicesSize");
        variantMapMustContain(variantMap, "Ranges");
        variantMapMustContain(variantMap, "RangesSize");

        _sourceDataSet.setDatasetGuid(variantMap["SourceDataSet"].toString());
        _targetDataSet.setDatasetGuid(variantMap["TargetDataSet"].toString());

        std::vector<std::uint32_t> indices;
        std::vector<std::uint32_t> ranges;

        indices.resize(variantMap["IndicesSize"].toInt());
        ranges.resize(variantMap["RangesSize"].toInt());

        populateDataBufferFromVariantMap(variantMap["Indices"].toMap(), (char*)indices.data());
        populateDataBufferFromVariantMap(variantMap["Ranges"].toMap(), (char*)ranges.data());

        for (int i = 0; i < ranges.size(); i += 3)
            _mapping[ranges[i]] = std::vector<std::uint32_t>(indices.begin() + ranges[i + 1], indices.begin() + ranges[i + 2]);
    }

    QVariantMap LinkedData::toVariantMap() const
    {
        std::vector<std::uint32_t> indices;
        std::vector<std::uint32_t> ranges;

        ranges.reserve(_mapping.size() * 3);
        
        for (const auto& item : _mapping) {
            ranges.push_back(item.first);
            ranges.push_back(indices.size());
            indices.insert(indices.end(), item.second.begin(), item.second.end());
            ranges.push_back(indices.size() - 1);
        }

        return {
            { "SourceDataSet", QVariant::fromValue(_sourceDataSet.getDatasetGuid()) },
            { "TargetDataSet", QVariant::fromValue(_targetDataSet.getDatasetGuid()) },
            { "NumberOfIndices", QVariant::fromValue(indices.size()) },
            { "Indices", rawDataToVariantMap((char*)indices.data(), indices.size() * sizeof(std::uint32_t), true) },
            { "IndicesSize", QVariant::fromValue(indices.size()) },
            { "Ranges", rawDataToVariantMap((char*)ranges.data(), ranges.size() * sizeof(std::uint32_t), true) },
            { "RangesSize", QVariant::fromValue(ranges.size()) }
        };
    }

}
