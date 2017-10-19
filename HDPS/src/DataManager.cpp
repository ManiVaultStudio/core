#include "DataManager.h"

#include <QRegularExpression>
#include <iostream>

namespace hdps {

void DataManager::addSet(Set* set)
{
    _dataSets.push_back(std::move(std::unique_ptr<Set>(set)));
}

void DataManager::addSelection(QString dataName, Set* selection)
{
    _selections.emplace(dataName.toStdString(), std::move(std::unique_ptr<Set>(selection)));
}

Set* DataManager::getSet(QString name)
{
    for (const auto& set : _dataSets)
    {
        if (set->getName() == name)
        {
            return set.get();
        }
    }
    qFatal((QString("Failed to find set with name: ") + name).toStdString().c_str());
    return nullptr;
}

Set* DataManager::getSelection(QString name)
{
    return _selections[name.toStdString()].get();
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
            else {
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
