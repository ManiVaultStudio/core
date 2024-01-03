// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"

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

class DatasetImpl;

namespace plugin {
    class RawData;
}

/**
 * Abstract data manager
 *
 * Base abstract data manager class for managing datasets.
 *
 * @author Thomas Kroes and Julian Thijssen
 */
class AbstractDataManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Construct data manager with \p parent object
     * @param parent Pointer to parent object
     */
    AbstractDataManager(QObject* parent = nullptr) :
        AbstractManager(parent, "Data")
    {
    }

public: // Raw data

    /**
     * Add \p rawData to the data manager (ownership remains with the plugin manager)
     * @param rawData Pointer to the raw data to add
     */
    virtual void addRawData(plugin::RawData* rawData) = 0;

protected:

    /**
     * Remove raw data by \p rawDataName from the data manager
     * @param rawDataName Name of the raw data to remove
     */
    virtual void removeRawData(const QString& rawDataName) = 0;

public:

    /**
     * Get raw data by \p rawDataName
     * @param rawDataName Name of the raw data
     * @return Pointer to raw data if found, nullptr otherwise
     */
    virtual plugin::RawData* getRawData(const QString& rawDataName) = 0;

    /**
     * Get raw data by \p rawDataName
     * @param rawDataName Name of the raw data
     * @return Pointer of \p RawDataType to raw data if found, nullptr otherwise
     */
    template<typename RawDataType>
    RawDataType* getRawData(const QString& rawDataName) {
        return dynamic_cast<RawDataType*>(getRawData(rawDataName));
    }

    /**
     * Get raw data names
     * @retun Raw data names list
     */
    virtual QStringList getRawDataNames() const = 0;

public: // Datasets

    /**
     * Add \p dataset to the data manager
     * @param dataset Dataset to added
     * @param parentDataset Parent dataset (if any)
     * @param visible Whether \p dataset should be visible or not
     * @param notify Whether to notify the core that a dataset is added
     */
    virtual void addDataset(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, bool visible = true, bool notify = true) = 0;

    /**
     * Remove \p dataset from the data manager
     * If necessary, other datasets derived from the dataset are
     * converted to non-derived data. Datasets which may not be
     * un-derived will be automatically removed if their parent dataset
     * is removed.
     * @param dataset Smart pointer to dataset to remove
     */
    virtual void removeDataset(Dataset<DatasetImpl> dataset) = 0;

    /**
     * Remove \p datasets from the data manager
     *
     * If necessary, other datasets derived from one of the datasets
     * in the list are converted to non-derived data. Datasets which may not
     * be un-derived will be automatically removed if their parent dataset
     * is removed.
     * @param datasets Smart pointer to datasets to remove
     */
    virtual void removeDatasets(Datasets datasets) = 0;

    /**
     * Get dataset by \p datasetId
     * @param datasetId GUID of the dataset
     * @return Smart pointer to the dataset
     */
    virtual Dataset<DatasetImpl> getDataset(const QString& datasetId) = 0;

    /**
     * Get a dataset of a specific type by \p datasetId
     * @param datasetId GUID of the dataset
     * @return Smart pointer to the dataset of the specified type
     */
    template<typename DatasetType>
    Dataset<DatasetType> getDataset(const QString& datasetId) {
        return Dataset<DatasetType>(getDataset(datasetId));
    }

    /**
     * Get all datasets for \p dataTypes
     * @param dataTypes Data types filter
     * @return Smart pointers to datasets
     */
    virtual Datasets getAllDatasets(const std::vector<DataType>& dataTypes = std::vector<DataType>()) const = 0;

public: // Selections

    /**
     * Add \p selection on \p rawDataName to the data manager
     * @param rawDataName Name of the raw data
     * @param selection Smart pointer to selection dataset
     */
    virtual void addSelection(const QString& rawDataName, Dataset<DatasetImpl> selection) = 0;

protected:

    /**
     * Remove \p selection from the data manager
     * @param rawDataName Name of the selection raw data
     */
    virtual void removeSelection(const QString& rawDataName) = 0;

public:

    /**
     * Get selection dataset by \p rawDataName
     * @param rawDataName Name of the raw data
     * @return Smart pointer to the selection dataset
     */
    virtual Dataset<DatasetImpl> getSelection(const QString& rawDataName) = 0;

    /**
     * Get a selection of \p DatasetType by \p rawDataName
     * @param rawdataName Name of the raw data
     * @return Smart pointer to the selection dataset
     */
    template<typename DatasetType>
    Dataset<DatasetType> getSelection(const QString& rawDataName) {
        return Dataset<DatasetType>(dynamic_cast<DatasetType*>(getSelection(rawDataName).get()));
    }

    /**
     * Get all selection datasets
     * @return Smart pointers to selection datasets
     */
    virtual Datasets getAllSelections() = 0;

signals:

    /**
     * Signals that \p rawData is added to the data manager
     * @param rawData Pointer to raw data that was added
     */
    void rawDataAdded(plugin::RawData* rawData);

    /**
     * Signals that \p rawData is about to be removed from the data manager
     * @param rawData Pointer to raw data that is about to be removed
     */
    void rawDataAboutToBeRemoved(plugin::RawData* rawData);

    /**
     * Signals that raw data with \p rawDataId is removed from the data manager
     * @param rawData Pointer to raw data that was added
     */
    void rawDataRemoved(const QString& rawDataId);

    /**
     * Signals that \p dataset is added to the data manager
     * @param dataset Dataset that was added
     * @param parentDataset Parent dataset (if any)
     * @param visible Whether \p dataset should be visible or not
     */
    void datasetAdded(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, bool visible = true);

    /**
     * Signals that \p dataset is about to be removed from the data manager
     * @param dataset Dataset which is about to be removed
     */
    void datasetAboutToBeRemoved(Dataset<DatasetImpl> dataset);

    /**
     * Signals that dataset with \p datasetId is removed from the data manager
     * @param datasetId GUID of the removed dataset
     */
    void datasetRemoved(const QString& datasetId);

    /**
     * Signals that \p selection dataset is added to the data manager
     * @param selection Selection dataset that was added
     */
    void selectionAdded(Dataset<DatasetImpl> selection);

    /**
     * Signals that \p selection dataset is about to be removed from the data manager
     * @param selection Selection dataset that is about to be removed
     */
    void selectionAboutToBeRemoved(Dataset<DatasetImpl> selection);

    /**
     * Signals that selection dataset with \p selectionId is removed from the data manager
     * @param selectionId GUID of the removed selection dataset
     */
    void selectionRemoved(const QString& selectionId);
};

}