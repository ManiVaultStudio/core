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

namespace hdps
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
     * Add raw data to the data manager
     * @param rawData Pointer to the raw data
     */
    virtual void addRawData(plugin::RawData* rawData) = 0;

    /**
     * Add data set to the data manager
     * @param dataset Smart pointer to the dataset
     */
    virtual void addSet(const Dataset<DatasetImpl>& dataset) = 0;

    /**
     * Add selection to the data manager
     * @param dataName Name of the raw data
     * @param selection Smart pointer to selection dataset
     */
    virtual void addSelection(const QString& dataName, Dataset<DatasetImpl> selection) = 0;

    /**
     * Removes a Dataset, other datasets derived from this dataset are converted to non-derived data (notifies listeners)
     * @param dataset Smart pointer to dataset to remove
     */
    virtual void removeDataset(Dataset<DatasetImpl> dataset) = 0;

    /**
     * Get raw data by name
     * @param name Name of the raw data
     */
    virtual plugin::RawData& getRawData(const QString& name) = 0;

    /**
     * Get dataset by dataset GUID
     * @param datasetGuid GUID of the dataset
     * @return Smart pointer to the dataset
     */
    virtual Dataset<DatasetImpl> getSet(const QString& datasetGuid) = 0;

    /**
     * Get selection by data name
     * @param dataName Name of the data
     * @return Smart pointer to the selection dataset
     */
    virtual Dataset<DatasetImpl> getSelection(const QString& dataName) = 0;

    /** Get all sets from the data manager */
    virtual const QVector<Dataset<DatasetImpl>>& allSets() const = 0;
};

}