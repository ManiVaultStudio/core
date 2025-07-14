// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectsModelProject.h"

#include "CoreInterface.h"

#include <QTextBrowser>
#include <QtConcurrent>
#include <QFuture>

namespace mv::util {

ProjectsModelProject::ProjectsModelProject(const QVariantMap& variantMap) :
    _title(variantMap.contains("title") ? variantMap["title"].toString() : ""),
    _isGroup(false),
    _group(variantMap.contains("group") ? variantMap["group"].toString() : ""),
    _tags(variantMap.contains("tags") ? variantMap["tags"].toStringList() : QStringList()),
    _date(variantMap.contains("date") ? variantMap["date"].toString() : ""),
    _iconName(variantMap.contains("icon") ? variantMap["icon"].toString() : "database"),
    _summary(variantMap.contains("summary") ? variantMap["summary"].toString() : ""),
    _url(QUrl(variantMap.contains("url") ? variantMap["url"].toString() : "")),
    _minimumHardwareSpec(HardwareSpec::Type::Minimum),
    _recommendedHardwareSpec(HardwareSpec::Type::Recommended),
	_startup(variantMap.contains("startup") ? variantMap["startup"].toBool() : false)

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

    if (variantMap.contains("hardwareRequirements")) {
        const auto hardwareRequirementsMap = variantMap["hardwareRequirements"].toMap();

        if (hardwareRequirementsMap.contains("minimum"))
            _minimumHardwareSpec.fromVariantMap(hardwareRequirementsMap["minimum"].toMap());

        if (hardwareRequirementsMap.contains("recommended"))
            _recommendedHardwareSpec.fromVariantMap(hardwareRequirementsMap["recommended"].toMap());
    }
    
    if (!_missingPlugins.isEmpty())
        qWarning() << "Project" << _title << "is added to the project database but cannot be opened because of missing plugins:" << _missingPlugins.join(", ");

    computeSha();
}

ProjectsModelProject::ProjectsModelProject(const QString& groupTitle) :
    _title(groupTitle),
    _isGroup(true),
    _group(groupTitle),
    _iconName("folder"),
    _minimumHardwareSpec(HardwareSpec::Type::Minimum),
    _recommendedHardwareSpec(HardwareSpec::Type::Recommended)
{
}

void ProjectsModelProject::initialize()
{
    determineDownloadSize();
    determineLastModified();
}

QString ProjectsModelProject::getTitle() const
{
    return _title;
}

QDateTime ProjectsModelProject::getLastModified() const
{
    return _lastModified;
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

std::uint64_t ProjectsModelProject::getDownloadSize() const
{
    return _downloadSize;
}

HardwareSpec ProjectsModelProject::getMinimumHardwareSpec() const
{
    return _minimumHardwareSpec;
}

HardwareSpec ProjectsModelProject::getRecommendedHardwareSpec() const
{
    return _recommendedHardwareSpec;
}

bool ProjectsModelProject::isStartup() const
{
	return _startup;
}

QString ProjectsModelProject::getSha() const
{
    const_cast<ProjectsModelProject*>(this)->computeSha();

	return _sha;
}

void ProjectsModelProject::determineDownloadSize()
{
    FileDownloader::getDownloadSizeAsync(getUrl()).then(this, [this](std::uint64_t size) {
        _downloadSize = size;

        emit downloadSizeDetermined(size);
    }).onFailed(this, [this](const QException& e) {
        qWarning().noquote() << QString("Unable to determine download size for %1: %2").arg(getUrl().toString(), e.what());
    });
}

void ProjectsModelProject::determineLastModified()
{
    FileDownloader::getLastModifiedAsync(getUrl()).then(this, [this](const QDateTime& lastModified) {
        _lastModified = lastModified;

        emit lastModifiedDetermined(lastModified);
        }).onFailed(this, [this](const QException& e) {
            qWarning().noquote() << QString("Unable to determine last modified for %1: %2").arg(getUrl().toString(), e.what());
    });
}

void ProjectsModelProject::computeSha()
{
    QCryptographicHash hash(QCryptographicHash::Sha256);

    hash.addData(_title.toUtf8());
    hash.addData(_lastModified.toString().toUtf8());
    hash.addData(_tags.join(",").toUtf8());
    hash.addData(_date.toUtf8());
    hash.addData(_iconName.toUtf8());
    hash.addData(_summary.toUtf8());
    hash.addData(_url.toString().toUtf8());
    hash.addData(_requiredPlugins.join(",").toUtf8());

    _sha = hash.result().toHex();
}

}
