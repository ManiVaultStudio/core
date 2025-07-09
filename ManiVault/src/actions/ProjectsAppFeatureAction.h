// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AppFeatureAction.h"

#include "actions/ToggleAction.h"

namespace mv::gui {

/**
 * Projects app feature action class
 *
 * App feature action class for grouping projects dynamic content settings
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ProjectsAppFeatureAction : public AppFeatureAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE ProjectsAppFeatureAction(QObject* parent, const QString& title = "Projects");

//TODO
//public: // Action getters
//
//    const ToggleAction& getTutorialsAction() const { return _tutorialsAction; }
//    const ToggleAction& getProjectsAction() const { return _projectsAction; }
//
//private:
//    ToggleAction    _tutorialsAction;   /** Toggle tutorials dynamic content */
//    ToggleAction    _projectsAction;    /** Toggle projects dynamic content */
};

}

Q_DECLARE_METATYPE(mv::gui::ProjectsAppFeatureAction)

inline const auto projectsAppFeatureActionMetaTypeId = qRegisterMetaType<mv::gui::ProjectsAppFeatureAction*>("mv::gui::ProjectsAppFeatureAction");
