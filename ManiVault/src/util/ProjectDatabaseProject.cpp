// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectDatabaseProject.h"

namespace mv::util {

ProjectDatabaseProject::ProjectDatabaseProject(const QVariantMap& variantMap) :
    _title(variantMap.contains("Title") ? variantMap["Title"].toString() : ""),
    _tags(variantMap.contains("Tags") ? variantMap["Tags"].toStringList() : QStringList()),
    _date(variantMap.contains("Date") ? variantMap["Date"].toString() : ""),
    _iconName(variantMap.contains("icon") ? variantMap["icon"].toString() : ""),
    _summary(variantMap.contains("Summary") ? variantMap["Summary"].toString() : ""),
    _url(QUrl(variantMap.contains("Url") ? variantMap["Url"].toString() : ""))
    
{
    if (variantMap.contains("CoreVersion")) {
        const auto coreVersionMap = variantMap["CoreVersion"].toMap();

        qDebug() << coreVersionMap;

        if (coreVersionMap.contains("Major"))
			_minimumCoreVersion.setMajor(coreVersionMap["Major"].toInt());

        if (coreVersionMap.contains("Minor"))
            _minimumCoreVersion.setMinor(coreVersionMap["Minor"].toInt());

        if (coreVersionMap.contains("Patch"))
            _minimumCoreVersion.setPatch(coreVersionMap["Patch"].toInt());

        if (coreVersionMap.contains("Suffix"))
            _minimumCoreVersion.setSuffix(coreVersionMap["Suffix"].toString().toStdString());
    }

    if (variantMap.contains("RequiredPlugins")) {
        const auto requiredPluginsMap = variantMap["RequiredPlugins"].toList();

        for (const auto& requiredPluginVariant : requiredPluginsMap) {
            const auto requiredPluginMap = requiredPluginVariant.toMap();

            if (requiredPluginMap.contains("Name")) {
                const auto requiredPluginName = requiredPluginMap["Name"].toString();

                _requiredPlugins << requiredPluginName;

                if (!mv::plugins().isPluginLoaded(requiredPluginName)) {
                    _missingPlugins << requiredPluginName;
                }
            }
        }
    }
}

const QString& ProjectDatabaseProject::getTitle() const
{
    return _title;
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
