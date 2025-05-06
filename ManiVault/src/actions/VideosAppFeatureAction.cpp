// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "VideosAppFeatureAction.h"

namespace mv::gui {

VideosAppFeatureAction::VideosAppFeatureAction(QObject* parent, const QString& title /*= "Videos"*/) :
    AppFeatureAction(parent, title)
{
    loadDescriptionFromResource(":/HTML/AppFeatureVideos");

    getSummaryAction().setString("Allow ManiVault Studio to download data for learning center videos.");
}

}
