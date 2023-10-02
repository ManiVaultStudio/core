// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SplashScreenAction.h"
#include "ProjectMetaAction.h"
#include "Application.h"

#include "widgets/SplashScreenWidget.h"

namespace hdps::gui {

SplashScreenAction::SplashScreenAction(QObject* parent, bool mayClose /*= false*/) :
    HorizontalGroupAction(parent, "Splash Screen"),
    _mayCloseSplashScreenWidget(mayClose),
    _projectMetaAction(nullptr),
    _enabledAction(this, "Enable splash screen"),
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
    addAction(&_openAction);

    _editAction.addAction(&_enabledAction);
    _editAction.addAction(&_projectImageAction);
    _editAction.addAction(&_affiliateLogosImageAction);

    getTaskAction().setTask(Application::current()->getTask(Application::TaskType::LoadApplication));

    setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _taskAction.setTask(Application::current()->getTask(Application::TaskType::LoadApplication));

    _enabledAction.setStretch(1);
    _enabledAction.setToolTip("Show splash screen at startup");

    _openAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _openAction.setIcon(fontAwesome.getIcon("eye"));
    _openAction.setToolTip("Open the splash screen");

    _closeAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _closeAction.setIcon(fontAwesome.getIcon("eye-slash"));
    _closeAction.setToolTip("Close the splash screen");

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

bool SplashScreenAction::getMayCloseSplashScreenWidget() const
{
    return _mayCloseSplashScreenWidget;
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
    _projectImageAction.fromParentVariantMap(variantMap);
    _affiliateLogosImageAction.fromParentVariantMap(variantMap);
}

QVariantMap SplashScreenAction::toVariantMap() const
{
    auto variantMap = HorizontalGroupAction::toVariantMap();

    _enabledAction.insertIntoVariantMap(variantMap);
    _projectImageAction.insertIntoVariantMap(variantMap);
    _affiliateLogosImageAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}