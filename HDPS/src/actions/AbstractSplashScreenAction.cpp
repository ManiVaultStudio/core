// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractSplashScreenAction.h"
#include "Application.h"

namespace hdps::gui {

AbstractSplashScreenAction::AbstractSplashScreenAction(QObject* parent) :
    HorizontalGroupAction(parent, "Group"),
    _enabledAction(this, "Enable splash screen"),
    _closeManuallyAction(this, "Close manually"),
    _durationAction(this, "Duration", 1000, 10000, 4000),
    _animationDurationAction(this, "Animation Duration", 10, 10000, 250),
    _animationPanAmountAction(this, "Pan Amount", 10, 100, 20),
    _backgroundColorAction(this, "Background Color", Qt::white),
    _openAction(this, "Open splash screen"),
    _closeAction(this, "Close splash screen"),
    _splashScreenDialog(),
    _taskAction(this, "Application Startup")
{
    addAction(&_enabledAction);
    
    //auto& fontAwesome = Application::getIconFont("FontAwesome");

    _enabledAction.setStretch(1);
    _enabledAction.setToolTip("Show splash screen at startup");

    _closeManuallyAction.setToolTip("Close manually");

    _durationAction.setToolTip("Duration of the splash screen in milliseconds");
    _durationAction.setSuffix("ms");

    _animationDurationAction.setToolTip("Duration of the splash screen animations in milliseconds");
    _animationDurationAction.setSuffix("ms");

    _animationPanAmountAction.setToolTip("Amount of panning up/down during animation in pixels");
    _animationPanAmountAction.setSuffix("px");

    //_openAction.setDefaultWidgetFlags(TriggerAction::Icon);
    //_openAction.setIcon(fontAwesome.getIcon("eye"));
    _openAction.setToolTip("Open the splash screen");

    //_closeAction.setDefaultWidgetFlags(TriggerAction::Icon);
    //_closeAction.setIcon(fontAwesome.getIcon("eye-slash"));
    _closeAction.setToolTip("Close the splash screen");

    const auto updateDurationAction = [this]() -> void {
        _durationAction.setEnabled(!_closeManuallyAction.isChecked());
    };

    updateDurationAction();

    connect(&_closeManuallyAction, &ToggleAction::toggled, this, updateDurationAction);

    connect(&_openAction, &TriggerAction::triggered, this, &AbstractSplashScreenAction::openSplashScreenDialog);
    connect(&_closeAction, &TriggerAction::triggered, this, &AbstractSplashScreenAction::closeSplashScreenDialog);
}

void AbstractSplashScreenAction::openSplashScreenDialog()
{
    if (_splashScreenDialog.isNull()) {
        _splashScreenDialog.reset(getSplashScreenDialog());
        _splashScreenDialog->open();

        Application::processEvents();
    }
    else {
        qDebug() << "Cannot create more than one splash screen simultaneously";
    }
}

void AbstractSplashScreenAction::closeSplashScreenDialog()
{
    if (_splashScreenDialog.isNull()) {
        qDebug() << "No splash screen active which can be closed";
    }
    else {
        _splashScreenDialog->close();
        _splashScreenDialog.reset();
    }
}

void AbstractSplashScreenAction::fromVariantMap(const QVariantMap& variantMap)
{
    HorizontalGroupAction::fromVariantMap(variantMap);

    _enabledAction.fromParentVariantMap(variantMap);
    _closeManuallyAction.fromParentVariantMap(variantMap);
    _durationAction.fromParentVariantMap(variantMap);
    _animationDurationAction.fromParentVariantMap(variantMap);
    _animationPanAmountAction.fromParentVariantMap(variantMap);
    _backgroundColorAction.fromParentVariantMap(variantMap);
}

QVariantMap AbstractSplashScreenAction::toVariantMap() const
{
    auto variantMap = HorizontalGroupAction::toVariantMap();

    _enabledAction.insertIntoVariantMap(variantMap);
    _closeManuallyAction.insertIntoVariantMap(variantMap);
    _durationAction.insertIntoVariantMap(variantMap);
    _animationDurationAction.insertIntoVariantMap(variantMap);
    _animationPanAmountAction.insertIntoVariantMap(variantMap);
    _backgroundColorAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}