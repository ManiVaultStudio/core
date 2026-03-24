// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DataHierarchyManager.h"
#include "DataHierarchyLoadRecipeBuilder.h"

/** Context for project loading */
struct ProjectLoadContext
{
    QString         _projectJsonPath;           /** Path to the project JSON file */
    QVariantMap     _projectVariantMap;         /** Variant map containing the project data */
    QVariantMap     _dataHierarchyVariantMap;   /** Variant map containing the data hierarchy structure and dataset data */
    bool            _loadWorkspace = true;      /** Whether to load the workspace */
    QString         _workspaceJsonPath;         /** Path to the workspace JSON file */
    QString         _error;                     /** Error message, if any error occurs during the loading process */
};

/** Storage for project load context */
using ProjectLoadContextStorage = QtTaskTree::Storage<ProjectLoadContext>;

class ProjectLoadRecipeBuilder
{
public:

    /** Construct recipe builder */
    ProjectLoadRecipeBuilder();

    /**
     * Make a recipe for loading the project based on the data in \p projectLoadContextStorage
     * @param projectLoadContextStorage The storage containing the project load context
     * @return A QtTaskTree::Group representing the recipe for loading the project
     */
    QtTaskTree::Group makeRecipe(ProjectLoadContextStorage& projectLoadContextStorage);

private:
    void loadProjectJson(ProjectLoadContext& projectLoadContext);
    void loadWorkspace(ProjectLoadContext& projectLoadContext);

    QtTaskTree::Group makeLoadDataHierarchyStage(QtTaskTree::Storage<ProjectLoadContext>& projectLoadContext);

private:
    DataHierarchyLoadRecipeBuilder  _dataHierarchyLoadRecipeBuilder;

    QtTaskTree::Storage<DataHierarchyLoadContext> _dataHierarchyLoadContext;
};
