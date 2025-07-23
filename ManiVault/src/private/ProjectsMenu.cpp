// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectsMenu.h"

#include "CoreInterface.h"

#include <util/Miscellaneous.h>
#include <actions/TriggerAction.h>

#include <QOperatingSystemVersion>

using namespace mv;
using namespace mv::util;
using namespace mv::gui;
using namespace mv::plugin;

ProjectsMenu::ProjectsMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent)
{
    setTitle("Projects");
    setToolTip("Projects");

    _projectsFilterModel.setSourceModel(const_cast<ProjectsTreeModel*>(&mv::projects().getProjectsTreeModel()));

    populate();
}

void ProjectsMenu::showEvent(QShowEvent* event)
{
    QMenu::showEvent(event);

    if (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::Windows)
        populate();
}

void ProjectsMenu::populate()
{
    clear();

    const auto addProject = [this](QMenu* menu, ProjectsModelProjectSharedPtr project) -> void {
        auto loadProjectAction = new TriggerAction(this, project->getTitle());

        connect(loadProjectAction, &TriggerAction::triggered, this, [project]() {
            mv::projects().openProject(project->getUrl());  
        });

        menu->addAction(loadProjectAction);
    };

    for (int rowIndex = 0; rowIndex < _projectsFilterModel.rowCount(); rowIndex++) {
        const auto filterModelIndex = _projectsFilterModel.index(rowIndex, 0);
        const auto sourceModelIndex = _projectsFilterModel.mapToSource(filterModelIndex);

        if (const auto project = mv::projects().getProjectsTreeModel().getProject(sourceModelIndex)) {
            if (mv::projects().hasProject()) {
                if (QFileInfo(mv::projects().getCurrentProject()->getFilePath()).completeBaseName() == project->getTitle())
					continue;
            }

            if (project->isGroup()) {
                auto projectGroupMenu = new QMenu(project->getTitle());

                for (int childRowIndex = 0; childRowIndex < mv::projects().getProjectsTreeModel().rowCount(sourceModelIndex); childRowIndex++) {
                    const auto childModelIndex = mv::projects().getProjectsTreeModel().index(childRowIndex, 0, sourceModelIndex);

                    if (const auto childProject = mv::projects().getProjectsTreeModel().getProject(childModelIndex))
						addProject(projectGroupMenu, childProject);
                }

                addMenu(projectGroupMenu);
            } else {
                addProject(this, project);
            }
        }
    }

    if (!actions().isEmpty())
        addSeparator();

    addAction(&const_cast<ProjectsTreeModel&>(mv::projects().getProjectsTreeModel()).getEditDsnsAction());
}
