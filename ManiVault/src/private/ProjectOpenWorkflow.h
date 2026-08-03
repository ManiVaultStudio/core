// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <util/AbstractWorkflow.h>

#include "ProjectOpenContext.h"

/**
 * @brief Opens a ManiVault project with the legacy workflow API.
 *
 * ProjectOpenWorkflow extracts the project archive, reads project and workspace
 * JSON, and finalizes the open operation through AbstractWorkflow stages.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class ProjectOpenWorkflow final : public mv::util::AbstractWorkflow
{
public:

    /**
     * @brief Constructs a project-open workflow.
     * @param filePath Project file path to open.
     */
    explicit ProjectOpenWorkflow(const QString& filePath);

protected:

    /**
     * @brief Creates the task-tree recipe.
     * @return Task-tree group representing the workflow.
     */
    QtTaskTree::Group makeRecipe() override;

    /**
     * @brief Initializes workflow runtime storage.
     * @param context Runtime context storage shared by workflow tasks.
     */
    void setupStorage(WorkflowRuntimeContext& context) override;

    /**
     * @brief Handles completed workflow runtime storage.
     * @param context Runtime context storage shared by workflow tasks.
     */
    void onStorageDone(const WorkflowRuntimeContext& context) override;

    /**
     * @brief Handles workflow completion.
     * @param status Workflow completion status.
     */
    void handleDone(QtTaskTree::DoneWith status) override;

protected:

    /**
     * @brief Initializes the workflow result.
     * @param result Workflow result to initialize.
     */
    void initResult(UniqueWorkflowResultBase& result) override;

private:

    /**
     * @brief Creates the workflow context.
     * @param filePath Project file path used to initialize the context.
     * @return Workflow context for the open operation.
     */
    static UniqueWorkflowContext createContext(const QString& filePath);

private:

    /** Runs initial setup for opening the project. */
    void setup(ProjectOpenContext& context);

    /** Extracts the project archive into the temporary workspace. */
    void extractProjectArchive(ProjectOpenContext& context);

    /** Opens the project JSON document. */
    void openProjectJson(ProjectOpenContext& context);

    /** Opens the workspace JSON document. */
    void openWorkspaceJson(ProjectOpenContext& context);

    /** Finalizes the project-open operation. */
    void finalize(ProjectOpenContext& context);
};
