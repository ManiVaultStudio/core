// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SettingsStatusBarAction.h"

#ifdef _DEBUG
#define SETTINGS_STATUS_BAR_ACTION_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;

SettingsStatusBarAction::SettingsStatusBarAction(QObject* parent, const QString& title) :
    StatusBarAction(parent, title),
    _barGroupAction(this, "Bar Group"),
    _iconAction(this, "Icon")
{
    setBarAction(&_barGroupAction);

    _barGroupAction.setShowLabels(false);

    _barGroupAction.addAction(&_iconAction);

    _iconAction.setEnabled(false);
    _iconAction.setDefaultWidgetFlags(StringAction::Label);
    _iconAction.setToolTip("Edit global settings");
    _iconAction.setString(Application::getIconFont("FontAwesome").getIconCharacter("cog"));
    _iconAction.setWidgetConfigurationFunction([](WidgetAction* action, QWidget* widget) -> void {
        auto labelWidget = widget->findChild<QLabel*>("Label");

        Q_ASSERT(labelWidget != nullptr);

        if (labelWidget == nullptr)
            return;

        labelWidget->setFont(Application::getIconFont("FontAwesome").getFont());
    });

    connect(this, &StatusBarAction::toolButtonClicked, this, []() -> void {
        settings().getEditSettingsAction().trigger();
    });
}
