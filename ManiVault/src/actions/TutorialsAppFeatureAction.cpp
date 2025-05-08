// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TutorialsAppFeatureAction.h"

namespace mv::gui {

TutorialsAppFeatureAction::TutorialsAppFeatureAction(QObject* parent, const QString& title /*= "Tutorials"*/) :
    AppFeatureAction(parent, title)
{
    loadDescriptionFromResource(":/HTML/AppFeatureTutorials");

    getSummaryAction().setString("Allow ManiVault Studio to download learning center tutorial content");
}

}
