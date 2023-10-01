// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SplashScreenAction.h"
#include "ProjectMetaAction.h"
#include "Application.h"

#include "widgets/SplashScreenWidget.h"

namespace hdps::gui {

SplashScreenAction::SplashScreenAction(QObject* parent, bool mayClose /*= false*/) :
    HorizontalGroupAction(parent, "Group"),
    _mayClose(mayClose),
    _projectMetaAction(nullptr),
    _enabledAction(this, "Enable splash screen"),
    _closeManuallyAction(this, "Close manually"),
    _durationAction(this, "Duration", 1000, 10000, 4000),
    _animationDurationAction(this, "Animation Duration", 10, 10000, 250),
    _animationPanAmountAction(this, "Pan Amount", 10, 100, 20),
    _projectImageAction(this, "Project Image"),
    _affiliateLogosImageAction(this, "Affiliate Logos"),
    _editAction(this, "Edit"),
    _openAction(this, "Open splash screen"),
    _closeAction(this, "Close splash screen"),
    _taskAction(this, "ManiVault"),
    _splashScreenWidget()
{
    addAction(&_enabledAction);
    addAction(&_editAction);

    _editAction.addAction(&_enabledAction);
    _editAction.addAction(&_closeManuallyAction);
    _editAction.addAction(&_durationAction);
    _editAction.addAction(&_projectImageAction);
    _editAction.addAction(&_affiliateLogosImageAction);
    _editAction.addAction(&_openAction);

    getTaskAction().setTask(Application::current()->getTask(Application::TaskType::LoadApplication));

    setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _taskAction.setTask(Application::current()->getTask(Application::TaskType::LoadApplication));

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

    //_editAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _editAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _editAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);
    _editAction.setIcon(fontAwesome.getIcon("cog"));
    _editAction.setToolTip("Edit the splash screen settings");
    _editAction.setPopupSizeHint(QSize(350, 0));

    _projectImageAction.setDefaultWidgetFlags(ImageAction::Loader);
    _projectImageAction.setIcon(fontAwesome.getIcon("image"));
    _projectImageAction.setToolTip("Project image");

    _affiliateLogosImageAction.setDefaultWidgetFlags(ImageAction::Loader);
    _affiliateLogosImageAction.setIcon(fontAwesome.getIcon("image"));
    _affiliateLogosImageAction.setToolTip("Affiliate logos image");

    const auto updateDurationAction = [this]() -> void {
        _durationAction.setEnabled(!_closeManuallyAction.isChecked());
    };

    updateDurationAction();

    connect(&_closeManuallyAction, &ToggleAction::toggled, this, updateDurationAction);

    connect(&_openAction, &TriggerAction::triggered, this, &SplashScreenAction::showSplashScreenWidget);
    connect(&_closeAction, &TriggerAction::triggered, this, &SplashScreenAction::closeSplashScreenWidget);

    connect(Application::current()->getTask(Application::TaskType::LoadApplication), &Task::statusChanged, this, [this](const Task::Status& previousStatus, const Task::Status& status) -> void {
        if (previousStatus == Task::Status::Finished && status == Task::Status::Idle)
            closeSplashScreenWidget();
    });
}

SplashScreenAction::~SplashScreenAction()
{
}

bool SplashScreenAction::getMayClose() const
{
    return _mayClose;
}

ProjectMetaAction* SplashScreenAction::getProjectMetaAction()
{
    return _projectMetaAction;
}

void SplashScreenAction::setProjectMetaAction(ProjectMetaAction* projectMetaAction)
{
    _projectMetaAction = projectMetaAction;
}

void SplashScreenAction::showSplashScreenWidget()
{
    if (_splashScreenWidget.isNull()) {
        _splashScreenWidget = new SplashScreenWidget(*this, nullptr);

        connect(_splashScreenWidget, &QWidget::destroyed, this, [this]() -> void {
            _splashScreenWidget.clear();
        });
    }
        
    _splashScreenWidget->showAnimated();
}

void SplashScreenAction::closeSplashScreenWidget()
{
    if (_splashScreenWidget.isNull())
        return;

    _splashScreenWidget->closeAnimated();

    _splashScreenWidget.clear();
}

void SplashScreenAction::fromVariantMap(const QVariantMap& variantMap)
{
    HorizontalGroupAction::fromVariantMap(variantMap);

    _enabledAction.fromParentVariantMap(variantMap);
    _closeManuallyAction.fromParentVariantMap(variantMap);
    _durationAction.fromParentVariantMap(variantMap);
    _animationDurationAction.fromParentVariantMap(variantMap);
    _animationPanAmountAction.fromParentVariantMap(variantMap);
    _projectImageAction.fromParentVariantMap(variantMap);
    _affiliateLogosImageAction.fromParentVariantMap(variantMap);
}

QVariantMap SplashScreenAction::toVariantMap() const
{
    auto variantMap = HorizontalGroupAction::toVariantMap();

    _enabledAction.insertIntoVariantMap(variantMap);
    _closeManuallyAction.insertIntoVariantMap(variantMap);
    _durationAction.insertIntoVariantMap(variantMap);
    _animationDurationAction.insertIntoVariantMap(variantMap);
    _animationPanAmountAction.insertIntoVariantMap(variantMap);
    _projectImageAction.insertIntoVariantMap(variantMap);
    _affiliateLogosImageAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}