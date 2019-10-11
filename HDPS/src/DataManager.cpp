#include "DataManager.h"

#include <QRegularExpression>
#include <cassert>
#include <iostream>

namespace hdps
{

void DataManager::addRawData(plugin::RawData* rawData)
{
    _rawDataMap.emplace(rawData->getName(), std::unique_ptr<plugin::RawData>(rawData));
}

QString DataManager::addSet(QString requestedName, Set* set)
{
    QString uniqueName = getUniqueSetName(requestedName);
    set->setName(uniqueName);
    _dataSetMap.emplace(set->getName(), std::unique_ptr<Set>(set));
    return uniqueName;
}

void DataManager::addSelection(QString dataName, Set* selection)
{
    _selections.emplace(dataName, std::unique_ptr<Set>(selection));
}

QStringList DataManager::removeRawData(QString name)
{
    // Convert any derived data referring to this data to non-derived data
    for (auto& pair : _rawDataMap)
    {
        plugin::RawData& rawData = *pair.second;

        // Set as non-derived data
        if (rawData.isDerivedData() && rawData.getSourceDataName() == name)
        {
            rawData.setDerived(false, QString());
        }
        
        // Generate a selection set for the previously derived data
        Set* selection = rawData.createSet();
        addSelection(rawData.getName(), selection);
    }

    // Remove any sets referring to this data, and keep track of the removed set names
    QStringList removedSets;
    for (auto it = _dataSetMap.begin(); it != _dataSetMap.end();)
    {
        const Set& set = *it->second;
        if (set.getDataName() == name)
        {
            removedSets.append(set.getName());
            it = _dataSetMap.erase(it);
        }
        else
            it++;
    }

    // Remove the selection belonging to the raw data
    _selections.erase(name);

    // Remove the raw data
    _rawDataMap.erase(name);

    return removedSets;
}

plugin::RawData& DataManager::getRawData(QString name)
{
    if (_rawDataMap.find(name) == _rawDataMap.end())
        throw DataNotFoundException(name);

    return *_rawDataMap[name];
}

Set& DataManager::getSet(QString name)
{
    if (_dataSetMap.find(name) == _dataSetMap.end())
        throw SetNotFoundException(name);

    return *_dataSetMap[name];
}

Set& DataManager::getSelection(QString name)
{
    plugin::RawData& rawData = getRawData(name);
    if (rawData.isDerivedData())
    {
        return getSelection(rawData.getSourceData().getName());
    }

    Set* selection = _selections[name].get();

    if (!selection)
        throw SelectionNotFoundException(name);

    return *selection;
}

const std::unordered_map<QString, std::unique_ptr<Set>>& DataManager::allSets() const
{
    return _dataSetMap;
}

const QString DataManager::getUniqueSetName(QString request)
{
    for (const auto& pair : allSets())
    {
        const Set& set = *pair.second;
        if (set.getName() == request)
        {
            // Index in the string where the underscore followed by digits starts
            int index = request.lastIndexOf(QRegularExpression("_\\d+"));

            // If the regular expression was not found create the first copy
            if (index == -1)
            {
                return getUniqueSetName(request + "_1");
            }
            else
            {
                // Number of characters used by the digits we need to replace
                int numChars = request.length() - (index + 1);
                // The digit we want to increment and place back
                int digit = request.right(numChars).toInt() + 1;
                return getUniqueSetName(request.left(request.length() - numChars) + QString::number(digit));
            }
        }
    }
    return request;
}

} // namespace hdps
