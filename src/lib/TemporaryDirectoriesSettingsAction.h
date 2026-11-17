// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/file/TemporaryDirectoryAction.h"
#include "actions/ToggleAction.h"
#include "actions/OptionAction.h"
#include "actions/OptionsAction.h"
#include "actions/TriggerAction.h"
#include "actions/HorizontalGroupAction.h"
#include "actions/IntegralAction.h"
#include "actions/MessageCardAction.h"

#include <QTimer>

namespace mv::gui
{

/**
 * @brief Global settings for application temporary storage.
 *
 * In addition to the application temporary directory and storage checks, this group
 * contains the default staging policy for archive-backed project resources. Projects
 * that inherit the application default use this policy when they are opened.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT TemporaryDirectoriesSettingsAction final : public GlobalSettingsGroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    TemporaryDirectoriesSettingsAction(QObject* parent);

    /** Checks application temporary storage at startup and shows a first-run location notice. */
    void checkTemporaryStorageAtStartup();

    /** Refreshes the free-space status shown in Temporary settings. */
    void refreshTemporaryStorageStatus();

public: // Action getters

    TemporaryDirectoryAction& getApplicationTemporaryDirAction() { return _applicationTemporaryDirAction; }
    ToggleAction& getRemoveStaleTemporaryDirsAtStartupAction() { return _removeStaleTemporaryDirsAtStartupAction; }
    /**
     * @brief Gets the default policy for staging archive-backed project resources.
     * @return Application-wide policy used by projects that inherit the default
     */
    OptionAction& getDefaultProjectResourceStagingPolicyAction() { return _defaultProjectResourceStagingPolicyAction; }
    TriggerAction& getScanForStaleTemporaryDirectoriesAction() { return _scanForStaleTemporaryDirectoriesAction; }
    OptionsAction& getStaleTemporaryDirectoriesAction() { return _selectStaleTemporaryDirectoriesAction; }
    TriggerAction& getRemoveStaleTemporaryDirectoriesAction() { return _removeStaleTemporaryDirectoriesAction; }
    HorizontalGroupAction& getStaleTemporaryDirectoriesGroupAction() { return _staleTemporaryDirectoriesGroupAction; }
    ToggleAction& getCheckTemporaryStoragePeriodicallyAction() { return _checkTemporaryStoragePeriodicallyAction; }
    IntegralAction& getLowTemporaryStorageThresholdAction() { return _lowTemporaryStorageThresholdAction; }

private:

    /** Checks the application temporary directory and warns on a low-space transition. */
    void checkTemporaryStorage();

    /** Checks \p directory, optionally notifying for healthy status and treating it as a newly configured path. */
    void checkTemporaryStorage(const QString& directory, bool notifyWhenHealthy, bool alwaysNotify, bool takesEffectAfterRestart);

    /** Queues a temporary-storage notification until startup notification services are ready. */
    void scheduleTemporaryStorageNotification(const QString& title, const QString& description, util::SeverityLevel severity);

    /** Updates the settings status card using the active and configured temporary directories. */
    void updateTemporaryStorageStatus();

private:
    TemporaryDirectoryAction _applicationTemporaryDirAction;           /** Temporary directory picker and reset action */
    /** Application-wide default for archive-backed project resource staging. */
    OptionAction             _defaultProjectResourceStagingPolicyAction;
    ToggleAction             _removeStaleTemporaryDirsAtStartupAction; /** Turn on/off the automatic removal of the application temporary directories at application startup */
    ToggleAction             _checkTemporaryStoragePeriodicallyAction; /** Enables periodic checks for low free space on application temporary storage */
    IntegralAction           _lowTemporaryStorageThresholdAction;      /** Free-space threshold, in GiB, for low-storage warnings */
    MessageCardAction        _temporaryStorageStatusAction;             /** Free-space status for active and pending application temporary directories */
    TriggerAction             _scanForStaleTemporaryDirectoriesAction;  /** Action which scans for stale temporary directories */
    OptionsAction             _selectStaleTemporaryDirectoriesAction;   /** Action which lists stale temporary directories */
    TriggerAction             _removeStaleTemporaryDirectoriesAction;   /** Action which triggers the cleanup of stale temporary directories */
    HorizontalGroupAction     _staleTemporaryDirectoriesGroupAction;    /** Group action for working with stale temporary directories */
    QTimer                    _temporaryStorageCheckTimer;              /** Periodic free-space check timer */
    bool                      _lowStorageWarningPosted = false;         /** Whether a low-storage warning was posted during the current low-space period */
    bool                      _startupStorageCheckComplete = false;     /** Whether the delayed startup storage check has completed */
};

}
