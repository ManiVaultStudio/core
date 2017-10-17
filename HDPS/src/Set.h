#ifndef HDPS_SET_H
#define HDPS_SET_H

#include <QString>
#include <QVector>

namespace hdps {

class Set {
public:
    Set(QString dataName) : _dataName(dataName), _all(false) {}
    virtual ~Set() {}

    virtual Set* copy() const = 0;

    QString getName() const
    {
        return _name;
    }

    QString getDataName() const
    {
        return _dataName;
    }

    bool isFull() const
    {
        return _all;
    }

    void setName(QString name)
    {
        _name = name;
    }

    void setAll()
    {
        _all = true;
    }
private:
    QString _name;
    QString _dataName;
    bool _all;
};

} // namespace hdps

#endif // HDPS_SET_H
