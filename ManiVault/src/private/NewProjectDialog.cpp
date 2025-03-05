// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "NewProjectDialog.h"

#include "PageAction.h"

#include <AbstractWorkspaceManager.h>

#include <CoreInterface.h>

#include <QVBoxLayout>

#ifdef _DEBUG
    #define NEW_PROJECT_DIALOG_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

NewProjectDialog::NewProjectDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _workspacesWidget(this, "", false),
    _cancelAction(this, "Cancel")
{
    setWindowIcon(StyledIcon("file"));
    setModal(true);
    setWindowTitle("New Project From Workspace");

    _workspacesWidget.getLayout().setContentsMargins(0, 0, 0, 0);

    auto layout = new QVBoxLayout();

    layout->addWidget(&_workspacesWidget);

    auto bottomLayout = new QHBoxLayout();

    bottomLayout->addStretch(1);
    bottomLayout->addWidget(_cancelAction.createWidget(this));

    layout->addLayout(bottomLayout);

    setLayout(layout);

    _cancelAction.setToolTip("Exit the dialog without creating a new project");

    connect(&_cancelAction, &TriggerAction::triggered, this, &QDialog::reject);

    _workspacesWidget.getHierarchyWidget().setItemTypeName("Workspace");

    _workspacesWidget.getModel().reset();

    for (const auto workspaceLocation : workspaces().getWorkspaceLocations(WorkspaceLocation::Types(WorkspaceLocation::Type::BuiltIn))) {
        Workspace workspace(workspaceLocation.getFilePath());

        PageAction fromWorkspaceStartPageAction(workspaces().getIcon(), QFileInfo(workspaceLocation.getFilePath()).baseName(), workspaceLocation.getFilePath(), workspace.getDescriptionAction().getString(), workspaceLocation.getFilePath(), [this, workspaceLocation]() -> void {
            projects().newProject(workspaceLocation.getFilePath());

            QDialog::accept();
        });

        fromWorkspaceStartPageAction.setComments(workspace.getCommentsAction().getString());
        fromWorkspaceStartPageAction.setTags(workspace.getTagsAction().getStrings());
        fromWorkspaceStartPageAction.setMetaData(workspaceLocation.getTypeName());
        fromWorkspaceStartPageAction.setPreviewImage(projects().getWorkspacePreview(workspaceLocation.getFilePath()));

        _workspacesWidget.getModel().add(fromWorkspaceStartPageAction);
    }
}
