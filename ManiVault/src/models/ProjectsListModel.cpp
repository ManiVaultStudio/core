// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectsListModel.h"

#ifdef _DEBUG
    //#define PROJECTS_LIST_MODEL_VERBOSE
#endif

using namespace mv::util;
using namespace mv::gui;

namespace mv {

ProjectsListModel::ProjectsListModel(const PopulationMode& populationMode /*= Mode::Automatic*/, QObject* parent /*= nullptr*/) :
    AbstractProjectsModel(populationMode, parent)
{
}

void ProjectsListModel::populate(ProjectsModelProjectSharedPtrs projects)
{
#ifdef PROJECTS_LIST_MODEL_VERBOSE
    qDebug() << __FUNCTION__ << "Populating projects tree model with" << projects.size() << "projects";
#endif

    for (const auto& project : projects)
        if (project)
            addProject(project);
}

}
