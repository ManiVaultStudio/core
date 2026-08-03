// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <AbstractHelpManager.h>

#include <util/FileDownloader.h>
#include <util/Notification.h>
#include <util/Notifications.h>

#include <models/TasksListModel.h>
#include <models/TasksFilterModel.h>

namespace mv
{

class HelpManager : public mv::AbstractHelpManager
{
    Q_OBJECT

public:

    /**
     * @brief Constructs the application help manager.
     * @param parent Optional parent object.
     */
    HelpManager(QObject* parent);

    /** Resets the manager when destructed. */
    ~HelpManager() override;

    /** Performs help manager startup initialization. */
    void initialize() override;

    /** Resets the contents of the help manager. */
    void reset() override;

    /**
     * @brief Adds a learning center video.
     * @param video Video to add.
     */
    void addVideo(const util::LearningCenterVideo* video) override;

    /**
     * @brief Returns videos matching the requested tags.
     * @param tags Filter tags. Empty returns all videos.
     * @return Matching videos.
     */
    util::LearningCenterVideos getVideos(const QStringList& tags) const override;

    /**
     * @brief Returns the videos menu.
     * @return Videos menu.
     */
    QMenu* getVideosMenu() const override;

    /**
     * @brief Returns the videos model.
     * @return Videos model.
     */
    const LearningCenterVideosModel& getVideosModel() const override;

    /**
     * @brief Adds a learning center tutorial.
     * @param tutorial Tutorial to add.
     */
    void addTutorial(const util::LearningCenterTutorial* tutorial) override;

    /**
     * @brief Returns tutorials matching the requested tags.
     * @param includeTags Required tags. Empty returns all tutorials.
     * @param excludeTags Tags to exclude.
     * @return Matching tutorials.
     */
    util::LearningCenterTutorials getTutorials(const QStringList& includeTags, const QStringList& excludeTags = QStringList()) const override;

    /**
     * @brief Returns the tutorials model.
     * @return Tutorials model.
     */
    const LearningCenterTutorialsModel& getTutorialsModel() const override;

    /**
     * @brief Returns the tutorials menu.
     * @return Tutorials menu.
     */
    QMenu* getTutorialsMenu() const override;

    /**
     * @brief Adds a toast notification to the main window.
     * @param title Notification title, optionally rich text.
     * @param description Notification description, optionally rich text.
     * @param icon Notification icon.
     * @param durationType Notification duration policy.
     * @param delayMs Delay in milliseconds before showing the notification.
     */
    void addNotification(const QString& title, const QString& description, const QIcon& icon = QIcon(), const util::Notification::DurationType& durationType = util::Notification::DurationType::Calculated, std::int32_t delayMs = 0) override;

    /**
     * @brief Registers a notification link handler.
     * @param route Route to handle, for example "app://open/errors".
     * @param handler Handler invoked when a matching notification link is clicked.
     */
    void addNotificationLinkHandler(const QString& route, const NotificationLinkHandler& handler) override;

    /**
     * @brief Handles a clicked notification link.
     * @param url Activated link URL.
     */
    void handleNotificationLink(const QUrl& url) override;

    /**
     * @brief Initializes the notifications manager.
     * @param parentWidget Parent widget used as the notification anchor.
     */
    void initializeNotifications(QWidget* parentWidget) override;

protected:

    /**
     * @brief Adds a task notification to the main window.
     * @param task Task whose state is displayed by the notification.
     */
    void addNotification(QPointer<Task> task) override;

public: // Action getters

    gui::ToggleAction& getShowLearningCenterPageAction() override { return _showLearningCenterPageAction; }
    gui::TriggerAction& getToDiscordAction() override { return _toDiscordAction; }
    gui::TriggerAction& getToWebsiteAction() override { return _toWebsiteAction; }
    gui::TriggerAction& getToWikiAction() override { return _toWikiAction; }
    gui::TriggerAction& getToRepositoryAction() override { return _toRepositoryAction; }
    gui::TriggerAction& getToLearningCenterAction() override { return _toLearningCenterAction; }

private:

    gui::ToggleAction                       _showLearningCenterPageAction;  /**< Toggles the learning center page */
    gui::TriggerAction                      _toDiscordAction;               /**< Opens the Discord link */
    gui::TriggerAction                      _toWebsiteAction;               /**< Opens the website link */
    gui::TriggerAction                      _toWikiAction;                  /**< Opens the wiki link */
    gui::TriggerAction                      _toRepositoryAction;            /**< Opens the repository link */
    gui::TriggerAction                      _toLearningCenterAction;        /**< Opens the learning center */
    LearningCenterVideosModel               _videosModel;                   /**< Videos model */
    LearningCenterTutorialsModel            _tutorialsModel;                /**< Tutorials model */
    util::Notifications                     _notifications;                 /**< Notifications manager */
    mv::TasksListModel                      _tasksModel;                    /**< Tasks list model */
    mv::TasksFilterModel                    _tasksFilterModel;              /**< Filter model for tasks */
    QHash<QString, NotificationLinkHandler> _linkHandlers;                  /**< Notification link handlers indexed by route */
};

}
