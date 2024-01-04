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
    _askConfirmationBeforeRemovingDatasetsAction(this, "Ask confirmation before removing datasets", true),
    _keepDescendantsAfterRemovalAction(this, "Keep descendants after removal", true)
{
    setShowLabels(false);

    addAction(&_ignoreLoadingErrorsAction);
    addAction(&_askConfirmationBeforeRemovingDatasetsAction);

    _askConfirmationBeforeRemovingDatasetsAction.setToolTip("Ask confirmation prior to removal of datasets");
    _keepDescendantsAfterRemovalAction.setToolTip("If checked, descendants will not be removed and become orphans (placed at the root of the hierarchy)");

    _ignoreLoadingErrorsAction.setSettingsPrefix(getSettingsPrefix() + "IgnoreLoadingErrors");
    _askConfirmationBeforeRemovingDatasetsAction.setSettingsPrefix(getSettingsPrefix() + "AskConfirmationBeforeRemovingDatasets");
}

}
