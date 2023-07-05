// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "MiscellaneousSettingsAction.h"
#include "Application.h"

namespace hdps
{

MiscellaneousSettingsAction::MiscellaneousSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Miscellaneous"),
    _ignoreLoadingErrorsAction(this, "Ignore loading errors")
{
    setShowLabels(false);

    addAction(&_ignoreLoadingErrorsAction);

    _ignoreLoadingErrorsAction.setSettingsPrefix(getSettingsPrefix() + "IgnoreLoadingErrors");
}

}
