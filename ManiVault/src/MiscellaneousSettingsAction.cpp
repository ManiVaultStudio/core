
// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "MiscellaneousSettingsAction.h"
#include "actions/StatusBarAction.h"

namespace mv::gui
{

MiscellaneousSettingsAction::MiscellaneousSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Miscellaneous"),
    _ignoreLoadingErrorsAction(this, "Ignore loading errors", true),
    _askConfirmationBeforeRemovingDatasetsAction(this, "Ask confirmation before removing datasets", true),
    _keepDescendantsAfterRemovalAction(this, "Keep descendants after removal", true),
    _showSimplifiedGuidsAction(this, "Show simplified GUID's", true),
    _showStatusBarAction(this, "Show status bar", true),
    _statusBarOptionsAction(this, "Status bar options"),
    _statusBarSettingsGroupAction(this, "Status bar settings group")
{
    setShowLabels(false);

    updateStatusBarOptionsAction();

    _showStatusBarAction.setSettingsPrefix(QString("%1StatusBar/Show").arg(getSettingsPrefix()));
    _statusBarOptionsAction.setSettingsPrefix(QString("%1StatusBar/Options").arg(getSettingsPrefix()));

    _statusBarOptionsAction.setDefaultWidgetFlag(OptionsAction::WidgetFlag::Selection);

    _statusBarSettingsGroupAction.setShowLabels(false);

    _statusBarSettingsGroupAction.addAction(&_showStatusBarAction);
    _statusBarSettingsGroupAction.addAction(&_statusBarOptionsAction);

    _askConfirmationBeforeRemovingDatasetsAction.setToolTip("Ask confirmation prior to removal of datasets");
    _keepDescendantsAfterRemovalAction.setToolTip("If checked, descendants will not be removed and become orphans (placed at the root of the hierarchy)");
    _showSimplifiedGuidsAction.setToolTip("If checked, views will show a truncated version of a globally unique identifier");

    addAction(&_ignoreLoadingErrorsAction);
    addAction(&_askConfirmationBeforeRemovingDatasetsAction);
    addAction(&_keepDescendantsAfterRemovalAction);
    addAction(&_statusBarSettingsGroupAction);
}

void MiscellaneousSettingsAction::updateStatusBarOptionsAction()
{
    QStringList statusBarOptions, selectedStatusBarOptions;

    for (const auto statusBarAction : StatusBarAction::getStatusBarActions()) {
        statusBarOptions << statusBarAction->text();

        if (statusBarAction->isVisible())
            selectedStatusBarOptions << statusBarAction->text();
    }

    _statusBarOptionsAction.setOptions(statusBarOptions);
    _statusBarOptionsAction.setSelectedOptions(selectedStatusBarOptions);
}
}
