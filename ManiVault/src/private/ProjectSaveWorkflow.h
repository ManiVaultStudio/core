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
    explicit ProjectSaveWorkflow(const QString& filePath, QObject* parent = nullptr);

protected:


    QtTaskTree::Group makeRecipe() override;

private:

    /**
     * Utility method for creating a workflow context from a file. This method can be used by derived classes of AbstractWorkflow to implement custom logic for creating a workflow context based on a file, for example by reading the file and extracting any necessary information for the workflow that needs to be accessed by the tasks in the workflow. The createContextFromFile method will take a file path as an argument and return a unique pointer to a WorkflowContextBase that is initialized with the necessary information extracted from the file.
     * @param filePath Path to the file from which to create the workflow context. This method can be used for creating a workflow context based on a file, for example by reading the file and extracting any necessary information for the workflow that needs to be accessed by the tasks in the workflow. The createContextFromFile method can be used by derived classes of AbstractWorkflow to implement custom logic for creating a workflow context based on a file, and it can return a unique pointer to a WorkflowContextBase that is initialized with the necessary information extracted from the file.
     * @return A unique pointer to a WorkflowContextBase that is initialized with the necessary information extracted from the file. This method can be used by derived classes of AbstractWorkflow to implement custom logic for creating a workflow context based on a file, and it can return a unique pointer to a WorkflowContextBase that is initialized with the necessary information extracted from the file.
     */
    static UniqueWorkflowContext createContext(const QString& filePath);

private: // Stages

    void setup(ProjectSaveContext& context);
    void save(ProjectSaveContext& context);
    void finalize(ProjectSaveContext& context);
};
