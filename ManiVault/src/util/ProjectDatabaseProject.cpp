// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectDatabaseProject.h"

#include "CoreInterface.h"

namespace mv::util {

ProjectDatabaseProject::ProjectDatabaseProject(const QVariantMap& variantMap) :
    _title(variantMap.contains("title") ? variantMap["title"].toString() : ""),
    _group(variantMap.contains("group") ? variantMap["group"].toString() : ""),
    _tags(variantMap.contains("tags") ? variantMap["tags"].toStringList() : QStringList()),
    _date(variantMap.contains("date") ? variantMap["date"].toString() : ""),
    _iconName(variantMap.contains("icon") ? variantMap["icon"].toString() : ""),
    _summary(variantMap.contains("summary") ? variantMap["summary"].toString() : ""),
    _url(QUrl(variantMap.contains("url") ? variantMap["url"].toString() : ""))
    
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

const QString& ProjectDatabaseProject::getTitle() const
{
    return _title;
}

const QString& ProjectDatabaseProject::getGroup() const
{
    return _group;
}

const QStringList& ProjectDatabaseProject::getTags() const
{
    return _tags;
}

const QString& ProjectDatabaseProject::getDate() const
{
    return _date;
}

const QString& ProjectDatabaseProject::getIconName() const
{
    return _iconName;
}

const QString& ProjectDatabaseProject::getSummary() const
{
    return _summary;
}

const QUrl& ProjectDatabaseProject::getUrl() const
{
    return _url;
}

const Version& ProjectDatabaseProject::getMinimumCoreVersion() const
{
    return _minimumCoreVersion;
}

const QStringList& ProjectDatabaseProject::getRequiredPlugins() const
{
    return _requiredPlugins;
}

const QStringList& ProjectDatabaseProject::getMissingPlugins() const
{
    return _missingPlugins;
}

}
