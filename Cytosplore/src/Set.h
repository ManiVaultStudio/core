#ifndef HDPS_SET_H
#define HDPS_SET_H

#include <QString>
#include <QVector>

namespace hdps {

class Set {
public:
    Set() : all(false) {}
    virtual ~Set() {}

    virtual Set* copy() const = 0;

    QString getName() const
    {
        return name;
    }

    QString getDataName() const
    {
        return dataName;
    }

    bool isFull() const
    {
        return all;
    }

    void setName(QString name)
    {
        this->name = name;
    }

    void setDataName(QString name)
    {
        this->dataName = name;
    }

    void setAll()
    {
        this->all = true;
    }
private:
    QString name;
    QString dataName;
    bool all;
};

} // namespace hdps

#endif // HDPS_SET_H
