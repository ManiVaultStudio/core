// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <util/AbstractWorkflow.h>

#include "ProjectSaveContext.h"

/**
 * @brief Saves a ManiVault project with the legacy workflow API.
 *
 * ProjectSaveWorkflow writes project, metadata, and workspace JSON into a
 * temporary directory before archiving the result to the target project file.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class ProjectSaveWorkflow final : public mv::util::AbstractWorkflow
{
public:

    /**
     * @brief Constructs a project-save workflow.
     * @param filePath Project file path to save to.
     */
    explicit ProjectSaveWorkflow(const QString& filePath);

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

public:

    /**
     * @brief Returns the temporary directory.
     * @return Temporary directory used while saving the project.
     */
    [[nodiscard]] const QTemporaryDir& getTemporaryDir() const;

    /**
     * @brief Returns the temporary directory path.
     * @return Path of the temporary directory used while saving the project.
     */
    [[nodiscard]] QString getTemporaryDirPath() const;

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
     * @return Workflow context for the save operation.
     */
    static UniqueWorkflowContext createContext(const QString& filePath);

private:

    /** Runs initial setup for saving the project. */
    void setup(ProjectSaveContext& context);

    /** Saves the project JSON document. */
    void saveProjectJson(ProjectSaveContext& context);

    /** Saves the project metadata JSON document. */
    void saveProjectMetaJson(ProjectSaveContext& context);

    /** Saves the workspace JSON document. */
    void saveWorkspaceJson(ProjectSaveContext& context);

    /** Archives the temporary project files. */
    void archive(ProjectSaveContext& context);

    /** Finalizes the project-save operation. */
    void finalize(ProjectSaveContext& context);

private:

    QTemporaryDir   _temporaryDir;  /**< Temporary directory used while saving the project. */
};
