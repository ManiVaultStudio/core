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
    _iconAction(this, "Icon"),
    _versionAction(this, "Version"),
    _popupGroupAction(this, "Popup Group"),
    _popupAction(this, "Info")
{
    setBarAction(&_barGroupAction);
    setPopupAction(&_popupGroupAction);

    _barGroupAction.setShowLabels(false);

    _barGroupAction.addAction(&_iconAction);
    _barGroupAction.addAction(&_versionAction);

    _iconAction.setEnabled(false);
    _iconAction.setDefaultWidgetFlags(StringAction::Label);
    _iconAction.setString(Application::getIconFont("FontAwesome").getIconCharacter("home"));
    _iconAction.setWidgetConfigurationFunction([](WidgetAction* action, QWidget* widget) -> void {
        auto labelWidget = widget->findChild<QLabel*>("Label");

        Q_ASSERT(labelWidget != nullptr);

        if (labelWidget == nullptr)
            return;

        labelWidget->setFont(Application::getIconFont("FontAwesome").getFont());
    });

    _versionAction.setEnabled(false);
    _versionAction.setDefaultWidgetFlags(StringAction::Label);
    _versionAction.setString(QString("ManiVault <b>%1.%2%3</b>").arg(QString::number(MV_VERSION_MAJOR), QString::number(MV_VERSION_MINOR), "rc"));

    _popupAction.setDefaultWidgetFlags(StringAction::Label);
    _popupAction.setPopupSizeHint(QSize(200, 0));
    _popupAction.setString(Application::getAbout());
    _popupAction.setToolTip(_versionAction.getString());

    _popupGroupAction.setShowLabels(false);
    _popupGroupAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoGroupBoxInPopupLayout);
    _popupGroupAction.addAction(&_popupAction);
}
