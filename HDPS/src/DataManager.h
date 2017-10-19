#ifndef HDPS_DATA_MANAGER_H
#define HDPS_DATA_MANAGER_H

#include "Set.h"

#include <QString>
#include <QVector>

#include <unordered_map>
#include <memory>

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
    /**
     * Stores selection sets on all data plugins
     * NOTE: Can't be a QMap because it doesn't support move semantics of unique_ptr
     */
    std::unordered_map<std::string, std::unique_ptr<Set>> _selections;
};

} // namespace hdps

#endif // HDPS_DATA_MANAGER_H
