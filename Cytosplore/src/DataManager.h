#ifndef HDPS_DATA_MANAGER_H
#define HDPS_DATA_MANAGER_H

#include "Set.h"

#include <QString>
#include <QVector>
#include <QMap>

namespace hdps {

class DataManager {
public:
    void addSet(Set* set);
    void addSelection(QString dataName, Set* selection);

private:
    QVector<Set> dataSets;
    QMap<QString, Set> selections;
};

} // namespace hdps

#endif // HDPS_DATA_MANAGER_H
