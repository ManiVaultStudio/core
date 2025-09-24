// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ApplicationIconAction.h"

#include <QMainWindow>

#include "Application.h"
#include "util/Icon.h"

namespace mv::gui {

ApplicationIconAction::ApplicationIconAction(QObject* parent, const QString& title) :
    HorizontalGroupAction(parent, title),
    _overrideAction(this, "Override", true),
    _iconPickerAction(this, "IconPicker"),
    _testAction(this, "Test"),
    _testTimer(this)
{
    setShowLabels(false);

    auto& inputFilePathPickerAction = _iconPickerAction.getInputFilePathPickerAction();

    addAction(&_overrideAction);
    addAction(&inputFilePathPickerAction.getPickAction());
    addAction(&_iconPickerAction.getIconAction());
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
        ApplicationIconAction::resetApplicationIcon();

        _testAction.setIconByName(testIconName);
    });

    const auto updateApplicationIconActionReadOnly = [this]() -> void {
        const auto overrideApplicationIcon = _overrideAction.isChecked();

        _iconPickerAction.getInputFilePathPickerAction().getPickAction().setEnabled(overrideApplicationIcon);
        _iconPickerAction.getIconAction().setEnabled(overrideApplicationIcon);
    };

    updateApplicationIconActionReadOnly();

    connect(&_overrideAction, &ToggleAction::toggled, this, updateApplicationIconActionReadOnly);

    _overrideAction.setSortIndex(0);
    _overrideAction.setStretch(1);
    _overrideAction.setToolTip("Whether to override the default ManiVault application icon when the project is in published mode");

    const auto updateTestActionReadOnly = [this]() -> void {
        _testAction.setEnabled(_overrideAction.isChecked() && !_iconPickerAction.getIcon().isNull());
    };

    updateTestActionReadOnly();

    connect(&_iconPickerAction, &IconPickerAction::iconChanged, this, updateTestActionReadOnly);
    connect(&_overrideAction, &ToggleAction::toggled, this, updateTestActionReadOnly);
}

void ApplicationIconAction::overrideMainWindowIcon() const
{
    if (_iconPickerAction.getIcon().isNull())
        return;

    Application::getMainWindow()->setWindowIcon(_iconPickerAction.getIcon());
}

void ApplicationIconAction::resetApplicationIcon()
{
    Application::getMainWindow()->setWindowIcon(createIcon(QPixmap(":/Icons/AppIcon256")));
}

void ApplicationIconAction::fromVariantMap(const QVariantMap& variantMap)
{
    HorizontalGroupAction::fromVariantMap(variantMap);

    _overrideAction.fromParentVariantMap(variantMap);
    _iconPickerAction.fromParentVariantMap(variantMap);
    _testAction.fromParentVariantMap(variantMap);
}

QVariantMap ApplicationIconAction::toVariantMap() const
{
    auto variantMap = HorizontalGroupAction::toVariantMap();

    _overrideAction.insertIntoVariantMap(variantMap);
    _iconPickerAction.insertIntoVariantMap(variantMap);
    _testAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
