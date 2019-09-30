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

void DataManager::addSet(Set* set)
{
    _dataSets.push_back(std::unique_ptr<Set>(set));
}

void DataManager::addSelection(QString dataName, Set* selection)
{
    _selections.emplace(dataName.toStdString(), std::unique_ptr<Set>(selection));
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

    Set* selection = _selections[name.toStdString()].get();

    if (!selection)
        throw SelectionNotFoundException(name);

    return *selection;
}

const std::unordered_map<QString, std::unique_ptr<Set>>& DataManager::allSets()
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
