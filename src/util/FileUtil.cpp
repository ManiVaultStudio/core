// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "FileUtil.h"

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QString>
#include <QTextStream>
#include <QUrl>

namespace mv
{
namespace util
{

QString loadFileContents(const QString& path) {
    QFile file(path);

    QString contents = "";
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        contents = stream.readAll();
    }

    return contents;
}

bool showFileInFolder(const QString& path)
{
    const QFileInfo info(path);

    if (!info.exists())
    {
        return false;
    }

    // Based upon: How to "Reveal in Finder" or "Show in Explorer" with Qt
    // https://stackoverflow.com/questions/3490336/how-to-reveal-in-finder-or-show-in-explorer-with-qt
    enum class Os { Windows, Mac, Other };

    constexpr auto os =
#if defined(Q_OS_WIN)
        Os::Windows
#elif defined(Q_OS_MAC)
        Os::Mac
#else
        Os::Other
#endif
        ;

    if (os == Os::Windows)
    {
        const auto args = QStringList{}
            << "/select,"
            << QDir::toNativeSeparators(path);

        if (QProcess::startDetached("explorer.exe", args))
        {
            return true;
        }
    }
    if (os == Os::Mac)
    {
        const auto args = QStringList{}
            << "-e"
            << "tell application \"Finder\""
            << "-e"
            << "activate"
            << "-e"
            << "select POSIX file \"" + path + "\""
            << "-e"
            << "end tell"
            << "-e"
            << "return";

        if (QProcess::execute("/usr/bin/osascript", args) == 0)
        {
            return true;
        }
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(info.path()));
    return true;
}

}

}
