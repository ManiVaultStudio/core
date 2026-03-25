// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WorkspaceManager.h"

/** Context for workspace loading */
struct WorkspaceLoadContext
{
    QString     _workspaceJsonPath;     /** Path to the workspace JSON file */
    QString     _error;                 /** Error message, if any error occurs during the loading process */
};

/** Storage for workspace load context */
using WorkspaceLoadContextStorage = QtTaskTree::Storage<WorkspaceLoadContext>;

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
     * Make a recipe for loading the workspace based on the data in \p workspaceLoadContextStorage
     * @param workspaceLoadContextStorage The storage containing the workspace load context
     * @return A QtTaskTree::Group representing the recipe for loading the workspace
     */
    QtTaskTree::Group makeRecipe(WorkspaceLoadContextStorage& workspaceLoadContextStorage);

private:

    /**
     * Load the workspace based on the data in \p workspaceLoadContext
     * @param workspaceLoadContext The context containing the data for loading the workspace
     */
    void loadWorkspace(const WorkspaceLoadContext& workspaceLoadContext);
};
