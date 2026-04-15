// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/WorkflowContextBase.h>

using UniqueTemporaryDir = std::unique_ptr<QTemporaryDir>;

struct ProjectOpenContext : public WorkflowContextBase
{
    explicit ProjectOpenContext(const QString& filePath) :
		_filePath(filePath),
        _temporaryDirectory(new QTemporaryDir(QDir::cleanPath(mv::Application::current()->getTemporaryDir().path() + QDir::separator() + "OpenProject")))
    {
    }

    QString             _filePath;
    bool                _loadWorkspace = true;
    bool                _importDataOnly = false;
    bool                _disableReadOnly = false;
    UniqueTemporaryDir  _temporaryDirectory;
    QString             _temporaryDirectoryPath;
    QString             _workspaceJsonPath;
    QString             _projectJsonPath;
};
