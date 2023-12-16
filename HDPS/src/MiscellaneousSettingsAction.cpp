// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "MiscellaneousSettingsAction.h"
#include "Application.h"

namespace mv
{

MiscellaneousSettingsAction::MiscellaneousSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Miscellaneous"),
    _ignoreLoadingErrorsAction(this, "Ignore loading errors", true),
    _confirmDatasetsRemovalAction(this, "Ask for permission to remove dataset(s)", true)
{
    setShowLabels(false);

    addAction(&_ignoreLoadingErrorsAction);
    addAction(&_confirmDatasetsRemovalAction);

    _ignoreLoadingErrorsAction.setSettingsPrefix(getSettingsPrefix() + "IgnoreLoadingErrors");
    _confirmDatasetsRemovalAction.setSettingsPrefix(getSettingsPrefix() + "AskPermissionRemoval");
}

}
