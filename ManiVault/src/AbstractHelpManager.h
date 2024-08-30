// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"

#include "actions/TriggerAction.h"

#include <QObject>
#include <QMenu>

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
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractHelpManager(QObject* parent = nullptr) :
        AbstractManager(parent, "Help")
    {
    }

public: // Action getters

    virtual mv::gui::ToggleAction& getShowLearningCenterAction() = 0;
    virtual mv::gui::TriggerAction& getToDiscordAction() = 0;
    virtual mv::gui::TriggerAction& getToWebsiteAction() = 0;
    virtual mv::gui::TriggerAction& getToWikiAction() = 0;
    virtual mv::gui::TriggerAction& getToRepositoryAction() = 0;
    virtual mv::gui::TriggerAction& getToLearningCenterAction() = 0;
};

}
