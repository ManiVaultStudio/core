#pragma once

#include <QString>

#include <exception>

struct SetNotFoundException : public std::exception
{
public:
    SetNotFoundException(QString setName) : setName(setName) { }

    const char* what() const throw ()
    {
        return (QString("Failed to find a set with name: ") + setName).toStdString().c_str();
    }

private:
    QString setName;
};
