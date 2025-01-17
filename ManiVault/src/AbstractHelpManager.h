// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"

#include "actions/TriggerAction.h"

#include "util/Video.h"

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
     * Get videos for \p tags
     * @param tags Filter tags (returns all videos if empty)
     * @return Vector of videos
     */
    virtual util::Videos getVideos(const QStringList& tags) const = 0;

    /**
     * Add toaster notification in the main window
     * @param title Message title (maybe HTML)
     * @param description Message description (maybe HTML)
     */
    virtual void addNotification(const QString& title, const QString& description) = 0;

    /**
     * Initialize notifications manager with \p parentWidget widget
     * @param parentWidget Pointer to parent widget
     */
    virtual void initializeNotifications(QWidget* parentWidget) = 0;

public: // Action getters

    virtual gui::ToggleAction& getShowLearningCenterAction() = 0;
    virtual gui::TriggerAction& getToDiscordAction() = 0;
    virtual gui::TriggerAction& getToWebsiteAction() = 0;
    virtual gui::TriggerAction& getToWikiAction() = 0;
    virtual gui::TriggerAction& getToRepositoryAction() = 0;
    virtual gui::TriggerAction& getToLearningCenterAction() = 0;
};

}
