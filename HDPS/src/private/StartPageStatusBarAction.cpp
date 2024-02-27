// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageStatusBarAction.h"

#ifdef _DEBUG
    #define START_PAGE_STATUS_BAR_ACTION_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;

StartPageStatusBarAction::StartPageStatusBarAction(QObject* parent, const QString& title) :
    StatusBarAction(parent, title),
    _barGroupAction(this, "Bar Group"),
    _iconAction(this, "Icon")
{
    setBarAction(&_barGroupAction);

    _barGroupAction.setShowLabels(false);

    _barGroupAction.addAction(&_iconAction);

    _iconAction.setEnabled(false);
    _iconAction.setDefaultWidgetFlags(StringAction::Label);
    _iconAction.setString(Application::getIconFont("FontAwesome").getIconCharacter("door-open"));
    _iconAction.setToolTip("Toggle the start page");
    _iconAction.setWidgetConfigurationFunction([](WidgetAction* action, QWidget* widget) -> void {
        auto labelWidget = widget->findChild<QLabel*>("Label");

        Q_ASSERT(labelWidget != nullptr);

        if (labelWidget == nullptr)
            return;

        labelWidget->setFont(Application::getIconFont("FontAwesome").getFont());
    });
    
    connect(this, &StatusBarAction::toolButtonClicked, this, []() -> void {
        projects().getShowStartPageAction().toggle();
    });
}
