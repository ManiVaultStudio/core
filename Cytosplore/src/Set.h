#ifndef HDPS_SET_H
#define HDPS_SET_H

#include <QString>
#include <QVector>

namespace hdps {

class Set {
public:
    Set() {};

    QString getDataName();


private:
    QString dataName;
    QVector<unsigned int> indices;
};

} // namespace hdps

#endif // HDPS_SET_H
