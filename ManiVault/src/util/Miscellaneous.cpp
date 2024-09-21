// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Miscellaneous.h"

#include <QAction>
#include <QStringList>
#include <QTcpSocket>
#include <QUrl>

namespace mv::util
{

QString getIntegerCountHumanReadable(const float& count)
{
    if (count >= 0 && count < 1000)
        return QString::number(count);

    if (count >= 1000 && count < 1000000)
        return QString("%1 k").arg(QString::number(count / 1000.0f, 'f', 1));

    if (count >= 1000000)
        return QString("%1 mln").arg(QString::number(count / 1000000.0f, 'f', 1));

    return "";
}

QString getNoBytesHumanReadable(float noBytes)
{
    QStringList list{ "KB", "MB", "GB", "TB" };

    QStringListIterator it(list);
    QString unit("bytes");

    while (noBytes >= 1024.0 && it.hasNext())
    {
        unit = it.next();
        noBytes /= 1024.0;
    }

    return QString::number(noBytes, 'f', 2) + " " + unit;
}

CORE_EXPORT QString getColorAsCssString(const QColor& color, bool alpha /*= true*/)
{
    if (alpha)
        return QString("rgba(%1, %2, %3, %4)").arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue()), QString::number(color.alpha()));
    else
        return QString("rgb(%1, %2, %3)").arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue()));
}

bool urlExists(const QString& urlString)
{
    QUrl url(urlString);

    QTcpSocket socket;

    socket.connectToHost(url.host(), 80);

    if (socket.waitForConnected()) {
        socket.write("HEAD " + url.path().toUtf8() + " HTTP/1.1\r\n" "Host: " + url.host().toUtf8() + "\r\n\r\n");

        if (socket.waitForReadyRead()) {
            QByteArray bytes = socket.readAll();

            if (bytes.contains("200 OK"))
                return true;
        }
    }
    return false;
}

void replaceLayout(QWidget* widget, QLayout* newLayout, bool removeWidgets)
{
    Q_ASSERT(widget && newLayout);

    try {
        if (!widget)
            throw std::runtime_error("Widget may not be a nullptr");

        if (!newLayout)
            throw std::runtime_error("New layout may not be a nullptr");

        if (auto oldLayout = widget->layout()) {
            QLayoutItem* item = nullptr;

            while ((item = oldLayout->takeAt(0)) != nullptr) {
                if (removeWidgets)
                    delete item->widget();

                delete item;
            }

            delete oldLayout;
        }

        widget->setLayout(newLayout);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to replace widget layout", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to replace widget layout");
    }
}

}
