// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/WorkflowContextBase.h>

struct ProjectOpenContext : public WorkflowContextBase
{
    QString             _filePath;
    bool                _loadWorkspace = true;
    bool                _importDataOnly = false;
    bool                _disableReadOnly = false;
    UniqueTemporaryDir  _temporaryDirectory;
    QString             _temporaryDirectoryPath;
    QString             _workspaceJsonPath;
    QString             _projectJsonPath;
};

using ProjectOpenContextStorage = QtTaskTree::Storage<ProjectOpenContext>;