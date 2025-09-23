// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StandardPaths.h"

#include <QOperatingSystemVersion>
#include <QStandardPaths>

namespace mv::util {

QString StandardPaths::get(StandardLocation location)
{
	switch (location) {
		case Plugins:       return getPluginsDirectory();
        case Downloads:     return getDownloadsDirectory();
        case Customization: return getCustomizationDirectory();
        case Projects:      return getProjectsDirectory();
        case Logs:          return getLogsDirectory();
	}

    return {};
}

QString StandardPaths::getPluginsDirectory()
{
    const auto applicationDir   = QDir(QCoreApplication::applicationDirPath());
    const auto dirName          = QString("Plugins");

    auto pathSuffix = QString("/%1").arg(dirName);

    if (isMacOS())
        pathSuffix = applicationDir.dirName() == "MacOS" ? QString("../../../%1").arg(dirName) : QString("/../%1").arg(dirName);

    return QDir::cleanPath(applicationDir.path() + pathSuffix);
}

QString StandardPaths::getDownloadsDirectory()
{
    const auto applicationDir       = QDir(QCoreApplication::applicationDirPath());
    const auto applicationDataDir   = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    const auto dirName              = QString("Downloads");

    if (isMacOS()) {
        const auto pathSuffix = applicationDir.dirName() == "MacOS" ? QString("../../../%1").arg(dirName) : QString("/../%1").arg(dirName);

        [[maybe_unused]] auto resultMacOS = applicationDir.mkpath(pathSuffix);

        return QDir::cleanPath(applicationDir.path() + pathSuffix);
    }

    [[maybe_unused]] auto resultOther = applicationDataDir.mkpath(QString("/%1").arg(dirName));

    return QDir::cleanPath(applicationDataDir.path() + QString("/%1").arg(dirName));
}

QString StandardPaths::getCustomizationDirectory()
{
    const auto applicationDir   = QDir(QCoreApplication::applicationDirPath());
    const auto dirName          = QString("Customization");

    QString pathSuffix = QString("/%1//").arg(dirName);

    if (isMacOS())
        pathSuffix = applicationDir.dirName() == "MacOS" ? QString("../../../%1").arg(dirName) : QString("/../%1").arg(dirName);

    [[maybe_unused]] auto result = applicationDir.mkpath(pathSuffix);

    return QDir::cleanPath(applicationDir.path() + pathSuffix);
}

QString StandardPaths::getProjectsDirectory()
{
    const auto documentLocations = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    QDir documentLocationsDir(documentLocations);

    const auto dirSuffix = QString("%1/%2/Projects").arg(Application::organizationName(), Application::getBaseName());

    [[maybe_unused]] auto result = documentLocationsDir.mkpath(dirSuffix);

    if (documentLocations.isEmpty())
        return QDir::homePath();

    return documentLocationsDir.filePath(dirSuffix);
}

QString StandardPaths::getLogsDirectory()
{
    const auto documentLocations = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    QDir documentLocationsDir(documentLocations);

    const auto dirSuffix = QString("%1/%2/Logs").arg(Application::organizationName(), Application::getBaseName());

    [[maybe_unused]] auto result = documentLocationsDir.mkpath(dirSuffix);

    if (documentLocations.isEmpty())
        return QDir::homePath();

    return documentLocationsDir.filePath(dirSuffix);
}

bool StandardPaths::isMacOS()
{
    return QOperatingSystemVersion::currentType() == QOperatingSystemVersion::MacOS;
}

}
