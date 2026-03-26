// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WorkspaceLoadContext.h"

struct ProjectLoadContext;

using ProjectLoadContextStorage = QtTaskTree::Storage<ProjectLoadContext>;

/**
 * Builder for workspace load recipe
 *
 * @author Thomas Kroes
 */
class WorkspaceLoadRecipeBuilder
{
public:

    /** Construct recipe builder */
    WorkspaceLoadRecipeBuilder() = default;

    /**
     * Make a recipe for loading the workspace based on the data in \p projectLoadContextStorage
     * @param projectLoadContextStorage The storage containing the project load context, which includes the workspace load context
     * @return A QtTaskTree::Group representing the recipe for loading the workspace
     */
    QtTaskTree::Group makeRecipe(ProjectLoadContextStorage& projectLoadContextStorage);

private:

    /**
     * Load the workspace JSON file and populate the workspace load context with the data from the JSON file
     * @param workspaceLoadContext The context to populate with the workspace load data
     */
    void loadWorkspaceJson(WorkspaceLoadContext& workspaceLoadContext);

    /**
     * Load the workspace based on the data in \p workspaceLoadContext
     * @param workspaceLoadContext The context containing the workspace load data
     */
    void loadWorkspace(WorkspaceLoadContext& workspaceLoadContext);
};
