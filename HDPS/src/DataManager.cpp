#include "DataManager.h"

#include <QRegularExpression>
#include <cassert>
#include <iostream>

namespace hdps
{

void DataManager::addRawData(plugin::RawData* rawData)
{
    _rawData.push_back(std::unique_ptr<plugin::RawData>(rawData));
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
    for (const auto& rawData : _rawData)
    {
        if (rawData->getName() == name)
        {
            return *rawData.get();
        }
    }

    throw DataNotFoundException(name);
}

Set& DataManager::getSet(QString name)
{
    for (const auto& set : _dataSets)
    {
        if (set->getName() == name)
        {
            return *set.get();
        }
    }

    throw SetNotFoundException(name);
}

Set& DataManager::getSelection(QString name)
{
    Set* selection = _selections[name.toStdString()].get();
    assert(selection != nullptr);
    return *selection;
}

const std::vector<std::unique_ptr<Set>>& DataManager::allSets()
{
    return _dataSets;
}

const QString DataManager::getUniqueSetName(QString request)
{
    for (const auto& set : allSets())
    {
        if (set->getName() == request)
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
