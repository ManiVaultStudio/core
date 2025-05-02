// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AppFeatureAction.h"

namespace mv::gui {

AppFeatureAction::AppFeatureAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title)
{
}

}
