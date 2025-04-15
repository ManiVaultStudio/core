// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningCenterTutorial.h"

namespace mv::util {

LearningCenterTutorial::LearningCenterTutorial(const QVariantMap& variantMap) :
    _title(variantMap.contains("title") ? variantMap["title"].toString() : ""),
    _tags(variantMap.contains("tags") ? variantMap["tags"].toStringList() : QStringList()),
    _date(variantMap.contains("date") ? variantMap["date"].toString() : ""),
    _iconName(variantMap.contains("icon") ? variantMap["icon"].toString() : ""),
    _summary(variantMap.contains("summary") ? variantMap["summary"].toString() : ""),
    _content(variantMap.contains("fullpost") ? variantMap["fullpost"].toString() : ""),
    _url(QUrl(variantMap.contains("url") ? variantMap["url"].toString() : "")),
    _projectUrl(variantMap.contains("project") ? QUrl(variantMap["project"].toString()) : QUrl())
{
    if (variantMap.contains("minimum-version-major"))
        _minimumCoreVersion.setMajor(variantMap["minimum-version-major"].toInt());

    if (variantMap.contains("minimum-version-minor"))
        _minimumCoreVersion.setMinor(variantMap["minimum-version-minor"].toInt());

    _minimumCoreVersion.setPatch(0);

    if (variantMap.contains("plugins")) {
        for (const auto& requiredPluginVariant : variantMap["plugins"].toList()) {
            const auto requiredPluginName = requiredPluginVariant.toString();

			_requiredPlugins << requiredPluginName;

            if (!mv::plugins().isPluginLoaded(requiredPluginName)) {
				_missingPlugins << requiredPluginName;
            }
        }
    }

    if (_requiredPlugins.isEmpty()) {
        qWarning() << "Tutorial" << _title << "does not specify any required plugins. The tutorial can be loaded but there is no guarantee that its required plugins are available";
    }

    if (!_missingPlugins.isEmpty()) {
        qWarning() << "Tutorial" << _title << "is added to the tutorials model but likely will not open properly because the following plugins are not available: " << _missingPlugins.join(", ");
    }
}

const QString& LearningCenterTutorial::getTitle() const
{
    return _title;
}

const QStringList& LearningCenterTutorial::getTags() const
{
    return _tags;
}

const QString& LearningCenterTutorial::getDate() const
{
    return _date;
}

const QString& LearningCenterTutorial::getIconName() const
{
    return _iconName;
}

const QString& LearningCenterTutorial::getSummary() const
{
    return _summary;
}

const QString& LearningCenterTutorial::getContent() const
{
    return _content;
}

const QUrl& LearningCenterTutorial::getUrl() const
{
    return _url;
}

const QUrl& LearningCenterTutorial::getProjectUrl() const
{
    return _projectUrl;
}

const Version& LearningCenterTutorial::getMinimumCoreVersion() const
{
    return _minimumCoreVersion;
}

const QStringList& LearningCenterTutorial::getRequiredPlugins() const
{
    return _requiredPlugins;
}

const QStringList& LearningCenterTutorial::getMissingPlugins() const
{
    return _missingPlugins;
}

bool LearningCenterTutorial::hasProject() const
{
    return !_projectUrl.isEmpty();
}

}
