// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TemporaryDirectoriesSettingsAction.h"
#include "Application.h"

using namespace mv::gui;

namespace mv
{

TemporaryDirectoriesSettingsAction::TemporaryDirectoriesSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Temporary Files"),
    _applicationTemporaryDirAction(this, "Application temporary directory", Application::current()->getTemporaryDir().path()),
    _rescanForStaleTemporaryDirectoriesAction(this, "Refresh"),
    _selectStaleTemporaryDirectoriesAction(this, "Stale temporary directories"),
    _removeStaleTemporaryDirectoriesAction(this, "Remove all stale"),
    _staleTemporaryDirectoriesGroupAction(this, "Stale temporary directories")
{
    _applicationTemporaryDirAction.setEnabled(false);

    addAction(&_applicationTemporaryDirAction);
    addAction(&_staleTemporaryDirectoriesGroupAction);

    _rescanForStaleTemporaryDirectoriesAction.setToolTip("Rescan for stale temporary directories");
    _rescanForStaleTemporaryDirectoriesAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _rescanForStaleTemporaryDirectoriesAction.setIconByName("redo");

    _selectStaleTemporaryDirectoriesAction.setToolTip("Summary of stale temporary directories");
    _selectStaleTemporaryDirectoriesAction.setIconByName("list");
    _selectStaleTemporaryDirectoriesAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _selectStaleTemporaryDirectoriesAction.setDefaultWidgetFlag(OptionsAction::Selection);
    _selectStaleTemporaryDirectoriesAction.setPopupSizeHint(QSize(640, 100));

    _removeStaleTemporaryDirectoriesAction.setToolTip("Remove all stale temporary directories");
    _removeStaleTemporaryDirectoriesAction.setStretch(1);

    _staleTemporaryDirectoriesGroupAction.setShowLabels(false);

    _staleTemporaryDirectoriesGroupAction.addAction(&_rescanForStaleTemporaryDirectoriesAction);
    _staleTemporaryDirectoriesGroupAction.addAction(&_selectStaleTemporaryDirectoriesAction);
    _staleTemporaryDirectoriesGroupAction.addAction(&_removeStaleTemporaryDirectoriesAction);

    const auto updateStaleTemporaryDirectories = [this]() -> void {
        const auto staleTemporaryDirectories = Application::current()->getTemporaryDirectories().getStale();

        if (staleTemporaryDirectories != _selectStaleTemporaryDirectoriesAction.getOptions()) {
            _selectStaleTemporaryDirectoriesAction.setOptions(staleTemporaryDirectories);
            _selectStaleTemporaryDirectoriesAction.setSelectedOptions(staleTemporaryDirectories);
        }

        _selectStaleTemporaryDirectoriesAction.setEnabled(!staleTemporaryDirectories.isEmpty());
        _removeStaleTemporaryDirectoriesAction.setEnabled(!_selectStaleTemporaryDirectoriesAction.getSelectedOptions().isEmpty());
    };

    updateStaleTemporaryDirectories();

    connect(&_removeStaleTemporaryDirectoriesAction, &TriggerAction::triggered, this, [this, updateStaleTemporaryDirectories]() -> void {
        Application::current()->getTemporaryDirectories().removeStale(_selectStaleTemporaryDirectoriesAction.getSelectedOptions());

        updateStaleTemporaryDirectories();
    });

    connect(&_rescanForStaleTemporaryDirectoriesAction, &TriggerAction::triggered, this, updateStaleTemporaryDirectories);
    connect(&_selectStaleTemporaryDirectoriesAction, &OptionsAction::selectedOptionsChanged, this, updateStaleTemporaryDirectories);
}

}
