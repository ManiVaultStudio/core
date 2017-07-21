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
    Set* getSet(QString name);
    Set* getSelection(QString name);
    const QVector<Set*>& allSets();

    const QString getUniqueSetName(QString request);

private:
    QVector<Set*> _dataSets;
    QMap<QString, Set*> _selections;
};

} // namespace hdps

#endif // HDPS_DATA_MANAGER_H
