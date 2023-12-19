// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Set.h"

#include <AbstractDataManager.h>

#include <QObject> // To support signals
#include <QString>

#include <string>
#include <unordered_map>    // Data is stored in maps
#include <memory>           // Data is stored in unique pointers
#include <exception>        // Used for defining custom exceptions
#include <functional>       // Necessary for hash function

// Qt 5.14.1 has a std::hash<QString> specialization and defines the following macro.
// Qt 5.12.4 does not! See also https://bugreports.qt.io/browse/QTBUG-33428
#ifndef QT_SPECIALIZE_STD_HASH_TO_CALL_QHASH
// Hash specialization for QString so we can use it as a key in the data maps
namespace std {
    template<> struct hash<QString> {
        std::size_t operator()(const QString& s) const {
            return qHash(s);
        }
    };
}
#endif

namespace mv
{

namespace plugin {
    class RawData;
}

class DataManager final : public mv::AbstractDataManager
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param core Pointer to the core
     */
    DataManager();

    ~DataManager();

    /** Perform manager startup initialization */
    void initialize() override;

    /** Resets the contents of the data manager */
    void reset() override;

    /**
     * Add \p rawData to the data manager
     * @param rawData Pointer to the raw data to add
     */
    void addRawData(plugin::RawData* rawData) override;

    /**
     * Add \p dataset to the data manager
     * @param dataset Dataset to added
     * @param parentDataset Parent dataset (if any)
     * @param visible Whether \p dataset should be visible or not
     * @param notify Whether to notify the core that a dataset is added
     */
    void addDataset(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, bool visible = true, bool notify = true) override;

    /**
     * Add \p selection to the data manager
     * @param rawDataName Name of the raw data
     * @param selection Smart pointer to selection dataset
     */
    void addSelection(const QString& rawDataName, Dataset<DatasetImpl> selection) override;

    /**
     * Remove \p dataset from the manager
     * Other datasets derived from this dataset are converted to non-derived data
     * @param dataset Smart pointer to dataset to remove
     */
    void removeDataset(Dataset<DatasetImpl> dataset) override;

    /**
     * Remove \p dataset supervised (using a user interface)
     * Interface allows the user to select which descendants should be removed
     * This method calls AbstractDataManager::removeDataset()
     * @param dataset Smart pointer to dataset to remove
     */
    void removeDatasetSupervised(Dataset<DatasetImpl> dataset) override;

    /**
     * Get raw data by name
     * @param name Name of the raw data
     */
    plugin::RawData& getRawData(const QString& name) override;

    /**
     * Get dataset by dataset GUID
     * @param datasetGuid GUID of the dataset
     * @return Smart pointer to the dataset
     */
    Dataset<DatasetImpl> getSet(const QString& datasetGuid) override;

    /**
     * Get selection by data name
     * @param dataName Name of the data
     * @return Smart pointer to the selection dataset
     */
    Dataset<DatasetImpl> getSelection(const QString& dataName) override;

    /** Get all sets from the data manager */
    const QVector<Dataset<DatasetImpl>>& allSets() const override;

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

private:

    /**
     * Stores all raw data in the system. Raw data is stored by the name
     * retrieved from their Plugin::getName() function.
     */
    std::unordered_map<QString, std::unique_ptr<plugin::RawData>> _rawDataMap;

    /**
     * Stores all data sets in the system
     */
    QVector<Dataset<DatasetImpl>> _datasets;

    /**
    * Stores selection sets on all data plug-ins
    * NOTE: Can't be a QMap because it doesn't support move semantics of unique_ptr
    */
    std::unordered_map<QString, Dataset<DatasetImpl>> _selections;
};

} // namespace mv
