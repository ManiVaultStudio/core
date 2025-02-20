// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkspaceSettingsDialog.h"

#include <AbstractWorkspaceManager.h>

#include <Application.h>
#include <CoreInterface.h>

#include <QVBoxLayout>

#ifdef _DEBUG
    #define WORKSPACE_SETTINGS_DIALOG_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

WorkspaceSettingsDialog::WorkspaceSettingsDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _groupAction(this, "Group"),
    _okAction(this, "Ok")
{
    setWindowIcon(StyledIcon("cog"));
    setModal(true);
    setWindowTitle("Workspace Settings");

    _groupAction.setLabelSizingType(GroupAction::LabelSizingType::Auto);

    const auto workspace = workspaces().getCurrentWorkspace();

    _groupAction.addAction(&workspace->getTitleAction());
    _groupAction.addAction(&workspace->getDescriptionAction());
    _groupAction.addAction(&workspace->getTagsAction());
    _groupAction.addAction(&workspace->getCommentsAction());
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

    connect(&_okAction, &TriggerAction::triggered, this, &WorkspaceSettingsDialog::accept);
}
