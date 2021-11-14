#ifndef HDPS_DATAMANAGER_H
#define HDPS_DATAMANAGER_H

#include "Set.h"
#include "CoreInterface.h"

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
    namespace plugin
    {
        class RawData;
    }

struct DataNotFoundException : public std::exception
{
public:
    DataNotFoundException(QString dataName) :
        message((QString("Failed to find raw data with name: ") + dataName).toStdString()) { }

    const char* what() const throw () override
    {
        return message.c_str();
    }

private:
    std::string message;
};

struct SetNotFoundException : public std::exception
{
public:
    SetNotFoundException(QString setName) :
        message((QString("Failed to find a set with name: ") + setName).toStdString())
    { }

    const char* what() const throw () override
    {
        return message.c_str();
    }

private:
    std::string message;
};

struct SelectionNotFoundException : public std::exception
{
public:
    SelectionNotFoundException(QString name) :
        message((QString("Failed to find a selection for raw data: ") + name).toStdString())
    { }

    const char* what() const throw () override
    {
        return message.c_str();
    }

private:
    std::string message;
};

class DataManager : public QObject
{
    Q_OBJECT
public:
    DataManager(CoreInterface* core) :
        _core(core)
    {

    }

    void addRawData(plugin::RawData* rawData);

    /**
     * Add data set to the data manager
     * @param dataSet Reference to the data set
     */
    void addSet(DataSet& dataSet);

    void addSelection(QString dataName, DataSet* selection);

    /**
     * Removes a Dataset. Other datasets derived from this dataset are
     * converted to non-derived data.
     * Notifies all plug-ins of the removed dataset automatically.
     * @param datasetName Name of the (top-level) dataset to remove
     * @param recursively Remove datasets recursively
     */
    void removeDataset(const QString& datasetName, const bool& recursively = true);

    plugin::RawData& getRawData(QString name);

    /**
     * Get set by dataset globally unique identifier
     * @param datasetId Globally unique identifier of the dataset
     * @return Reference to the dataset
     */
    DataSet& getSet(const QString& datasetId);

    DataSet& getSelection(QString name);
    const std::unordered_map<QString, std::unique_ptr<DataSet>>& allSets() const;

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
    std::unordered_map<QString, std::unique_ptr<DataSet>> _dataSetMap;

    /**
    * Stores selection sets on all data plug-ins
    * NOTE: Can't be a QMap because it doesn't support move semantics of unique_ptr
    */
    std::unordered_map<QString, std::unique_ptr<DataSet>> _selections;

    /** Non-owning pointer to core for event notifications */
    CoreInterface* _core;
};

} // namespace hdps

#endif // HDPS_DATAMANAGER_H
