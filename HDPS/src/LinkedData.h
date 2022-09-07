#pragma once

#include "Dataset.h"
#include "Serializable.h"

#include <QString>

#include <map>
#include <vector>

namespace hdps
{
    class DatasetImpl;

    using SelectionMap = std::map<unsigned int, std::vector<unsigned int>>;

    class LinkedData : public Serializable
    {
    public:
        LinkedData();
        LinkedData(const Dataset<DatasetImpl>& sourceDataSet, const Dataset<DatasetImpl>& targetDataSet);

        const Dataset<DatasetImpl> getSourceDataSet() const { return _sourceDataSet; }
        const Dataset<DatasetImpl> getTargetDataset() const { return _targetDataSet; }

        const SelectionMap& getMapping() const;
        void setMapping(SelectionMap& map);

        /**
         * Load from variant map
         * @param variantMap Variant map
         */
        void fromVariantMap(const QVariantMap& variantMap) override;

        /**
         * Save to variant map
         * @return Variant map
         */
        QVariantMap toVariantMap() const override;

    private:
        Dataset<DatasetImpl>    _sourceDataSet;
        Dataset<DatasetImpl>    _targetDataSet;
        SelectionMap            _mapping;
    };

    class IndexLinkedData
    {

    };

}
