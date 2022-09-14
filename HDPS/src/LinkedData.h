#pragma once

#include "Dataset.h"
#include "Serializable.h"

#include <QString>

#include <map>
#include <vector>

namespace hdps
{
    class DatasetImpl;

    class SelectionMap : public Serializable
    {
    public:

        /** Type of selection mapping */
        enum class Type
        {
            Indexed,            /** Using mapped indices */
            ImagePyramid        /** Using image pyramids */
        };

        using Indices   = std::vector<std::uint32_t>;
        using Map       = std::map<std::uint32_t, Indices>;

    public:

        /**
         * Constructs a selection mapping of \p type
         * @param type Type of selection mapping
         */
        SelectionMap(Type type = Type::Indexed);

        /**
         * Constructs an image pyramid selection mapping
         * @param sourceImageSize Resolution of the source image
         * @param targetImageSize Resolution of the target image
         */
        SelectionMap(const QSize& sourceImageSize, const QSize& targetImageSize);

        /**
         * Populate mapping \p indices for \p pointIndex
         * @param pointIndex Point index for which to populate
         * @param indices Mapping indices
         */
        void populateMappingIndices(std::uint32_t pointIndex, Indices& indices) const;

        /**
         * Get map for indexed pixels
         * @return Index map
         */
        Map& getMap();

        /**
         * Establishes whether a mapping exists for \p pointIndex
         * @param pointIndex Point index to check for
         * @return Boolean indicating whether a mapping exists for \p pointIndex
         */
        bool hasMappingForPointIndex(std::uint32_t pointIndex) const;

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
        Type    _type;              /** The type of selection map */
        Map     _map;               /** Map contents (when mapping type is indexed) */
        QSize   _sourceImageSize;   /** Source image size (when mapping type is image pyramid) */
        QSize   _targetImageSize;   /** Target image size (when mapping type is image pyramid) */
    };

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
