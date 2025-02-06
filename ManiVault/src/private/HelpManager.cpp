// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HelpManager.h"

#include <models/LearningCenterVideosFilterModel.h>
#include <models/LearningCenterTutorialsFilterModel.h>

#include <util/Exception.h>

#include <actions/WatchVideoAction.h>

#include <Application.h>
#include <Task.h>

#include <QDesktopServices>
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
    _showLearningCenterPageAction(this, "Learning center"),
    _toDiscordAction(this, "Join us..."),
    _toWebsiteAction(this, "Website"),
    _toWikiAction(this, "Wiki"),
    _toRepositoryAction(this, "Repository"),
    _toLearningCenterAction(this, "Go to learning center"),
    _fileDownloader(FileDownloader::StorageMode::File, Task::GuiScope::Background)
{
    _showLearningCenterPageAction.setIconByName("chalkboard-teacher");
    _showLearningCenterPageAction.setToolTip("Go to the learning center");

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
        _showLearningCenterPageAction.setChecked(true);
    });

    connect(&_fileDownloader, &FileDownloader::downloaded, this, [this]() -> void {
        try
        {
            const auto jsonDocument = QJsonDocument::fromJson(_fileDownloader.downloadedData());
            const auto videos       = jsonDocument.object()["videos"].toArray();
            const auto tutorials    = jsonDocument.object()["tutorials"].toArray();

            for (const auto video : videos) {
                auto videoMap = video.toVariant().toMap();

                addVideo(new LearningCenterVideo(LearningCenterVideo::Type::YouTube, videoMap["title"].toString(), videoMap["tags"].toStringList(), videoMap["date"].toString().chopped(15), videoMap["summary"].toString(), videoMap["youtube-id"].toString()));
            }

            emit videosModelPopulatedFromWebsite();

            for (const auto tutorial : tutorials)
                addTutorial(new LearningCenterTutorial(tutorial.toVariant().toMap()));

            emit tutorialsModelPopulatedFromWebsite();
        }
        catch (std::exception& e)
        {
            exceptionMessageBox("Unable to process learning center JSON", e);
        }
        catch (...)
        {
            exceptionMessageBox("Unable to process learning center JSON");
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

void HelpManager::addVideo(const LearningCenterVideo* video)
{
    _videosModel.addVideo(video);
}

LearningCenterVideos HelpManager::getVideos(const QStringList& tags) const
{
    LearningCenterVideosFilterModel videosFilterModel;

    auto videosModel = &const_cast<HelpManager*>(this)->_videosModel;

    videosFilterModel.setSourceModel(videosModel);
    videosFilterModel.getTagsFilterAction().initialize(tags, tags);

    LearningCenterVideos videos;

    for (int rowIndex = 0; rowIndex < videosFilterModel.rowCount(); rowIndex++) {
        const auto videoIndex   = videosFilterModel.mapToSource(videosFilterModel.index(rowIndex, 0));
        const auto videoItem    = dynamic_cast<LearningCenterVideosModel::Item*>(videosModel->itemFromIndex(videoIndex));

        videos.push_back(videoItem->getVideo());
    }

    return videos;
}

void HelpManager::addNotification(const QString& title, const QString& description, const QIcon& icon /*= QIcon()*/, const util::Notification::DurationType& durationType /*= util::Notification::DurationType::Calculated*/, std::int32_t delayMs /*= 0*/)
{
    _notifications.showMessage(title, description, icon, durationType, delayMs);
}

void HelpManager::initializeNotifications(QWidget* parentWidget)
{
    _notifications.setParentWidget(parentWidget);
}

QMenu* HelpManager::getVideosMenu() const
{
    auto videosMenu = new QMenu("Videos");

    videosMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("video"));

    for (const auto video : getVideos({})) {
        auto videoAction = new WatchVideoAction(videosMenu, video->getTitle(), video);

        videoAction->setIconByName("external-link-square-alt");

        videosMenu->addAction(videoAction);
    }

    return videosMenu;
}

const LearningCenterVideosModel& HelpManager::getVideosModel() const
{
    return _videosModel;
}

void HelpManager::addTutorial(const util::LearningCenterTutorial* tutorial)
{
    _tutorialsModel.addTutorial(tutorial);
}

LearningCenterTutorials HelpManager::getTutorials(const QStringList& includeTags, const QStringList& excludeTags /*= QStringList()*/) const
{
    LearningCenterTutorialsFilterModel tutorialsFilterModel;

    auto tutorialsModel = &const_cast<HelpManager*>(this)->_tutorialsModel;

    tutorialsFilterModel.setSourceModel(tutorialsModel);
    tutorialsFilterModel.getTagsFilterAction().initialize(includeTags, includeTags);
    tutorialsFilterModel.getExcludeTagsFilterAction().initialize(excludeTags, excludeTags);

    LearningCenterTutorials tutorials;

    for (int rowIndex = 0; rowIndex < tutorialsFilterModel.rowCount(); rowIndex++) {
        const auto tutorialIndex    = tutorialsFilterModel.mapToSource(tutorialsFilterModel.index(rowIndex, 0));
        const auto itemIndex        = tutorialsModel->itemFromIndex(tutorialIndex);
        const auto tutorialItem     = dynamic_cast<LearningCenterTutorialsModel::Item*>(itemIndex);

        tutorials.push_back(tutorialItem->getTutorial());
    }

    return tutorials;
}

const LearningCenterTutorialsModel& HelpManager::getTutorialsModel() const
{
    return _tutorialsModel;
}

QMenu* HelpManager::getTutorialsMenu() const
{
    auto tutorialsMenu = new QMenu("Tutorials");

    tutorialsMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("user-graduate"));

    for (const auto tutorial : getTutorials({}, { "Installation" })) {
        
        auto tutorialAction = new TriggerAction(tutorialsMenu, tutorial->getTitle());

        tutorialAction->setIconByName(tutorial->getIconName());

        if (tutorial->hasProject()) {
            connect(tutorialAction, &TriggerAction::triggered, tutorialAction, [tutorial]() -> void {
                mv::projects().openProject(tutorial->getProjectUrl());
            });
        } else {
            connect(tutorialAction, &TriggerAction::triggered, tutorialAction, [tutorial]() -> void {
                if (!mv::projects().hasProject())
                    mv::projects().newBlankProject();

                if (auto tutorialPlugin = mv::plugins().requestViewPlugin("Tutorial")) {
                    if (auto pickerAction = dynamic_cast<OptionAction*>(tutorialPlugin->findChildByPath("Pick tutorial")))
                        pickerAction->setCurrentText(tutorial->getTitle());

                    if (auto toolbarAction = dynamic_cast<HorizontalGroupAction*>(tutorialPlugin->findChildByPath("Toolbar")))
                        toolbarAction->setVisible(false);
                }
			});
        }

        tutorialsMenu->addAction(tutorialAction);
    }

    return tutorialsMenu;
}

}
