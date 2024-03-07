// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "MiscellaneousSettingsAction.h"
#include "Application.h"

namespace mv::gui
{

MiscellaneousSettingsAction::MiscellaneousSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Miscellaneous"),
    _ignoreLoadingErrorsAction(this, "Ignore loading errors", true),
    _askConfirmationBeforeRemovingDatasetsAction(this, "Ask confirmation before removing datasets", true),
    _keepDescendantsAfterRemovalAction(this, "Keep descendants after removal", true),
    _showSimplifiedGuidsAction(this, "Show simplified GUID's", true)
{
    setShowLabels(false);

    addAction(&_ignoreLoadingErrorsAction);
    addAction(&_askConfirmationBeforeRemovingDatasetsAction);
    addAction(&_keepDescendantsAfterRemovalAction);
    addAction(&_showSimplifiedGuidsAction);

    _askConfirmationBeforeRemovingDatasetsAction.setToolTip("Ask confirmation prior to removal of datasets");
    _keepDescendantsAfterRemovalAction.setToolTip("If checked, descendants will not be removed and become orphans (placed at the root of the hierarchy)");
    _showSimplifiedGuidsAction.setToolTip("If checked, views will show a truncated version of a globally unique identifier");
}

}
