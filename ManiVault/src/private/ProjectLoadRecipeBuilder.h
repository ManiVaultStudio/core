// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DataHierarchyLoadRecipeBuilder.h"
#include "DatasetsLoadRecipeBuilder.h"
#include "WorkspaceLoadRecipeBuilder.h"

struct ProjectOpenContext;

using OpenProjectContextStorage = QtTaskTree::Storage<ProjectOpenContext>;

/** Context for project loading */
struct ProjectLoadContext
{
	QVariantMap     _projectVariantMap;             /** Variant map containing the project data */
    QVariantMap     _dataHierarchyVariantMap;       /** Variant map containing the data hierarchy structure and dataset data */
    bool            _loadWorkspace = true;          /** Whether to load the workspace */
    QString         _error;                         /** Error message, if any error occurs during the loading process */

    DataHierarchyLoadContext    _dataHierarchyLoadContext;      /** Context for loading the data hierarchy, which contains the dataset entries to be loaded in the correct order based on their dependencies */
    WorkspaceLoadContext        _workspaceLoadContext;          /* Context for loading the workspace, which contains the workspace load data */
    DatasetsLoadContext         _datasetsLoadContext;           /* Context for loading the datasets, which contains the dataset entries to be loaded in the correct order based on their dependencies */
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
     * @param openProjectContextStorage The storage containing the open project context
     * @return A QtTaskTree::Group representing the recipe for loading the project
     */
    QtTaskTree::Group makeRecipe(OpenProjectContextStorage& openProjectContextStorage, ProjectLoadContextStorage& projectLoadContextStorage);
private:
    DataHierarchyLoadRecipeBuilder      _dataHierarchyLoadRecipeBuilder;    /** Builder for the data hierarchy load recipe, which is used to create the stage of loading the datasets in the data hierarchy based on the dataset entries in \p dataHierarchyLoadContextStorage */
    DatasetsLoadRecipeBuilder           _datasetsLoadRecipeBuilder;         /** Builder for the datasets load recipe, which is used to create the stage of loading the datasets based on the dataset entries in \p datasetsLoadContextStorage */
    WorkspaceLoadRecipeBuilder          _workspaceLoadRecipeBuilder;        /** Builder for the workspace load recipe, which is used to create the stage of loading the workspace based on the data in \p workspaceLoadContextStorage */
};
