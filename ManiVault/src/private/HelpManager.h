// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractHelpManager.h"

namespace mv
{

class HelpManager final : public AbstractHelpManager
{
    Q_OBJECT

public:

    /** Default constructor */
    HelpManager();

    /** Perform event manager startup initialization */
    void initialize() override;

    /** Resets the contents of the event manager */
    void reset() override;

public: // Action getters

    mv::gui::ToggleAction& getShowLearningCenterAction() override { return _showLearningCenterAction; };
    mv::gui::TriggerAction& getToDiscordAction() override { return _toDiscordAction; };
    mv::gui::TriggerAction& getToWebsiteAction() override { return _toWebsiteAction; };
    mv::gui::TriggerAction& getToWikiAction() override { return _toWikiAction; };
    mv::gui::TriggerAction& getToRepositoryAction() override { return _toRepositoryAction; };

private:
    mv::gui::ToggleAction      _showLearningCenterAction;   /** Toggle action for toggling the learning center */
    mv::gui::TriggerAction     _toDiscordAction;            /** External link to discord */
    mv::gui::TriggerAction     _toWebsiteAction;            /** External link to website */
    mv::gui::TriggerAction     _toWikiAction;               /** External link to wiki */
    mv::gui::TriggerAction     _toRepositoryAction;         /** External link to repository */
};

}
