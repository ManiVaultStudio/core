// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"

#include "actions/TriggerAction.h"

#include "util/LearningCenterVideo.h"
#include "util/LearningCenterTutorial.h"
#include "util/Notification.h"

#include "models/LearningCenterVideosModel.h"
#include "models/LearningCenterTutorialsModel.h"

#include <QObject>

namespace mv {

/**
 * Abstract help manager class
 *
 * Base abstract help manager class
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractHelpManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractHelpManager(QObject* parent) :
        AbstractManager(parent, "Help")
    {
    }

    /**
     * Add \p video
     * @param video Pointer to video to add
     */
    virtual void addVideo(const util::LearningCenterVideo* video) = 0;

    /**
     * Get videos for \p tags
     * @param tags Filter tags (returns all videos if empty)
     * @return Vector of videos
     */
    virtual util::LearningCenterVideos getVideos(const QStringList& tags) const = 0;

    /**
     * Get videos model
     * @return Const reference to videos model
     */
    virtual const LearningCenterVideosModel& getVideosModel() const = 0;

    /**
     * Get videos menu (e.g. for use in the main menu)
     * @return Pointer to videos menu
     */
    virtual QMenu* getVideosMenu() const = 0;

    /**
     * Add \p tutorial
     * @param tutorial Pointer to tutorial to add
     */
    virtual void addTutorial(const util::LearningCenterTutorial* tutorial) = 0;

    /**
     * Get tutorials for \p tags
     * @param includeTags Filter tags (returns all tutorials if empty)
     * @param excludeTags Filter exclude tags
     * @return Vector of tutorials
     */
    virtual util::LearningCenterTutorials getTutorials(const QStringList& includeTags, const QStringList& excludeTags = QStringList()) const = 0;

    /**
     * Get tutorials model
     * @return Const reference to tutorials model
     */
    virtual const LearningCenterTutorialsModel& getTutorialsModel() const = 0;

    /**
     * Get tutorials menu (e.g. for use in the main menu)
     * @return Pointer to tutorials menu
     */
    virtual QMenu* getTutorialsMenu() const = 0;

    /**
     * Add toaster notification in the main window
     * @param title Message title (maybe HTML)
     * @param description Message description (maybe HTML)
     * @param icon Icon to display in the notification
     * @param durationType Duration type of the notification
     * @param delayMs Delay in milliseconds before the notification is shown
     */
    virtual void addNotification(const QString& title, const QString& description, const QIcon& icon = QIcon(), const util::Notification::DurationType& durationType = util::Notification::DurationType::Calculated, std::int32_t delayMs = 0) = 0;

    /**
     * Add toaster notification for \p task in the main window
     * @param task Task containing the notification details
     */
    virtual void addNotification(const Task& task) = 0;

    /**
     * Initialize notifications manager with \p parentWidget widget
     * @param parentWidget Pointer to parent widget
     */
    virtual void initializeNotifications(QWidget* parentWidget) = 0;

signals:

    /** Invoked when the videos model has been successfully populated from the website */
    void videosModelPopulatedFromWebsite();

public: // Action getters

    virtual gui::ToggleAction& getShowLearningCenterPageAction() = 0;
    virtual gui::TriggerAction& getToDiscordAction() = 0;
    virtual gui::TriggerAction& getToWebsiteAction() = 0;
    virtual gui::TriggerAction& getToWikiAction() = 0;
    virtual gui::TriggerAction& getToRepositoryAction() = 0;
    virtual gui::TriggerAction& getToLearningCenterAction() = 0;
};

}
