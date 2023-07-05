// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectSettingsDialog.h"

#include <AbstractProjectManager.h>

#include <Application.h>
#include <CoreInterface.h>

#include <QVBoxLayout>
#include <QVector>

#ifdef _DEBUG
    #define PROJECT_SETTINGS_DIALOG_VERBOSE
#endif

using namespace hdps;
using namespace hdps::gui;

ProjectSettingsDialog::ProjectSettingsDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _groupAction(this, "Group"),
    _okAction(this, "Ok")
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    setModal(true);
    setWindowTitle("Project Settings");

    _groupAction.setLabelSizingType(GroupAction::LabelSizingType::Auto);

    const auto project = projects().getCurrentProject();

    _groupAction.addAction(&project->getTitleAction());
    _groupAction.addAction(&project->getDescriptionAction());
    _groupAction.addAction(&project->getProjectVersionAction());
    _groupAction.addAction(&project->getTagsAction());
    _groupAction.addAction(&project->getCommentsAction());
    _groupAction.addAction(&project->getContributorsAction());
    _groupAction.addAction(&project->getSplashScreenAction());
    _groupAction.addAction(&project->getStudioModeAction());
    _groupAction.addAction(&workspaces().getLockingAction().getLockedAction());

    auto layout = new QVBoxLayout();

    auto groupActionWidget = _groupAction.createWidget(this);

    groupActionWidget->layout()->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(groupActionWidget);

    auto bottomLayout = new QHBoxLayout();

    bottomLayout->addStretch(1);
    bottomLayout->addWidget(_okAction.createWidget(this));

    layout->addLayout(bottomLayout);

    setLayout(layout);

    connect(&_okAction, &TriggerAction::triggered, this, &ProjectSettingsDialog::accept);
}
