// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ApplicationIconPickerAction.h"

#include <QMainWindow>

#include "Application.h"

namespace mv::gui {

ApplicationIconPickerAction::ApplicationIconPickerAction(QObject* parent, const QString& title) :
    IconPickerAction(parent, title),
    _overrideAction(this, "Override application icon"),
    _testAction(this, "Test"),
    _testTimer(this)
{
    addAction(&_overrideAction);
    addAction(&_testAction);

    const auto testIconName = "play";

    _testAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);
    _testAction.setToolTip("Briefly (2s) shows the icon in the main window");
    _testAction.setIconByName(testIconName);

    _testTimer.setSingleShot(true);
    _testTimer.setInterval(2000);

    connect(&_testAction, &TriggerAction::triggered, this, [this]() -> void {
        overrideMainWindowIcon();

        _testAction.setIconByName("stopwatch");
        _testTimer.start();
    });

    connect(&_testTimer, &QTimer::timeout, this, [this, testIconName]() -> void {
        ApplicationIconPickerAction::resetApplicationIcon();

        _testAction.setIconByName(testIconName);
    });

    const auto updateApplicationIconActionReadOnly = [this]() -> void {
        const auto overrideApplicationIcon = _overrideAction.isChecked();

        getInputFilePathPickerAction().setEnabled(overrideApplicationIcon);
        getIconAction().setEnabled(overrideApplicationIcon);
        getTestAction().setEnabled(overrideApplicationIcon);
    };

    updateApplicationIconActionReadOnly();

    connect(&_overrideAction, &ToggleAction::toggled, this, updateApplicationIconActionReadOnly);

    _overrideAction.setSortIndex(0);
    _overrideAction.setToolTip("Whether to override the default ManiVault application icon when the project is in published mode");

    const auto updateTestActionReadOnly = [this]() -> void {
        _testAction.setEnabled(!getIcon().isNull());
    };

    updateTestActionReadOnly();

    connect(this, &IconPickerAction::iconChanged, this, updateTestActionReadOnly);
}

void ApplicationIconPickerAction::overrideMainWindowIcon() const
{
    if (getIcon().isNull())
        return;

    Application::getMainWindow()->setWindowIcon(getIcon());
}

void ApplicationIconPickerAction::resetApplicationIcon()
{
    Application::getMainWindow()->setWindowIcon(createIcon(QPixmap(":/Icons/AppIcon256")));
}

void ApplicationIconPickerAction::fromVariantMap(const QVariantMap& variantMap)
{
    IconPickerAction::fromVariantMap(variantMap);

    _overrideAction.fromParentVariantMap(variantMap);
    _testAction.fromParentVariantMap(variantMap);
}

QVariantMap ApplicationIconPickerAction::toVariantMap() const
{
    auto variantMap = IconPickerAction::toVariantMap();

    _overrideAction.insertIntoVariantMap(variantMap);
    _testAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
