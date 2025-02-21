// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ManiVaultVersionStatusBarAction.h"

#include <Application.h>
#include <ManiVaultVersion.h>

#ifdef _DEBUG
    #define MANIVAULT_VERSION_STATUS_BAR_ACTION_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;

ManiVaultVersionStatusBarAction::ManiVaultVersionStatusBarAction(QObject* parent, const QString& title) :
    StatusBarAction(parent, title, "house"),
    _versionAction(this, "Version"),
    _popupGroupAction(this, "Popup Group"),
    _popupAction(this, "Info")
{
    setPopupAction(&_popupGroupAction);
    setToolTip("Show ManiVault info");

    getBarGroupAction().addAction(&_versionAction);

    _versionAction.setEnabled(false);
    _versionAction.setDefaultWidgetFlags(StringAction::Label);
    _versionAction.setString(QString("ManiVault <b>%1.%2%3</b>").arg(QString::number(MV_VERSION_MAJOR), QString::number(MV_VERSION_MINOR), QString::fromLocal8Bit(MV_VERSION_SUFFIX)));
    _versionAction.setToolTip(toolTip());

    _popupAction.setDefaultWidgetFlags(StringAction::Label);
    _popupAction.setPopupSizeHint(QSize(200, 0));
    _popupAction.setString(Application::getAbout());
    _popupAction.setToolTip(_versionAction.getString());
    _popupAction.setWidgetConfigurationFunction([](WidgetAction* action, QWidget* widget) -> void {
        auto label = widget->findChild<QLabel*>("Label");

        Q_ASSERT(label != nullptr);

        if (label == nullptr)
            return;

        label->setOpenExternalLinks(true);
    });

    _popupGroupAction.setShowLabels(false);
    _popupGroupAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoGroupBoxInPopupLayout);
    _popupGroupAction.addAction(&_popupAction);
}
