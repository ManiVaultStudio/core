#pragma once

#include "RawData.h"
#include "Set.h"

#include <QString>

#include <vector>
#include <unordered_map>
#include <memory>
#include <exception>
#include <string>

namespace hdps
{

struct DataNotFoundException : public std::exception
{
public:
    DataNotFoundException(QString dataName) :
        message((QString("Failed to find data with name: ") + dataName).toStdString()) { }

    const char* what() const throw ()
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

    const char* what() const throw ()
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
    void addSet(Set* set);
    void addSelection(QString dataName, Set* selection);

    plugin::RawData& getRawData(QString name);
    Set& getSet(QString name);
    Set& getSelection(QString name);
    const std::vector<std::unique_ptr<Set>>& allSets();

    const QString getUniqueSetName(QString request);

private:
    std::vector<std::unique_ptr<plugin::RawData>> _rawData;

    std::vector<std::unique_ptr<Set>> _dataSets;
    /**
    * Stores selection sets on all data plugins
    * NOTE: Can't be a QMap because it doesn't support move semantics of unique_ptr
    */
    std::unordered_map<std::string, std::unique_ptr<Set>> _selections;
};

} // namespace hdps
