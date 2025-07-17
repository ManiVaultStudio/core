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

    for (int rowIndex = 0; rowIndex < _projectsFilterModel.rowCount(); rowIndex++) {
        const auto sourceModelIndex = _projectsFilterModel.mapToSource(_projectsFilterModel.index(rowIndex, 0));

        if (const auto project = mv::projects().getProjectsTreeModel().getProject(sourceModelIndex)) {
            if (mv::projects().hasProject()) {
                if (QFileInfo(mv::projects().getCurrentProject()->getFilePath()).completeBaseName() == project->getTitle())
				continue;
            }

            if (project->isGroup()) {
                auto projectGroupMenu = new QMenu(project->getTitle());

                addMenu(projectGroupMenu);
            } else {
                auto groupAction = new TriggerAction(this, project->getTitle());

                connect(groupAction, &TriggerAction::triggered, this, [project]() {
                    mv::projects().openProject(project->getUrl());
                });

                addAction(groupAction);
            }

        }
    }
}
