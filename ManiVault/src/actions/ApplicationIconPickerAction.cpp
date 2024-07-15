// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ApplicationIconPickerAction.h"

#include <QMainWindow>
#include <QImage>

#include "Application.h"

namespace mv::gui {

ApplicationIconPickerAction::ApplicationIconPickerAction(QObject* parent, const QString& title) :
    IconPickerAction(parent, title),
    _testAction(this, "Test"),
    _testTimer(this)
{
    addAction(&_testAction);

    const auto testIconName = "play";

    _testAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);
    _testAction.setToolTip("Briefly (5s) shows the icon in the main window");
    _testAction.setIconByName(testIconName);

    _testTimer.setSingleShot(true);
    _testTimer.setInterval(2000);

    connect(&_testAction, &TriggerAction::triggered, this, [this]() -> void {
        Application::getMainWindow()->setWindowIcon(getIcon());

        _testAction.setIconByName("stopwatch");
        _testTimer.start();
    });

    connect(&_testTimer, &QTimer::timeout, this, [this, testIconName]() -> void {
        _testAction.setIconByName(testIconName);

        Application::getMainWindow()->setWindowIcon(createIcon(QPixmap(":/Icons/AppIcon256")));

        _testAction.setIconByName(testIconName);
    });
}

void ApplicationIconPickerAction::fromVariantMap(const QVariantMap& variantMap)
{
    IconPickerAction::fromVariantMap(variantMap);

    _testAction.fromParentVariantMap(variantMap);
}

QVariantMap ApplicationIconPickerAction::toVariantMap() const
{
    auto variantMap = IconPickerAction::toVariantMap();

    _testAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
