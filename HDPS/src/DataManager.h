#pragma once

#include "Set.h"
#include "CoreInterface.h"

#include <actions/WidgetAction.h>

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

namespace hdps
{

namespace plugin {
    class RawData;
}

class DataManager : public hdps::gui::WidgetAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param core Pointer to the core
     */
    DataManager(CoreInterface* core) :
        WidgetAction(),
        _core(core)
    {
        setText("Data manager");
        setObjectName("Datasets");
    }

    /**
     * Add raw data to the data manager
     * @param rawData Pointer to the raw data
     */
    void addRawData(plugin::RawData* rawData);

    /**
     * Add data set to the data manager
     * @param dataset Smart pointer to the dataset
     */
    void addSet(const Dataset<DatasetImpl>& dataset);

    /**
     * Add selection to the data manager
     * @param dataName Name of the raw data
     * @param selection Smart pointer to selection dataset
     */
    void addSelection(const QString& dataName, Dataset<DatasetImpl> selection);

    /**
     * Removes a Dataset, other datasets derived from this dataset are converted to non-derived data (notifies listeners)
     * @param dataset Smart pointer to dataset to remove
     */
    void removeDataset(Dataset<DatasetImpl> dataset);

    /**
     * Get raw data by name
     * @param name Name of the raw data
     */
    plugin::RawData& getRawData(const QString& name);

    /**
     * Get dataset by dataset GUID
     * @param datasetGuid GUID of the dataset
     * @return Smart pointer to the dataset
     */
    Dataset<DatasetImpl> getSet(const QString& datasetGuid);

    /**
     * Get selection by data name
     * @param dataName Name of the data
     * @return Smart pointer to the selection dataset
     */
    Dataset<DatasetImpl> getSelection(const QString& dataName);

    /** Get all sets from the data manager */
    const std::unordered_map<QString, Dataset<DatasetImpl>>& allSets() const;

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

signals:
    void dataChanged();

private:

    /**
     * Stores all raw data in the system. Raw data is stored by the name
     * retrieved from their Plugin::getName() function.
     */
    std::unordered_map<QString, std::unique_ptr<plugin::RawData>> _rawDataMap;

    /**
     * Stores all data sets in the system. Data sets are stored by the name
     * retrieved from their DataSet::getName() function.
     */
    std::unordered_map<QString, Dataset<DatasetImpl>> _dataSetMap;

    /**
    * Stores selection sets on all data plug-ins
    * NOTE: Can't be a QMap because it doesn't support move semantics of unique_ptr
    */
    std::unordered_map<QString, Dataset<DatasetImpl>> _selections;

    /** Non-owning pointer to core for event notifications */
    CoreInterface* _core;
};

} // namespace hdps
