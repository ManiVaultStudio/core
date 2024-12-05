// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HelpManager.h"

#include "models/VideosFilterModel.h"

#include <Application.h>

#include <QDesktopServices>
#include <QMainWindow>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>

using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
    //#define HELP_MANAGER_VERBOSE
#endif

namespace mv
{

HelpManager::HelpManager(QObject* parent) :
    AbstractHelpManager(parent),
    _showLearningCenterAction(this, "Learning center"),
    _toDiscordAction(this, "Join us..."),
    _toWebsiteAction(this, "Website"),
    _toWikiAction(this, "Wiki"),
    _toRepositoryAction(this, "Repository"),
    _toLearningCenterAction(this, "Go to learning center")
{
    _showLearningCenterAction.setIconByName("chalkboard-teacher");
    _showLearningCenterAction.setToolTip("Go to the learning center");

    _toDiscordAction.setIcon(Application::getIconFont("FontAwesomeBrands", 5, 14).getIcon("discord"));
    _toDiscordAction.setToolTip("Get in touch on our Discord");
    _toDiscordAction.setDefaultWidgetFlags(TriggerAction::Icon);

    _toWebsiteAction.setIconByName("globe");
    _toWebsiteAction.setToolTip("Visit our website");
    _toWebsiteAction.setDefaultWidgetFlags(TriggerAction::Icon);

    _toWikiAction.setIconByName("book");
    _toWikiAction.setToolTip("Visit our wiki");
    _toWikiAction.setDefaultWidgetFlags(TriggerAction::Icon);

    _toRepositoryAction.setIcon(Application::getIconFont("FontAwesomeBrands", 6, 5).getIcon("github"));
    _toRepositoryAction.setToolTip("Contribute to ManiVault on Github");
    _toRepositoryAction.setDefaultWidgetFlags(TriggerAction::Icon);
    //_toRepositoryAction.setShortcut(QKeySequence("Ctrl+F4"));
    //_toRepositoryAction.setShortcutContext(Qt::ShortcutContext::ApplicationShortcut);

    _toLearningCenterAction.setIconByName("chalkboard-teacher");
    //_toLearningCenterAction.setShortcut(QKeySequence(tr("Ctrl+F5")));
    //_toLearningCenterAction.setShortcutContext(Qt::ShortcutContext::ApplicationShortcut);

    connect(&_toDiscordAction, &TriggerAction::triggered, this, []() -> void {
        QDesktopServices::openUrl(QUrl("https://discord.gg/pVxmC2cSzA"));
    });

    connect(&_toWebsiteAction, &TriggerAction::triggered, this, []() -> void {
        QDesktopServices::openUrl(QUrl("https://www.manivault.studio/"));
    });

    connect(&_toWikiAction, &TriggerAction::triggered, this, []() -> void {
        QDesktopServices::openUrl(QUrl("https://github.com/ManiVaultStudio/PublicWiki/wiki/"));
    });

    connect(&_toRepositoryAction, &TriggerAction::triggered, this, []() -> void {
        QDesktopServices::openUrl(QUrl("https://github.com/ManiVaultStudio"));
    });

    connect(&_toLearningCenterAction, &TriggerAction::triggered, this, [this]() -> void {
        _showLearningCenterAction.setChecked(true);
    });

    connect(&_fileDownloader, &FileDownloader::downloaded, this, [this]() -> void {
        const auto jsonDocument = QJsonDocument::fromJson(_fileDownloader.downloadedData());
        const auto videos = jsonDocument.object()["videos"].toArray();

        for (const auto& video : videos) {
            auto videoMap = video.toVariant().toMap();

            addVideo(new Video(Video::Type::YouTube, videoMap["title"].toString(), videoMap["tags"].toStringList(), videoMap["date"].toString().chopped(15), videoMap["summary"].toString(), videoMap["youtube-id"].toString()));
        }
	});
}

HelpManager::~HelpManager()
{
    reset();
}

void HelpManager::initialize()
{
#ifdef HELP_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractHelpManager::initialize();

    if (isInitialized())
        return;

    beginInitialization();
    {
        _fileDownloader.download(QUrl("https://www.manivault.studio/api/learning-center.json"));
    }
    endInitialization();
}

void HelpManager::reset()
{
#ifdef HELP_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
    }
    endReset();
}

void HelpManager::addVideo(const Video* video)
{
    _videosModel.addVideo(video);
}

Videos HelpManager::getVideos(const QStringList& tags) const
{
    VideosFilterModel videosFilterModel;

    auto videosModel = &const_cast<HelpManager*>(this)->_videosModel;

    videosFilterModel.setSourceModel(videosModel);
    videosFilterModel.getTagsFilterAction().initialize(tags, tags);

    Videos videos;

    for (int rowIndex = 0; rowIndex < videosFilterModel.rowCount(); rowIndex++) {
        const auto videoIndex = videosFilterModel.mapToSource(videosFilterModel.index(rowIndex, 0));

        //const auto videoUrlString = videoIndex.siblingAtColumn(5).data().toString();

        //if (!urlExists(videoUrlString))
        //    continue;
        
        //videos.emplace_back(dynamic_cast<VideosModel::Item*>(videosModel->itemFromIndex(videoIndex))->getVideo());
    }

    return videos;
}

const VideosModel& HelpManager::getVideosModel() const
{
    return _videosModel;
}

}
