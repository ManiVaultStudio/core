#include "DataManager.h"

namespace hdps {

void DataManager::addSet(Set* set)
{
    dataSets.append(set);
}

void DataManager::addSelection(QString dataName, Set* selection)
{
    selections.insert(dataName, selection);
}

Set* DataManager::getSet(QString name)
{
    for (Set* set : dataSets) {
        if (set->getName() == name) {
            return set;
        }
    }
    qFatal((QString("Failed to find set with name: ") + name).toStdString().c_str());
}

Set* DataManager::getSelection(QString name)
{
    return selections[name];
}

const QVector<Set*>& DataManager::allSets() {
    return dataSets;
}

} // namespace hdps
