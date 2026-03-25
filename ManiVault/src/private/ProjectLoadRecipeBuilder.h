// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DataHierarchyManager.h"
#include "DataHierarchyLoadRecipeBuilder.h"
#include "WorkspaceLoadRecipeBuilder.h"

/** Context for project loading */
struct ProjectLoadContext
{
    QString                     _projectJsonPath;               /** Path to the project JSON file */
    QVariantMap                 _projectVariantMap;             /** Variant map containing the project data */
    QVariantMap                 _dataHierarchyVariantMap;       /** Variant map containing the data hierarchy structure and dataset data */
    bool                        _loadWorkspace = true;          /** Whether to load the workspace */
    QString                     _workspaceJsonPath;             /** Path to the workspace JSON file */
    QString                     _error;                         /** Error message, if any error occurs during the loading process */
    DataHierarchyLoadContext    _dataHierarchyLoadContext;      /** Storage for data hierarchy load context */
    WorkspaceLoadContext        _workspaceLoadContext;          /** Storage for workspace load context */
};

/** Storage for project load context */
using ProjectLoadContextStorage = QtTaskTree::Storage<ProjectLoadContext>;

/**
 * Builder for project load recipe
 *
 * @author Thomas Kroes
 */
class ProjectLoadRecipeBuilder
{
public:

    /** Construct recipe builder */
    ProjectLoadRecipeBuilder() = default;

    /**
     * Make a recipe for loading the project based on the data in \p projectLoadContextStorage
     * @param projectLoadContextStorage The storage containing the project load context
     * @return A QtTaskTree::Group representing the recipe for loading the project
     */
    QtTaskTree::Group makeRecipe(ProjectLoadContextStorage& projectLoadContextStorage);

private:
    //QtTaskTree::Group makeLoadDataHierarchyStage(QtTaskTree::Storage<ProjectLoadContext>& projectLoadContext);

private:
    DataHierarchyLoadRecipeBuilder      _dataHierarchyLoadRecipeBuilder;
    WorkspaceLoadRecipeBuilder          _workspaceLoadRecipeBuilder;
    DataHierarchyLoadContextStorage     _dataHierarchyLoadContextStorage;
    WorkspaceLoadContextStorage         _workspaceLoadContextStorage;
};
