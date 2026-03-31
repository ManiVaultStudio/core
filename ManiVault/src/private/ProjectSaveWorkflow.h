// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/AbstractWorkflow.h>

#include "ProjectSaveContext.h"

namespace mv
{
    class ProjectManager;
}

class ProjectSaveWorkflow final : public mv::util::AbstractWorkflow
{
    Q_OBJECT

public:
    explicit ProjectSaveWorkflow(mv::ProjectManager& projectManager, const QString& filePath, QObject* parent = nullptr);

protected:

    /**
     * Initialize the workflow context with the provided project file path and other necessary information for the save operation.
     * @param projectSaveContext Reference to the context storage for the workflow, used for passing data between tasks. The context will be initialized with the project file path and any other necessary information for the save operation.
     */
    void initializeContext(ProjectSaveContext& projectSaveContext) override;

    QtTaskTree::Group makeRecipe() override;

    void setup(ProjectSaveContext& context) override;
    void save(ProjectSaveContext& context);
    void finalize(ProjectSaveContext& context) override;



private:
    mv::ProjectManager&     _projectManager;        /** Reference to the project manager, used for performing project save operations. */
    QString                 _finalError;            /** Final error message, if any */
    ProjectSaveContext          _projectSaveContext;    /** Context storage for the workflow, used for passing data between tasks. */
    ProjectSaveContextStorage   _contextStorage;    /** TODO */
};
