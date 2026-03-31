// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/WorkflowContextBase.h>

#include "Archiver.h"

class ProjectSaveContext : public WorkflowContextBase
{
public:
    QString             _filePath;                  /** Path to the project file */
    UniqueTemporaryDir  _temporaryDirectory;        /** Temporary directory for extracting project files */
    QString             _temporaryDirectoryPath;    /** Path to the temporary directory */
    QString             _workspaceJsonPath;         /** Path to the workspace JSON file */
    QString             _projectJsonPath;           /** Path to the project JSON file */
    mv::util::Archiver  _archiver;                  /** Archiver for handling the project archive */
};

using ProjectSaveContextStorage = QtTaskTree::Storage<ProjectSaveContext>;
