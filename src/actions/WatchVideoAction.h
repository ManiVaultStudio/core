// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "TriggerAction.h"

namespace mv::util {
    class LearningCenterVideo;
}

namespace mv::gui {

/**
 * Watch video action class
 *
 * Watch video trigger action class
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT WatchVideoAction : public TriggerAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object, \p title and pointer to \p video
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param video Pointer to video
     */
    Q_INVOKABLE WatchVideoAction(QObject* parent, const QString& title, const util::LearningCenterVideo* video = nullptr);

private:

    /** Opens up a browser with YouTube or a dialog with a GIF animation */
    void watch() const;

private:
    const util::LearningCenterVideo*    _video;    /** Pointer to video to watch */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::WatchVideoAction)

inline const auto watchVideoActionMetaTypeId = qRegisterMetaType<mv::gui::WatchVideoAction*>("mv::gui::WatchVideoAction");
