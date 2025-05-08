// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <AbstractHelpManager.h>

#include <util/FileDownloader.h>

#include <util/Notification.h>

namespace mv
{

class HelpManager : public mv::AbstractHelpManager
{
    Q_OBJECT

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    HelpManager(QObject* parent);

    /** Reset when destructed */
    ~HelpManager() override;

    /** Perform event manager startup initialization */
    void initialize() override;

    /** Resets the contents of the event manager */
    void reset() override;

    /**
     * Add \p video
     * @param video Pointer to video to add
     */
    void addVideo(const util::LearningCenterVideo* video) override;

    /**
     * Get videos for \p tags
     * @param tags Filter tags (returns all videos if empty)
     * @return Vector of videos
     */
    util::LearningCenterVideos getVideos(const QStringList& tags) const override;

    /**
     * Get videos menu (e.g. for use in the main menu)
     * @return Pointer to videos menu
     */
    QMenu* getVideosMenu() const override;

    /**
     * Get videos model
     * @return Const reference to videos model
     */
    const LearningCenterVideosModel& getVideosModel() const override;

    /**
     * Add \p tutorial
     * @param tutorial Pointer to tutorial to add
     */
    void addTutorial(const util::LearningCenterTutorial* tutorial) override;

    /**
     * Get tutorials for \p tags
     * @param includeTags Filter tags (returns all tutorials if empty)
     * @param excludeTags Filter exclude tags
     * @return Vector of tutorials
     */
    util::LearningCenterTutorials getTutorials(const QStringList& includeTags, const QStringList& excludeTags = QStringList()) const override;

    /**
     * Get tutorials model
     * @return Const reference to tutorials model
     */
    const LearningCenterTutorialsModel& getTutorialsModel() const override;

    /**
     * Get tutorials menu (e.g. for use in the main menu)
     * @return Pointer to tutorials menu
     */
    QMenu* getTutorialsMenu() const override;

    /**
     * Add toaster notification in the main window
     * @param title Message title (maybe HTML)
     * @param description Message description (maybe HTML)
     * @param icon Icon to display in the notification
     * @param durationType Duration type of the notification
     * @param delayMs Delay in milliseconds before the notification is shown
     */
    void addNotification(const QString& title, const QString& description, const QIcon& icon = QIcon(), const util::Notification::DurationType& durationType = util::Notification::DurationType::Calculated, std::int32_t delayMs = 0) override;

    /**
     * Initialize notifications manager with \p parentWidget widget
     * @param parentWidget Pointer to parent widget
     */
    void initializeNotifications(QWidget* parentWidget) override;

public: // Action getters

    gui::ToggleAction& getShowLearningCenterPageAction() override { return _showLearningCenterPageAction; }
    gui::TriggerAction& getToDiscordAction() override { return _toDiscordAction; }
    gui::TriggerAction& getToWebsiteAction() override { return _toWebsiteAction; }
    gui::TriggerAction& getToWikiAction() override { return _toWikiAction; }
    gui::TriggerAction& getToRepositoryAction() override { return _toRepositoryAction; }
    gui::TriggerAction& getToLearningCenterAction() override { return _toLearningCenterAction; }

private:
    gui::ToggleAction       		_showLearningCenterPageAction;  /** Toggle action for toggling the learning center */
    gui::TriggerAction      		_toDiscordAction;               /** External link to discord */
    gui::TriggerAction      		_toWebsiteAction;               /** External link to website */
    gui::TriggerAction      		_toWikiAction;                  /** External link to wiki */
    gui::TriggerAction      		_toRepositoryAction;            /** External link to repository */
    gui::TriggerAction      		_toLearningCenterAction;        /** Trigger action to go the learning center */
    LearningCenterVideosModel  		_videosModel;                   /** Videos model */
	LearningCenterTutorialsModel    _tutorialsModel;                /** Tutorials model */
    util::Notifications     		_notifications;                 /** Notifications manager */
};

}
