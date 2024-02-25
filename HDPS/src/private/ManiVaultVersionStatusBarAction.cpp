// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ManiVaultVersionStatusBarAction.h"
#include "ManiVaultVersion.h"

#ifdef _DEBUG
    #define MANIVAULT_VERSION_STATUS_BAR_ACTION_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;

ManiVaultVersionStatusBarAction::ManiVaultVersionStatusBarAction(QObject* parent, const QString& title) :
    StatusBarAction(parent, title),
    _barGroupAction(this, "Bar Group"),
    _barAction(this, "ManiVault Version"),
    _popupGroupAction(this, "Popup Group"),
    _popupAction(this, "ManiVault Info")
{
    setBarAction(&_barGroupAction);
    setPopupAction(&_popupGroupAction);

    _barGroupAction.setShowLabels(false);
    _barGroupAction.addAction(&_barAction);

    _barAction.setEnabled(false);
    _barAction.setDefaultWidgetFlags(StringAction::Label);
    _barAction.setString(QString("ManiVault <b>v%1.%2</b>").arg(QString::number(MV_VERSION_MAJOR), QString::number(MV_VERSION_MINOR)));

    _popupAction.setDefaultWidgetFlags(StringAction::Label);
    _popupAction.setPopupSizeHint(QSize(200, 0));
    _popupAction.setString(Application::getAbout());
    _popupAction.setToolTip(_barAction.getString());

    _popupGroupAction.setShowLabels(false);
    _popupGroupAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoGroupBoxInPopupLayout);
    _popupGroupAction.addAction(&_popupAction);
}
