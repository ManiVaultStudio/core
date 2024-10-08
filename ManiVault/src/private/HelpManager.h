// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractHelpManager.h"
#include "HelpManagerVideosModel.h"

class HelpManager final : public mv::AbstractHelpManager
{
    Q_OBJECT

public:

    /** Default constructor */
    HelpManager();

    /** Perform event manager startup initialization */
    void initialize() override;

    /** Resets the contents of the event manager */
    void reset() override;

    /**
     * Get videos for \p tags
     * @param tags Fitler tags (returns all videos if empty)
     * @return Vector of videos
     */
    mv::util::Videos getVideos(const QStringList& tags) const override;

public: // Action getters

    mv::gui::ToggleAction& getShowLearningCenterAction() override { return _showLearningCenterAction; }
    mv::gui::TriggerAction& getToDiscordAction() override { return _toDiscordAction; }
    mv::gui::TriggerAction& getToWebsiteAction() override { return _toWebsiteAction; }
    mv::gui::TriggerAction& getToWikiAction() override { return _toWikiAction; }
    mv::gui::TriggerAction& getToRepositoryAction() override { return _toRepositoryAction; }
    mv::gui::TriggerAction& getToLearningCenterAction() override { return _toLearningCenterAction; }

private:
    mv::gui::ToggleAction       _showLearningCenterAction;      /** Toggle action for toggling the learning center */
    mv::gui::TriggerAction      _toDiscordAction;               /** External link to discord */
    mv::gui::TriggerAction      _toWebsiteAction;               /** External link to website */
    mv::gui::TriggerAction      _toWikiAction;                  /** External link to wiki */
    mv::gui::TriggerAction      _toRepositoryAction;            /** External link to repository */
    mv::gui::TriggerAction      _toLearningCenterAction;        /** Trigger action to go the learning center */
    HelpManagerVideosModel      _videosModel;                   /** Videos model */
    
};