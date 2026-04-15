// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/WorkflowContextBase.h>

#include "Application.h"
#include "Archiver.h"

using UniqueTemporaryDir = std::unique_ptr<QTemporaryDir>;

struct ProjectSaveContext : public WorkflowContextBase
{
    explicit ProjectSaveContext(const QString& filePath) :
		_filePath(filePath),
		_temporaryDirectory(new QTemporaryDir(QDir::cleanPath(mv::Application::current()->getTemporaryDir().path() + QDir::separator() + "SaveProject")))
    {
    }

    QString             _filePath;              /** Path to the project file */
    QString             _workspaceJsonPath;     /** Path to the workspace JSON file */
    QString             _projectJsonPath;       /** Path to the project JSON file */
    QString             _metaJsonPath;          /** Path to the project meta JSON file */
    UniqueTemporaryDir  _temporaryDirectory;    /** Temporary directory for saving the project */
    mv::util::Archiver  _archiver;              /** Archiver for handling the project archive */
};
