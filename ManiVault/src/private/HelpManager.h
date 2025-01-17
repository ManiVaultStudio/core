// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractHelpManager.h"
#include "HelpManagerVideosModel.h"

#include <util/Notification.h>

namespace mv
{

class HelpManager final : public mv::AbstractHelpManager
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
     * Get videos for \p tags
     * @param tags Filter tags (returns all videos if empty)
     * @return Vector of videos
     */
    util::Videos getVideos(const QStringList& tags) const override;

    /**
     * Add toaster notification in the main window
     * @param title Message title (maybe HTML)
     * @param description Message description (maybe HTML)
     */
    void addNotification(const QString& title, const QString& description) override;

    /**
     * Initialize notifications manager with \p parentWidget widget
     * @param parentWidget Pointer to parent widget
     */
    void initializeNotifications(QWidget* parentWidget) override;

public: // Action getters

    gui::ToggleAction& getShowLearningCenterAction() override { return _showLearningCenterAction; }
    gui::TriggerAction& getToDiscordAction() override { return _toDiscordAction; }
    gui::TriggerAction& getToWebsiteAction() override { return _toWebsiteAction; }
    gui::TriggerAction& getToWikiAction() override { return _toWikiAction; }
    gui::TriggerAction& getToRepositoryAction() override { return _toRepositoryAction; }
    gui::TriggerAction& getToLearningCenterAction() override { return _toLearningCenterAction; }

private:
    gui::ToggleAction       _showLearningCenterAction;      /** Toggle action for toggling the learning center */
    gui::TriggerAction      _toDiscordAction;               /** External link to discord */
    gui::TriggerAction      _toWebsiteAction;               /** External link to website */
    gui::TriggerAction      _toWikiAction;                  /** External link to wiki */
    gui::TriggerAction      _toRepositoryAction;            /** External link to repository */
    gui::TriggerAction      _toLearningCenterAction;        /** Trigger action to go the learning center */
    HelpManagerVideosModel  _videosModel;                   /** Videos model */
    util::Notifications     _notifications;                 /** Notifications manager */
};

}