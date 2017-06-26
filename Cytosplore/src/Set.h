#ifndef HDPS_SET_H
#define HDPS_SET_H

#include <QString>
#include <QVector>

namespace hdps {

class Set {
public:
    Set() : all(false) {};

    QString getDataName() { return dataName; }

    void setAll() { this->all = true; }
private:
    QString dataName;
    bool all;
};

} // namespace hdps

#endif // HDPS_SET_H
