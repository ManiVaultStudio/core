// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TemporaryFilesSettingsAction.h"
#include "Application.h"

using namespace mv::gui;

namespace mv
{

TemporaryFilesSettingsAction::TemporaryFilesSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Temporary Files"),
    _applicationTemporaryDirAction(this, "Application temporary directory", Application::current()->getTemporaryDir().path()),
    _staleTemporaryDirectoriesAction(this, "Stale temporary directories"),
    _removeStaleTemporaryDirectoriesAction(this, "Remove stale temporary directories"),
    _staleTemporaryDirectoriesGroupAction(this, "Stale temporary directories")
{
    _applicationTemporaryDirAction.setEnabled(false);

    addAction(&_staleTemporaryDirectoriesGroupAction);

    _staleTemporaryDirectoriesGroupAction.addAction(&_staleTemporaryDirectoriesAction);
    _staleTemporaryDirectoriesGroupAction.addAction(&_removeStaleTemporaryDirectoriesAction);

    connect(&_removeStaleTemporaryDirectoriesAction, &TriggerAction::triggered, this, []() -> void { Application::current()->removeStaleTemporaryDirectories(); });
}

}
