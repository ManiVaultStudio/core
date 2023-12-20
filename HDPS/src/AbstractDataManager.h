// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"

#include <QObject>
#include <QString>

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
 * @author Thomas Kroes
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

    /**
     * Add \p rawData to the data manager
     * @param rawData Pointer to the raw data to add
     */
    virtual void addRawData(plugin::RawData* rawData) = 0;

    /**
     * Add \p dataset to the data manager
     * @param dataset Dataset to added
     * @param parentDataset Parent dataset (if any)
     * @param visible Whether \p dataset should be visible or not
     * @param notify Whether to notify the core that a dataset is added
     */
    virtual void addDataset(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, bool visible = true, bool notify = true) = 0;

    /**
     * Add \p selection to the data manager
     * @param rawDataName Name of the raw data
     * @param selection Smart pointer to selection dataset
     */
    virtual void addSelection(const QString& rawDataName, Dataset<DatasetImpl> selection) = 0;

    /**
     * Remove \p dataset from the manager
     * Other datasets derived from this dataset are converted to non-derived data
     * @param dataset Smart pointer to dataset to remove
     */
    virtual void removeDataset(Dataset<DatasetImpl> dataset) = 0;

    /**
     * Remove \p dataset supervised (using a user interface)
     * Interface allows the user to select which descendants should be removed
     * This method calls AbstractDataManager::removeDataset()
     * @param dataset Smart pointer to dataset to remove
     */
    virtual void removeDatasetSupervised(Dataset<DatasetImpl> dataset) = 0;

    /**
     * Get raw data by name
     * @param name Name of the raw data
     */
    virtual plugin::RawData& getRawData(const QString& name) = 0;

    /**
     * Get dataset by \p datasetId
     * @param datasetId GUID of the dataset
     * @return Smart pointer to the dataset
     */
    virtual Dataset<DatasetImpl> getSet(const QString& datasetId) = 0;

    /**
     * Get selection by \p rawDataName
     * @param rawDataName Name of the raw data
     * @return Smart pointer to the selection dataset
     */
    virtual Dataset<DatasetImpl> getSelection(const QString& rawDataName) = 0;

    /** Get all sets from the data manager */
    virtual QVector<Dataset<DatasetImpl>> allSets() const = 0;

signals:

    /**
     * Signals that \p rawData is added to the data manager
     * @param rawData Pointer to raw data that was added
     */
    void rawDataAdded(plugin::RawData* rawData);

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
     * Signals that dataset with \p datasetGuid is removed from the data manager
     * @param datasetGuid GUID of the removed dataset
     */
    void datasetRemoved(const QString& datasetGuid);
};

}