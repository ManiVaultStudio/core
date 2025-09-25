#pragma once

#include <Dataset.h>
#include <util/Serializable.h>

#include <QString>

#include <unordered_map>

namespace mv
{

/**
 * Class implementing a bidirectional map, providing a unique coupling between
 * a list of keys and a list of values.
 */
class CORE_EXPORT BiMap : public util::Serializable
{
public:
    void addKeyValuePairs(const std::vector<QString>& keys, const std::vector<uint32_t>& values);

    std::vector<QString> getKeysByValues(const std::vector<uint32_t>& values) const;
    std::vector<uint32_t> getValuesByKeys(const std::vector<QString>& keys, bool verbose = false) const;
    std::vector<int> getValuesByKeysWithMissingValue(const std::vector<QString>& keys, int missingValue) const;

public: // Serialization
    /**
     * Load bimap from variant
     * @param Variant representation of the bimap
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save bimap to variant
     * @return Variant representation of the bimap
     */
    QVariantMap toVariantMap() const override;

private:
    std::unordered_map<QString, uint32_t> _kvMap = {};
    std::unordered_map<uint32_t, QString> _vkMap = {};
};

class CORE_EXPORT KeyBasedSelectionGroup : public util::Serializable
{
public:
    BiMap& getBiMap(Dataset<DatasetImpl> dataset);

    void addDataset(Dataset<DatasetImpl> dataset, BiMap& bimap);

    void selectionChanged(Dataset<DatasetImpl> dataset, const std::vector<uint32_t>& indices) const;

public: // Serialization
    /**
     * Load selection group from variant
     * @param Variant representation of the selection group
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save selection group to variant
     * @return Variant representation of the selection group
     */
    QVariantMap toVariantMap() const override;

private:
    std::vector<Dataset<DatasetImpl>> _datasets = {};
    std::vector<BiMap> _biMaps = {};
};

}
