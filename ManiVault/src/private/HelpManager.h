// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <AbstractHelpManager.h>

#include <models/VideosModel.h>

#include <util/FileDownloader.h>

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
     * Add \p video
     * @param video Pointer to video to add
     */
    void addVideo(const mv::util::Video* video) override;

    /**
     * Get videos for \p tags
     * @param tags Filter tags (returns all videos if empty)
     * @return Vector of videos
     */
    util::Videos getVideos(const QStringList& tags) const override;

    /**
     * Get videos model
     * @return Const reference to videos model
     */
    const mv::VideosModel& getVideosModel() const override;

    /**
     * Get videos model
     * @return Const reference to videos model
     */
    const mv::VideosModel& getVideosModel() const override;

public: // Action getters

    gui::ToggleAction& getShowLearningCenterAction() override { return _showLearningCenterAction; }
    gui::TriggerAction& getToDiscordAction() override { return _toDiscordAction; }
    gui::TriggerAction& getToWebsiteAction() override { return _toWebsiteAction; }
    gui::TriggerAction& getToWikiAction() override { return _toWikiAction; }
    gui::TriggerAction& getToRepositoryAction() override { return _toRepositoryAction; }
    gui::TriggerAction& getToLearningCenterAction() override { return _toLearningCenterAction; }

private:
    mv::gui::ToggleAction       _showLearningCenterAction;  /** Toggle action for toggling the learning center */
    mv::gui::TriggerAction      _toDiscordAction;           /** External link to discord */
    mv::gui::TriggerAction      _toWebsiteAction;           /** External link to website */
    mv::gui::TriggerAction      _toWikiAction;              /** External link to wiki */
    mv::gui::TriggerAction      _toRepositoryAction;        /** External link to repository */
    mv::gui::TriggerAction      _toLearningCenterAction;    /** Trigger action to go the learning center */
    mv::VideosModel             _videosModel;               /** Videos model */
    mv::util::FileDownloader    _fileDownloader;            /** For downloading the learning center JSON file */
};

}
