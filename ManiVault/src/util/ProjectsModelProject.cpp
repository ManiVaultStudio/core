// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectsModelProject.h"

#include "CoreInterface.h"

namespace mv::util {

ProjectsModelProject::ProjectsModelProject(const QVariantMap& variantMap) :
    _title(variantMap.contains("title") ? variantMap["title"].toString() : ""),
    _isGroup(false),
    _group(variantMap.contains("group") ? variantMap["group"].toString() : ""),
    _tags(variantMap.contains("tags") ? variantMap["tags"].toStringList() : QStringList()),
    _date(variantMap.contains("date") ? variantMap["date"].toString() : ""),
    _iconName(variantMap.contains("icon") ? variantMap["icon"].toString() : "file"),
    _summary(variantMap.contains("summary") ? variantMap["summary"].toString() : ""),
    _url(QUrl(variantMap.contains("url") ? variantMap["url"].toString() : "")),
    _size(variantMap.contains("size") ? variantMap["size"].toString() : "NA")
{
    if (variantMap.contains("coreVersion")) {
        const auto coreVersionMap = variantMap["coreVersion"].toMap();

        if (coreVersionMap.contains("major"))
			_minimumCoreVersion.setMajor(coreVersionMap["major"].toInt());

        if (coreVersionMap.contains("minor"))
            _minimumCoreVersion.setMinor(coreVersionMap["minor"].toInt());

        if (coreVersionMap.contains("patch"))
            _minimumCoreVersion.setPatch(coreVersionMap["patch"].toInt());

        if (coreVersionMap.contains("suffix"))
            _minimumCoreVersion.setSuffix(coreVersionMap["suffix"].toString().toStdString());
    }

    if (variantMap.contains("requiredPlugins")) {
        const auto requiredPluginsMap = variantMap["requiredPlugins"].toList();

        for (const auto& requiredPluginVariant : requiredPluginsMap) {
            const auto requiredPluginMap = requiredPluginVariant.toMap();

            if (requiredPluginMap.contains("name")) {
                const auto requiredPluginName = requiredPluginMap["name"].toString();

                _requiredPlugins << requiredPluginName;

                if (!mv::plugins().isPluginLoaded(requiredPluginName)) {
                    _missingPlugins << requiredPluginName;
                }
            }
        }
    }

    if (!_missingPlugins.isEmpty())
        qWarning() << "Project" << _title << "is added to the project database but cannot be opened because of missing plugins:" << _missingPlugins.join(", ");
}

ProjectsModelProject::ProjectsModelProject(const QString& groupTitle) :
    _title(groupTitle),
    _isGroup(true),
    _group(groupTitle),
    _iconName("folder")
{
}

void ProjectsModelProject::load() const
{
    mv::projects().openProject(getUrl());
}

QString ProjectsModelProject::getTitle() const
{
    return _title;
}

bool ProjectsModelProject::isDownloaded() const
{
    const auto downloadedProjectFilePath = mv::projects().getDownloadedProjectsDir().filePath(getUrl().fileName());

	QFileInfo fileInfo(downloadedProjectFilePath);

    fileInfo.refresh();

    return fileInfo.exists() && fileInfo.isFile();
}

bool ProjectsModelProject::isGroup() const
{
    return _isGroup;
}

QString ProjectsModelProject::getGroup() const
{
    return _group;
}

QStringList ProjectsModelProject::getTags() const
{
    return _tags;
}

QString ProjectsModelProject::getDate() const
{
    return _date;
}

QString ProjectsModelProject::getIconName() const
{
    return _iconName;
}

QString ProjectsModelProject::getSummary() const
{
    return _summary;
}

QUrl ProjectsModelProject::getUrl() const
{
    return _url;
}

Version ProjectsModelProject::getMinimumCoreVersion() const
{
    return _minimumCoreVersion;
}

QStringList ProjectsModelProject::getRequiredPlugins() const
{
    return _requiredPlugins;
}

QStringList ProjectsModelProject::getMissingPlugins() const
{
    return _missingPlugins;
}

QString ProjectsModelProject::getSize() const
{
    return _size;
}

}
