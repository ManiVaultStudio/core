// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DataHierarchyLoadContext.h"
#include "WorkspaceLoadContext.h"
#include "DatasetsLoadContext.h"


/** Context for project loading */
struct ProjectLoadContext
{
	QVariantMap     _projectVariantMap;             /** Variant map containing the project data */
    QVariantMap     _dataHierarchyVariantMap;       /** Variant map containing the data hierarchy structure and dataset data */
    bool            _loadWorkspace = true;          /** Whether to load the workspace */
    QString         _error;                         /** Error message, if any error occurs during the loading process */

    DataHierarchyLoadContext    _dataHierarchyLoadContext;      /** Context for loading the data hierarchy, which contains the dataset entries to be loaded in the correct order based on their dependencies */
    WorkspaceLoadContext        _workspaceLoadContext;          /** Context for loading the workspace, which contains the workspace load data */
    DatasetsLoadContext         _datasetsLoadContext;           /** Context for loading the datasets, which contains the dataset entries to be loaded in the correct order based on their dependencies */
};

/** Storage for project load context */
using ProjectLoadContextStorage = QtTaskTree::Storage<ProjectLoadContext>;
