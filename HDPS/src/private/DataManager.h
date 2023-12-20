// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Set.h"

#include <AbstractDataManager.h>

#include <QObject> // To support signals
#include <QString>

#include <string>
#include <map>
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

    /** Empty the data manager upon destruction */
    ~DataManager();

    /** Perform manager startup initialization */
    void initialize() override;

    /** Resets the contents of the data manager */
    void reset() override;

public: // Raw data

    /**
     * Add \p rawData
     * @param rawData Pointer to the raw data to add
     */
    void addRawData(plugin::RawData* rawData) override;

    /**
     * Get raw data by \p rawDataName
     * @param rawDataName Name of the raw data
     * @return Pointer to raw data if found, nullptr otherwise
     */
    plugin::RawData* getRawData(const QString& rawDataName) override;

    /**
     * Get raw data names
     * @retun Raw data names list
     */
    QStringList& getRawDataNames() const override;

public: // Datasets

    /**
     * Add \p dataset to the data manager
     * @param dataset Dataset to added
     * @param parentDataset Parent dataset (if any)
     * @param visible Whether \p dataset should be visible or not
     * @param notify Whether to notify the core that a dataset is added
     */
    void addDataset(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, bool visible = true, bool notify = true) override;

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
     * Get dataset by \p datasetId
     * @param datasetId GUID of the dataset
     * @return Smart pointer to the dataset
     */
    Dataset<DatasetImpl> getDataset(const QString& datasetId) override;

    /**
     * Get all datasets for \p dataTypes
     * @param dataTypes Data types filter
     * @return Smart pointers to datasets
     */
    Datasets getAllDatasets(const std::vector<DataType>& dataTypes = std::vector<DataType>()) const override;

public: // Selections

    /**
     * Add \p selection on \p rawDataName to the data manager
     * @param rawDataName Name of the raw data
     * @param selection Smart pointer to selection dataset
     */
    void addSelection(const QString& rawDataName, Dataset<DatasetImpl> selection) override;

protected:

    /**
     * Remove \p selection from the data manager
     * @param rawDataName Name of the selection raw data
     */
    void removeSelection(const QString& rawDataName) override;

public:

    /**
     * Get selection by \p rawDataName
     * @param rawDataName Name of the raw data
     * @return Smart pointer to the selection dataset
     */
    Dataset<DatasetImpl> getSelection(const QString& rawDataName) override;

    /**
     * Get all selection datasets
     * @return Smart pointers to selection datasets
     */
    Datasets getAllSelections() override;

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
    std::unordered_map<QString, std::unique_ptr<plugin::RawData>>   _rawDataMap;        /** Maps raw data name to raw data plugin pointer */
    std::map<QString, std::unique_ptr<DatasetImpl>>                 _datasetsMap;       /** Maps raw data name to dataset pointer */
    std::map<QString, std::unique_ptr<DatasetImpl>>                 _selectionsMap;     /** Maps dataset ID to dataset pointer */
};

}
