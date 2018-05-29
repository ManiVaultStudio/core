#pragma once

#include <QString>
#include <QFile>
#include <QTextStream>

namespace hdps
{
namespace util
{

QString loadFileContents(QString path) {
    QFile file(path);

    QString contents = "";
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        contents = stream.readAll();
    }

    return contents;
}

}

}
