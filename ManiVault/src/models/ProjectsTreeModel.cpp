// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectsTreeModel.h"

#ifdef _DEBUG
    #define PROJECTS_TREE_MODEL_VERBOSE
#endif

using namespace mv::util;
using namespace mv::gui;

namespace mv {

ProjectsTreeModel::ProjectsTreeModel(const PopulationMode& populationMode /*= Mode::Automatic*/, QObject* parent /*= nullptr*/) :
    AbstractProjectsModel(populationMode, parent),
    _visibilityController(this)
{
}

void ProjectsTreeModel::populate(ProjectsModelProjectSharedPtrs projects)
{
#ifdef PROJECTS_TREE_MODEL_VERBOSE
    qDebug() << __FUNCTION__ << "Populating projects tree model with" << projects.size() << "projects";
#endif

    for (const auto& project : projects) {
        if (!project)
            return;

        if (!project->getGroup().isEmpty()) {
            const auto groupTitleMatches = match(index(0, static_cast<std::int32_t>(Column::Title)), Qt::DisplayRole, project->getGroup(), 1, Qt::MatchExactly | Qt::MatchRecursive);

            if (groupTitleMatches.isEmpty()) {
                auto projectGroup = std::make_shared<ProjectsModelProject>(project->getGroup());

                projectGroup->setProjectsJsonDsn(project->getProjectsJsonDsn());

                addProject(projectGroup);
            }
        }

        const auto matches = match(index(0, static_cast<std::int32_t>(Column::Title)), Qt::DisplayRole, project->getGroup(), -1, Qt::MatchExactly | Qt::MatchRecursive);

        addProject(project, matches.isEmpty() ? QModelIndex() : matches.first());
    }
}

}