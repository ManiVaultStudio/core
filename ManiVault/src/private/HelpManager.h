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

    mv::gui::TriggerAction& getLaunchLearningCenterAction() override { return _launchLearningCenterAction; };

private:
    mv::gui::TriggerAction      _launchLearningCenterAction;    /** Trigger action to launch the learning center */
};

}
