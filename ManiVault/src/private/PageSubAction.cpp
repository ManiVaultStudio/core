// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageSubAction.h"
#include <CoreInterface.h>

#include <util/Exception.h>

#include <QBuffer>

using namespace mv::util;
using namespace mv::gui;

PageSubAction::PageSubAction(const QIcon& icon, const TooltipCallback& tooltipCallback /*= {}*/, const ClickedCallback& clickedCallback /*= {}*/) :
    _iconLabel(new IconLabel(icon))
{
    _iconLabel->hide();

    _iconLabel->setTooltipCallback([this]() -> QString {
        return getTooltip();
    });

    _iconLabel->setClickedCallback(clickedCallback);
}

PageSubAction::~PageSubAction()
{
    if (_iconLabel && !_iconLabel->parent())
        delete _iconLabel;
}

void PageSubAction::setIcon(const QIcon& icon)
{
    _iconLabel->setIcon(icon);
}

QString PageSubAction::getTooltip() const
{
    if (_iconLabel->getTooltipCallback())
        return _iconLabel->getTooltipCallback()();

    return {};
}

void PageSubAction::setVisible(bool visible)
{
    if (!_iconLabel)
        return;
    
    _iconLabel->setVisible(visible);
}

IconLabel* PageSubAction::getIconLabel()
{
    return _iconLabel;
}

PageSubAction::ClickedCallback PageSubAction::getClickedCallback() const
{
    return _iconLabel->getClickedCallback();
}

void PageSubAction::setClickedCallback(const ClickedCallback& clickedCallback)
{
    _iconLabel->setClickedCallback(clickedCallback);
}

PageSubAction::TooltipCallback PageSubAction::getTooltipCallback() const
{
    return _iconLabel->getTooltipCallback();
}

void PageSubAction::setTooltipCallback(const TooltipCallback& tooltipCallback)
{
    _iconLabel->setTooltipCallback(tooltipCallback);
}

ProjectPurgePageSubAction::ProjectPurgePageSubAction(ProjectsModelProjectSharedPtr project) :
    PageSubAction(StyledIcon("trash")),
    _project(project)
{
    setTooltipCallback([]() -> QString {
        return "Remove downloaded project";
    });

    setClickedCallback([this]() -> void {
        
        try {
            if (!_project)
                throw BaseException("Project is null");

            const auto downloadedProjectFilePath = _project->getDownloadedProjectFilePath();

            if (downloadedProjectFilePath.isEmpty())
                throw BaseException("File path is empty");

            qDebug() << "Purging downloaded project" << downloadedProjectFilePath;

            const auto fileInfo = QFileInfo(downloadedProjectFilePath);

            if (!fileInfo.exists())
                throw BaseException("File does not exist");

            if (!QFile::remove(downloadedProjectFilePath))
                throw BaseException("Unable to remove file");

            _project->setDownloaded();

            qDebug() << "Purged downloaded project at" << downloadedProjectFilePath;

            mv::help().addNotification("Project removed", QString("%1 has been removed from the system.").arg(_project->getTitle()), StyledIcon("trash"));

            setVisible(false);
        }
        catch (const std::exception& e) {
            qWarning() << "Unable to purge downloaded project:" << e.what();

            mv::help().addNotification("Problem purging project", QString("Unable to purge the project: %1").arg(e.what()), StyledIcon("trash"));
        }
    });
}

CommentsPageSubAction::CommentsPageSubAction(const QString& comments) :
    PageSubAction(StyledIcon("scroll"))
{
    setTooltipCallback([this, comments]() -> QString {
        return comments;
    });
}

TagsPageSubAction::TagsPageSubAction(const QStringList& tags) :
    PageSubAction(mv::util::StyledIcon("tags"))
{
    setTooltipCallback([this, tags]() -> QString {
        return tags.join(", ");
    });
}

PageDownloadSubAction::PageDownloadSubAction(const QString& downloadSize) :
    PageSubAction(mv::util::StyledIcon("download"))
{
    setTooltipCallback([this, downloadSize]() -> QString {
        return QString("Requires a %1 download").arg(downloadSize);
    });
}

ProjectCompatibilityPageSubAction::ProjectCompatibilityPageSubAction(const HardwareSpec::SystemCompatibilityInfo& systemCompatibilityInfo) :
    PageSubAction({})
{
    setIcon(systemCompatibilityInfo._icon);

    switch (systemCompatibilityInfo._compatibility) {
        case HardwareSpec::SystemCompatibility::Incompatible:
        {
            setTooltipCallback([this]() -> QString {
                return "Your system does not meet the minimum requirements for this project, there might be problems with opening it, its stability and performance!";
            });

            break;
        }

        case HardwareSpec::SystemCompatibility::Minimum:
        {
            setTooltipCallback([this]() -> QString {
                return "Your system does not meet the recommended requirements for this project, the interactivity might not be optimal!";
            });

            break;
        }

        case HardwareSpec::SystemCompatibility::Compatible:
        {
            setTooltipCallback([this]() -> QString {
                return "Your system meets the minimum requirements for this project!";
            });
            
            break;
        }

        case HardwareSpec::SystemCompatibility::Unknown:
        {
            setTooltipCallback([this]() -> QString {
                return "Your system compatibility is unknown, the project does not specify hardware requirements!";
            });
            
            break;
        }
    }
}

ContributorsPageSubAction::ContributorsPageSubAction(const QStringList& contributors) :
    PageSubAction(StyledIcon("user"))
{
    setTooltipCallback([this, contributors]() -> QString {
        return QString("Contributor(s): %1").arg(contributors.join(", "));
    });
}

ProjectsJsonUrlPageSubAction::ProjectsJsonUrlPageSubAction(const QUrl& projectJsonUrl) :
    PageSubAction(StyledIcon("globe"))
{
    setTooltipCallback([this, projectJsonUrl]() -> QString {
        return QString("Projects JSON: %1").arg(projectJsonUrl.toString());
    });
}

ProjectPluginsPageSubAction::ProjectPluginsPageSubAction(const QStringList& plugins) :
    PageSubAction(StyledIcon("plug"))
{
    setTooltipCallback([this, plugins]() -> QString {
        return QString("Plugin(s): %1").arg(plugins.isEmpty() ? "none" : plugins.join(", "));
    });
}

ProjectLastUpdatedPageSubAction::ProjectLastUpdatedPageSubAction(const QDateTime& lastUpdated) :
    PageSubAction(StyledIcon("clock"))
{
    setTooltipCallback([this]() -> QString {
        return QString("Last server update: %1").arg(_projectLastUpdated.isValid() ? _projectLastUpdated.toString() : "unknown");
    });
}

void ProjectLastUpdatedPageSubAction::setProjectLastUpdated(const QDateTime& projectLastUpdated)
{
    _projectLastUpdated = projectLastUpdated;
}

ProjectPreviewPageSubAction::ProjectPreviewPageSubAction(const QImage& previewImage) :
    PageSubAction(StyledIcon("image"))
{
    setTooltipCallback([this]() -> QString {
        if (!_previewImage.isNull()) {
            QBuffer buffer;

            buffer.open(QIODevice::WriteOnly);

            QPixmap::fromImage(_previewImage).save(&buffer, "JPG");

            auto image = buffer.data().toBase64();

            return QString("<img style='padding: 100px;'src='data:image/jpg;base64,%1'></p>").arg(image);
        }

        return {};
    });
}

void ProjectPreviewPageSubAction::setPreviewImage(const QImage& previewImage)
{
    _previewImage = previewImage;
}

ProxyPageSubAction::ProxyPageSubAction(const QIcon& icon, const TooltipCallback& tooltipCallback) :
    PageSubAction(icon)
{
    setTooltipCallback(tooltipCallback);
}
