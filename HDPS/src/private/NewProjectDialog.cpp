// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "NewProjectDialog.h"

#include <AbstractWorkspaceManager.h>

#include <Application.h>
#include <CoreInterface.h>

#include <QVBoxLayout>

#ifdef _DEBUG
    #define NEW_PROJECT_DIALOG_VERBOSE
#endif

using namespace hdps;
using namespace hdps::gui;

NewProjectDialog::NewProjectDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _workspacesWidget(this, "", false),
    _cancelAction(this, "Cancel")
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("file"));
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

        StartPageAction fromWorkspaceStartPageAction(workspaces().getIcon(), QFileInfo(workspaceLocation.getFilePath()).baseName(), workspaceLocation.getFilePath(), workspace.getDescriptionAction().getString(), workspaceLocation.getFilePath(), [this, workspaceLocation]() -> void {
            projects().newProject(workspaceLocation.getFilePath());

            QDialog::accept();
        });

        fromWorkspaceStartPageAction.setComments(workspace.getCommentsAction().getString());
        fromWorkspaceStartPageAction.setTags(workspace.getTagsAction().getStrings());
        fromWorkspaceStartPageAction.setMetaData(workspaceLocation.getTypeName());
        fromWorkspaceStartPageAction.setPreviewImage(Workspace::getPreviewImage(workspaceLocation.getFilePath()));

        _workspacesWidget.getModel().add(fromWorkspaceStartPageAction);
    }
}
