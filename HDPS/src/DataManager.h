#pragma once

#include "RawData.h"
#include "Set.h"

#include <QString>

#include <string>
#include <unordered_map>    // Data is stored in maps
#include <memory>           // Data is stored in unique pointers
#include <exception>        // Used for defining custom exceptions
#include <functional>       // Necessary for hash function

// Hash specialization for QString so we can use it as a key in the data maps
namespace std {
    template<> struct hash<QString> {
        std::size_t operator()(const QString& s) const {
            return qHash(s);
        }
    };
}

namespace hdps
{

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

class DataManager
{
public:
    void addRawData(plugin::RawData* rawData);
    QString addSet(QString requestedName, Set* set);
    void addSelection(QString dataName, Set* selection);

    plugin::RawData& getRawData(QString name);
    Set& getSet(QString name);
    Set& getSelection(QString name);
    const std::unordered_map<QString, std::unique_ptr<Set>>& allSets();

    /**
     * Generates a unique set name based on a requested name.
     * If the requested name already is the name of a stored data set, it will suffix
     * the name with incrementing numbers until the name is unique and then return it.
     * If the requested name is not in use, then the request will be returned as is.
     */
    const QString getUniqueSetName(QString request);

private:
    /**
     * Stores all raw data in the system. Raw data is stored by the name
     * retrieved from their Plugin::getName() function.
     */
    std::unordered_map<QString, std::unique_ptr<plugin::RawData>> _rawDataMap;

    /**
     * Stores all data sets in the system. Data sets are stored by the name
     * retrieved from their Set::getName() function.
     */
    std::unordered_map<QString, std::unique_ptr<Set>> _dataSetMap;

    /**
    * Stores selection sets on all data plug-ins
    * NOTE: Can't be a QMap because it doesn't support move semantics of unique_ptr
    */
    std::unordered_map<QString, std::unique_ptr<Set>> _selections;
};

} // namespace hdps
