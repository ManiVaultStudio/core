// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DynamicContentAppFeatureAction.h"

namespace mv::gui {

DynamicContentAppFeatureAction::DynamicContentAppFeatureAction(QObject* parent, const QString& title /*= "Dynamic Content"*/) :
    AppFeatureAction(parent, title),
    _tutorialsAction(this, "Tutorials"),
    _projectsAction(this, "Projects")
{
    addAction(&_tutorialsAction);
    addAction(&_projectsAction);
}

}
