#include "DataManager.h"

namespace hdps {

void DataManager::addSet(Set* set) {
    dataSets.append(set);
}

void DataManager::addSelection(QString dataName, Set* selection) {
    selections.insert(dataName, selection);
}

} // namespace hdps
