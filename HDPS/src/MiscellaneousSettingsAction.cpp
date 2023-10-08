// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "MiscellaneousSettingsAction.h"
#include "Application.h"

namespace hdps
{

MiscellaneousSettingsAction::MiscellaneousSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Miscellaneous"),
    _ignoreLoadingErrorsAction(this, "Ignore loading errors", true, true)
{
    setShowLabels(false);

    addAction(&_ignoreLoadingErrorsAction);

    _ignoreLoadingErrorsAction.setSettingsPrefix(getSettingsPrefix() + "IgnoreLoadingErrors");
}

}
