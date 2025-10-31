// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectsModelProject.h"

#include "CoreInterface.h"

#include "util/Serializable.h"

#include <QTextBrowser>
#include <QtConcurrent>
#include <QFuture>
#include <QUuid>

#ifdef _DEBUG
    #define PROJECTS_MODEL_PROJECT_VERBOSE
#endif

namespace mv::util {

ProjectsModelProject::ProjectsModelProject(const QVariantMap& variantMap) :
    _title(variantMap.contains("title") ? variantMap["title"].toString() : ""),
    _uuid(variantMap.contains("uuid") ? variantMap["uuid"].toString() : ""),
    _serverDownloadSize(0),
    _userSpecifiedDownloadSize(variantMap.contains("downloadSize") ? parseByteSize(variantMap["downloadSize"].toString()) : 0),
    _isGroup(false),
    _group(variantMap.contains("group") ? variantMap["group"].toString() : ""),
    _tags(variantMap.contains("tags") ? variantMap["tags"].toStringList() : QStringList()),
    _date(variantMap.contains("date") ? variantMap["date"].toString() : ""),
    _iconName(variantMap.contains("icon") ? variantMap["icon"].toString() : "database"),
    _summary(variantMap.contains("summary") ? variantMap["summary"].toString() : ""),
    _url(QUrl(variantMap.contains("url") ? variantMap["url"].toString() : "")),
    _minimumHardwareSpec(HardwareSpec::Type::Minimum),
    _recommendedHardwareSpec(HardwareSpec::Type::Recommended),
    _startup(variantMap.contains("startup") ? variantMap["startup"].toBool() : false),
    _projectsJsonDsn(variantMap.contains("projectsJsonDsn") ? variantMap["projectsJsonDsn"].toUrl() : QUrl()),
    _expanded(false)
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
    updateIcon();
}

ProjectsModelProject::ProjectsModelProject(const QString& groupTitle) :
    _title(groupTitle),
    _uuid(Serializable::createId()),
    _serverDownloadSize(0),
    _isGroup(true),
    _iconName("folder"),
    _minimumHardwareSpec(HardwareSpec::Type::Minimum),
    _recommendedHardwareSpec(HardwareSpec::Type::Recommended),
    _expanded(false)
{
    computeSha();
    updateIcon();
}

QString ProjectsModelProject::getTitle() const
{
    return _title;
}

QString ProjectsModelProject::getUUID() const
{
    return _uuid;
}

QString ProjectsModelProject::getTooltip() const
{
    return _tooltip;
}

QDateTime ProjectsModelProject::getServerLastModified() const
{
    if (getUrl().isEmpty())
        return {};

    return _serverLastModified;
}

std::int64_t ProjectsModelProject::getServerDownloadSize() const
{
    if (getUrl().isEmpty())
        return 0;

    return _serverDownloadSize;
}

std::int64_t ProjectsModelProject::getUserSpecifiedDownloadSize() const
{
    return _userSpecifiedDownloadSize;
}

std::int64_t ProjectsModelProject::getDownloadSize() const
{
    if (const auto userSpecifiedDownloadSize = getUserSpecifiedDownloadSize(); userSpecifiedDownloadSize > 0)
        return userSpecifiedDownloadSize;

    return getServerDownloadSize();
}

QString ProjectsModelProject::getDownloadSizeHumanReadable() const
{
    if (getDownloadSize() <= 0)
        return "Unknown size";

    return getNoBytesHumanReadable(getDownloadSize());
}

bool ProjectsModelProject::isDownloaded() const
{
    QFileInfo fileInfo(getDownloadedProjectFilePath());

    fileInfo.refresh();

    return fileInfo.exists() && fileInfo.isFile();
}

bool ProjectsModelProject::isDownloadedProjectStale() const
{
    if (!isDownloaded())
        return false;

    if (getServerDownloadSize() > 0 && getServerDownloadSize() != getDownloadedProjectFileSize())
        return true;

    if (getServerLastModified().isValid() && getServerLastModified() > getDownloadedProjectLastModified())
        return true;

    return false;
}

bool ProjectsModelProject::requiresDownload() const
{
    if (!isDownloaded())
        return true;

    if (isDownloaded() && isDownloadedProjectStale())
        return true;

    return false;
}

void ProjectsModelProject::setDownloaded()
{
    updateIcon();
    updateTooltip();

    emit downloaded();
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

QUrl ProjectsModelProject::getProjectsJsonDsn() const
{
    return _projectsJsonDsn;
}

void ProjectsModelProject::setProjectsJsonDsn(const QUrl& projectsJsonDsn)
{
    _projectsJsonDsn = projectsJsonDsn;
}

bool ProjectsModelProject::isExpanded() const
{
    return _expanded;
}

void ProjectsModelProject::setExpanded(bool expanded)
{
    if (expanded == _expanded)
        return;

    _expanded = expanded;

    updateIcon();
    updateTooltip();
}

void ProjectsModelProject::updateMetadata()
{
    determineDownloadSize();
    determineLastModified();
}

QIcon ProjectsModelProject::getIcon() const
{
    return _icon;
}

void ProjectsModelProject::determineDownloadSize()
{
    if (getUrl().isEmpty())
        return;

#ifdef PROJECTS_MODEL_PROJECT_VERBOSE
    //qDebug() << __FUNCTION__ << getTitle();
#endif

    auto future     = FileDownloader::getDownloadSizeAsync(getUrl());
    auto watcher    = new QFutureWatcher<std::uint64_t>(this);

    connect(watcher, &QFutureWatcher<std::uint64_t>::finished, watcher, [this, future, watcher]() {
        try {
            if (watcher->future().isCanceled() || watcher->future().isFinished() == false)
                throw std::runtime_error("Future is cancelled or did not finish");

            QMetaObject::invokeMethod(qApp, [this, future]() {
                _serverDownloadSize = future.result();

                emit downloadSizeDetermined(_serverDownloadSize);
            });
        }
        catch (const BaseException& exception) {
            emit downloadSizeDetermined(0);

            qCritical() << "Unable to determine download size for" << getUrl().toDisplayString() << ":" << exception.what();
        }
        catch (const std::exception& exception) {
            emit downloadSizeDetermined(0);

            qCritical() << "Unable to determine download size for" << getUrl().toDisplayString() << ":" << exception.what();
        }
        catch (...) {
            emit downloadSizeDetermined(0);

            qCritical() << "Unable to determine download size for" << getUrl().toDisplayString() << ":" << ", an unknown exception occurred";
        }

        watcher->deleteLater();
    });

    watcher->setFuture(future);
}

void ProjectsModelProject::determineLastModified()
{
    if (getUrl().isEmpty())
        return;

#ifdef PROJECTS_MODEL_PROJECT_VERBOSE
    //qDebug() << __FUNCTION__ << getTitle();
#endif

    auto future     = FileDownloader::getLastModifiedAsync(getUrl());
    auto watcher    = new QFutureWatcher<QDateTime>(this);

    connect(watcher, &QFutureWatcher<QDateTime>::finished, watcher, [this, future, watcher]() {
        try {
            if (watcher->future().isCanceled() || watcher->future().isFinished() == false)
                throw std::runtime_error("Future is cancelled or did not finish");

            QMetaObject::invokeMethod(qApp, [this, future]() {
                _serverLastModified = future.result();

                emit lastModifiedDetermined(_serverLastModified);
            });
        }
        catch (const BaseException& exception) {
            emit lastModifiedDetermined({});

            qCritical() << "Unable to determine download last modified for" << getUrl().toDisplayString() << ":" << exception.what();
        }
        catch (const std::exception& exception) {
            emit lastModifiedDetermined({});

            qCritical() << "Unable to determine download last modified for" << getUrl().toDisplayString() << ":" << exception.what();
        }
        catch (...) {
            emit lastModifiedDetermined({});

            qCritical() << "Unable to determine download last modified for" << getUrl().toDisplayString() << ":" << ", an unknown exception occurred";
        }

        watcher->deleteLater();
    });

    watcher->setFuture(future);
}

void ProjectsModelProject::computeSha()
{
    QCryptographicHash hash(QCryptographicHash::Sha256);

    hash.addData(_title.toUtf8());
    hash.addData(_tags.join(",").toUtf8());
    hash.addData(_iconName.toUtf8());
    hash.addData(_summary.toUtf8());
    hash.addData(_url.toString().toUtf8());
    hash.addData(_requiredPlugins.join(",").toUtf8());

    _sha = hash.result().toHex();
}

void ProjectsModelProject::updateIcon()
{
    if (isGroup()) {
        _icon = isExpanded() ? StyledIcon("folder-open") : StyledIcon("folder");
    } else {
        if (isDownloaded()) {
            switch (HardwareSpec::getSystemCompatibility(getMinimumHardwareSpec(), getRecommendedHardwareSpec())._compatibility) {
                case HardwareSpec::SystemCompatibility::Incompatible:
                case HardwareSpec::SystemCompatibility::Minimum:
                    _icon = StyledIcon("file-circle-exclamation");
                    break;

                case HardwareSpec::SystemCompatibility::Compatible:
                    _icon = StyledIcon("file-circle-check");
                    break;

                case HardwareSpec::SystemCompatibility::Unknown:
                    _icon = StyledIcon("file-circle-question");
                    break;
            }
        }
        else {
            _icon = StyledIcon("download");
        }
    }

    emit iconChanged(_icon);
}

void ProjectsModelProject::updateTooltip()
{
    if (isGroup()) {
        _tooltip =  isExpanded() ? "<b>Click to close the group</b>" : "<b>Click to open the group</b>";
    }
    else {
        if (isDownloaded()) {
            const auto systemCompatibility = HardwareSpec::getSystemCompatibility(getMinimumHardwareSpec(), getRecommendedHardwareSpec());

            switch (systemCompatibility._compatibility) {
                case HardwareSpec::SystemCompatibility::Incompatible:
                case HardwareSpec::SystemCompatibility::Minimum:
                case HardwareSpec::SystemCompatibility::Compatible:
                case HardwareSpec::SystemCompatibility::Unknown:
                {
                    _tooltip = QString("<p><b>Click to open the project</b></p><p>%1</p>").arg(systemCompatibility._message);
                    break;
                }

            }
        }
        else {
            _tooltip = "<b>Click to download and open the project</b>";
        }
    }

    emit tooltipChanged(_tooltip);
}

QString ProjectsModelProject::getDownloadedProjectFilePath() const
{
    return mv::projects().getDownloadedProjectsDir().filePath(getUrl().fileName());
}

std::uint64_t ProjectsModelProject::getDownloadedProjectFileSize() const
{
    if (const auto downloadedProjectFilePath = getDownloadedProjectFilePath(); !downloadedProjectFilePath.isEmpty()) {
        QFileInfo fileInfo(downloadedProjectFilePath);

        if (fileInfo.exists() && fileInfo.isFile())
            return fileInfo.size();
    }

    return 0;
}

QDateTime ProjectsModelProject::getDownloadedProjectLastModified() const
{
    return QFileInfo(getDownloadedProjectFilePath()).lastModified();
}

}
