// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/WorkflowContext.h>

struct ProjectSaveContext
{
    QString             _filePath;                  /** Path to the project file */
    UniqueTemporaryDir  _temporaryDirectory;        /** Temporary directory for extracting project files */
    QString             _temporaryDirectoryPath;    /** Path to the temporary directory */
    QString             _workspaceJsonPath;         /** Path to the workspace JSON file */
    QString             _projectJsonPath;           /** Path to the project JSON file */
    QString             _error;                     /** Error message, if any */
};

using ProjectSaveContextStorage = QtTaskTree::Storage<ProjectSaveContext>;
