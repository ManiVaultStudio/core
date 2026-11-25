// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TemporaryDirectoriesSettingsAction.h"
#include "Application.h"
#include "CoreInterface.h"

#include "util/StyledIcon.h"
#include "util/Miscellaneous.h"
#include "util/ProjectResourceStagingPolicy.h"

#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QStorageInfo>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <winioctl.h>
#endif

namespace mv::gui
{

namespace
{

enum class StorageSeekPenalty
{
    Unknown,
    Present,
    Absent
};

StorageSeekPenalty getStorageSeekPenalty(const QString& directory)
{
#ifdef Q_OS_WIN
    const auto nativePath = QDir::toNativeSeparators(QFileInfo(directory).absoluteFilePath());
    wchar_t volumePath[MAX_PATH] = {};
    if (!GetVolumePathNameW(reinterpret_cast<LPCWSTR>(nativePath.utf16()), volumePath, MAX_PATH))
        return StorageSeekPenalty::Unknown;

    // The volume handle form below is available for drive-letter volumes. For
    // mounted folders and other volume layouts, leave the classification unknown.
    if (volumePath[0] == L'\0' || volumePath[1] != L':')
        return StorageSeekPenalty::Unknown;

    const auto volumeDevice = QStringLiteral("\\\\.\\%1").arg(QString::fromWCharArray(volumePath, 2));
    const auto volumeHandle = CreateFileW(reinterpret_cast<LPCWSTR>(volumeDevice.utf16()), 0,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, 0, nullptr);
    if (volumeHandle == INVALID_HANDLE_VALUE)
        return StorageSeekPenalty::Unknown;

    STORAGE_PROPERTY_QUERY query{};
    query.PropertyId = StorageDeviceSeekPenaltyProperty;
    query.QueryType = PropertyStandardQuery;

    DEVICE_SEEK_PENALTY_DESCRIPTOR descriptor{};
    DWORD bytesReturned = 0;
    const auto succeeded = DeviceIoControl(volumeHandle, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query),
        &descriptor, sizeof(descriptor), &bytesReturned, nullptr);
    CloseHandle(volumeHandle);

    if (!succeeded || bytesReturned < sizeof(descriptor))
        return StorageSeekPenalty::Unknown;

    return descriptor.IncursSeekPenalty ? StorageSeekPenalty::Present : StorageSeekPenalty::Absent;
#else
    Q_UNUSED(directory);
    return StorageSeekPenalty::Unknown;
#endif
}

QString describeStorageSeekPenalty(const StorageSeekPenalty seekPenalty)
{
    switch (seekPenalty) {
        case StorageSeekPenalty::Present:
            return "Windows reports a disk seek penalty (likely a hard disk drive).";
        case StorageSeekPenalty::Absent:
            return "Windows reports no disk seek penalty (typically an SSD or flash storage).";
        case StorageSeekPenalty::Unknown:
            return "Drive type could not be determined.";
    }

    return "Drive type could not be determined.";
}

}

TemporaryDirectoriesSettingsAction::TemporaryDirectoriesSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Temporary"),
    _applicationTemporaryDirAction(this, "Application temporary directory", Application::current()->getTemporaryDirRoot(), QDir::tempPath()),
    _defaultProjectResourceStagingPolicyAction(this, "Default resource staging policy for new projects", { "Read ranges on demand", "Stage on first read", "Stage immediately" }, "Read ranges on demand"),
    _removeStaleTemporaryDirsAtStartupAction(this, "Remove stale temporary directories at startup", true),
    _checkTemporaryStoragePeriodicallyAction(this, "Check temporary storage periodically", false),
    _lowTemporaryStorageThresholdAction(this, "Warn when free space is below", 1, 1000, 10),
    _temporaryStorageStatusAction(this),
    _scanForStaleTemporaryDirectoriesAction(this, "Refresh"),
    _selectStaleTemporaryDirectoriesAction(this, "Stale temporary directories"),
    _removeStaleTemporaryDirectoriesAction(this, "Remove all stale"),
    _staleTemporaryDirectoriesGroupAction(this, "Stale temporary directories")
{
    setIconByName("hard-drive");

    _applicationTemporaryDirAction.setToolTip("Application temporary directory root. Changes take effect after restarting ManiVault Studio.");
    _applicationTemporaryDirAction.setDescription(
        "<p>Root folder for temporary data managed by ManiVault Studio, including staged project resources.</p>"
        "<p>Changing this folder does not move existing temporary data. Restart ManiVault Studio for the new location to be used.</p>");
    _defaultProjectResourceStagingPolicyAction.setToolTip("Initial staging policy copied into each new project. Changing this setting does not affect existing projects.");
    _defaultProjectResourceStagingPolicyAction.setDescription(
        "<p>Sets the initial policy copied into a project when it is created. Each project then stores its own policy, and datasets can override it.</p>"
        "<ul><li><b>Read ranges on demand:</b> read requested data directly from the archive without extracting full resource files.</li>"
        "<li><b>Stage on first read:</b> extract each resource to the temporary directory when it is first accessed.</li>"
        "<li><b>Stage immediately:</b> extract resources while the project opens, which can increase opening time and temporary disk use.</li></ul>"
        "<p>Changing this setting affects projects created afterwards. Existing projects are unchanged.</p>");
    _checkTemporaryStoragePeriodicallyAction.setToolTip("Periodically check free space on the application temporary directory's drive and notify when it falls below the threshold.");
    _checkTemporaryStoragePeriodicallyAction.setDescription(
        "<p>When enabled, ManiVault Studio checks the free space on the temporary directory's drive every 15 minutes and shows a warning if it falls below the configured threshold.</p>");
    _lowTemporaryStorageThresholdAction.setSuffix(" GiB");
    _lowTemporaryStorageThresholdAction.setDefaultWidgetFlags(IntegralAction::SpinBox | IntegralAction::Slider);
    _lowTemporaryStorageThresholdAction.setToolTip("Show a low-storage warning when available space on the application temporary directory's drive falls below this amount.");
    _lowTemporaryStorageThresholdAction.setDescription(
        "<p>Sets the free-space level that triggers a low-storage warning for the drive containing the application temporary directory. The periodic check must be enabled for automatic warnings.</p>");
    _temporaryStorageStatusAction.setDisplayMode(MessageCardAction::DisplayMode::Detailed);

    _temporaryStorageCheckTimer.setInterval(15 * 60 * 1000);
    connect(&_temporaryStorageCheckTimer, &QTimer::timeout, this, [this] { checkTemporaryStorage(); });
    connect(&_checkTemporaryStoragePeriodicallyAction, &ToggleAction::toggled, this, [this](const bool enabled) {
        if (enabled) {
            _temporaryStorageCheckTimer.start();
            _lowStorageWarningPosted = false;
            if (_startupStorageCheckComplete)
                checkTemporaryStorage();
        } else {
            _temporaryStorageCheckTimer.stop();
            _lowStorageWarningPosted = false;
        }
    });
    connect(&_lowTemporaryStorageThresholdAction, &IntegralAction::valueChanged, this, [this](const std::int32_t&) {
        if (_startupStorageCheckComplete) {
            if (_checkTemporaryStoragePeriodicallyAction.isChecked())
                checkTemporaryStorage();
            else
                updateTemporaryStorageStatus();
        }
    });

    connect(&_applicationTemporaryDirAction, &TemporaryDirectoryAction::directoryChanged, this, [this](const QString& directory) {
        // Settings can be restored before the notification UI and main window are ready.
        // The startup check will inspect the active and configured paths once initialization
        // has completed; only user changes made afterwards need an immediate notification.
        if (_startupStorageCheckComplete) {
            checkTemporaryStorage(directory, true, true, true);
            if (getStorageSeekPenalty(directory) == StorageSeekPenalty::Present) {
                scheduleTemporaryStorageNotification(
                    "Temporary directory may be slow",
                    QString("Windows reports a disk seek penalty for <code>%1</code>, which usually means it is a hard disk drive. Large imports and staging workloads may run more slowly there; an SSD or NVMe location may improve performance. Drive type detection can be inconclusive for some storage devices.")
                        .arg(directory.toHtmlEscaped()),
                    util::SeverityLevel::Warning);
            }
        }
        _scanForStaleTemporaryDirectoriesAction.trigger();
    });

    addAction(&_applicationTemporaryDirAction);
    addAction(&_defaultProjectResourceStagingPolicyAction);
    addAction(&_removeStaleTemporaryDirsAtStartupAction);
    addAction(&_checkTemporaryStoragePeriodicallyAction);
    addAction(&_lowTemporaryStorageThresholdAction);
    addAction(&_temporaryStorageStatusAction);
    addAction(&_staleTemporaryDirectoriesGroupAction);

    connect(&_defaultProjectResourceStagingPolicyAction, &OptionAction::currentTextChanged, this, [](const QString& policyName) {
        const auto policy = util::projectResourceStagingPolicyFromName(policyName);
        QString effect;

        switch (policy) {
            case util::ProjectResourceStagingPolicy::ReadRangesOnDemand:
                effect = "Archive-backed resources are read from the project archive as needed, without copying entire files into staging.";
                break;

            case util::ProjectResourceStagingPolicy::StageOnFirstRead:
                effect = "Each archive-backed resource is copied into temporary storage on its first read; later reads use the staged file.";
                break;

            case util::ProjectResourceStagingPolicy::StageImmediately:
                effect = "All archive-backed resources are copied into temporary storage while the project opens, which can increase open time and disk use.";
                break;

            case util::ProjectResourceStagingPolicy::Inherit:
                effect = "The fallback policy is on-demand reads from the project archive.";
                break;
        }

        const auto description = QString("%1 This policy is copied into projects created after this change. Existing projects and open sessions are unchanged.").arg(effect);
        mv::help().addNotification("Default policy for new projects changed", description, util::StyledIcon("hard-drive"));
    });

    _removeStaleTemporaryDirsAtStartupAction.setToolTip("Remove stale temporary directories when ManiVault starts up");
    _removeStaleTemporaryDirsAtStartupAction.setDescription(
        "<p>Automatically remove temporary directories left behind by earlier ManiVault sessions during startup. Active sessions' temporary directories are not considered stale.</p>");

    _scanForStaleTemporaryDirectoriesAction.setToolTip("Rescan for stale temporary directories");
    _scanForStaleTemporaryDirectoriesAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _scanForStaleTemporaryDirectoriesAction.setIconByName("rotate-right");

    _selectStaleTemporaryDirectoriesAction.setToolTip("Summary of stale temporary directories");
    _selectStaleTemporaryDirectoriesAction.setIconByName("list");
    _selectStaleTemporaryDirectoriesAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _selectStaleTemporaryDirectoriesAction.setDefaultWidgetFlag(OptionsAction::Selection);
    _selectStaleTemporaryDirectoriesAction.setPopupSizeHint(QSize(640, 0));

    _removeStaleTemporaryDirectoriesAction.setToolTip("Remove all stale temporary directories");
    _removeStaleTemporaryDirectoriesAction.setStretch(1);

    _staleTemporaryDirectoriesGroupAction.setShowLabels(false);

    _staleTemporaryDirectoriesGroupAction.addAction(&_scanForStaleTemporaryDirectoriesAction);
    _staleTemporaryDirectoriesGroupAction.addAction(&_selectStaleTemporaryDirectoriesAction);
    _staleTemporaryDirectoriesGroupAction.addAction(&_removeStaleTemporaryDirectoriesAction);

    const auto updateRemoveStaleTemporaryDirectoriesAction = [this]() -> void {
        if (Application::current()->getTemporaryDirs().getStale().isEmpty())
            _removeStaleTemporaryDirectoriesAction.setText("No stale temporary directories");
        else {
            if (_selectStaleTemporaryDirectoriesAction.getSelectedOptions().isEmpty())
                _removeStaleTemporaryDirectoriesAction.setText("No stale temporary directories selected for removal");
            else
                _removeStaleTemporaryDirectoriesAction.setText(QString("Remove %1 stale").arg(QString::number(_selectStaleTemporaryDirectoriesAction.getSelectedOptions().count())));
        }

        _removeStaleTemporaryDirectoriesAction.setToolTip(_removeStaleTemporaryDirectoriesAction.text());
    };

    connect(&_removeStaleTemporaryDirectoriesAction, &TriggerAction::triggered, this, [this]() -> void {
        Application::current()->getTemporaryDirs().removeStale(_selectStaleTemporaryDirectoriesAction.getSelectedOptions());

        _scanForStaleTemporaryDirectoriesAction.trigger();
    });

    connect(&_scanForStaleTemporaryDirectoriesAction, &TriggerAction::triggered, this, [this, updateRemoveStaleTemporaryDirectoriesAction]() -> void {
        const auto staleTemporaryDirectories = Application::current()->getTemporaryDirs().getStale();

        _selectStaleTemporaryDirectoriesAction.setOptions(staleTemporaryDirectories);
        _selectStaleTemporaryDirectoriesAction.setSelectedOptions(staleTemporaryDirectories);
        _selectStaleTemporaryDirectoriesAction.setEnabled(!staleTemporaryDirectories.isEmpty());

        _removeStaleTemporaryDirectoriesAction.setEnabled(!_selectStaleTemporaryDirectoriesAction.getSelectedOptions().isEmpty());

        updateRemoveStaleTemporaryDirectoriesAction();
    });
    
    connect(&_selectStaleTemporaryDirectoriesAction, &OptionsAction::selectedOptionsChanged, this, [this, updateRemoveStaleTemporaryDirectoriesAction]() -> void {
        _removeStaleTemporaryDirectoriesAction.setEnabled(!_selectStaleTemporaryDirectoriesAction.getSelectedOptions().isEmpty());

        updateRemoveStaleTemporaryDirectoriesAction();
    });

    _scanForStaleTemporaryDirectoriesAction.trigger();
}

void TemporaryDirectoriesSettingsAction::checkTemporaryStorageAtStartup()
{
    constexpr auto onboardingSetting = "GlobalSettings/Temporary/TemporaryStorageNoticeShownV2";
    QSettings settings;
    const auto showLocationNotice = !settings.value(onboardingSetting, false).toBool();

    if (_checkTemporaryStoragePeriodicallyAction.isChecked() && !_temporaryStorageCheckTimer.isActive())
        _temporaryStorageCheckTimer.start();

    const auto notifyLowSpace = showLocationNotice || _checkTemporaryStoragePeriodicallyAction.isChecked();
    const auto directory = Application::current()->getTemporaryDirRoot();
    checkTemporaryStorage(directory, showLocationNotice, notifyLowSpace, false);
    _startupStorageCheckComplete = true;
    if (showLocationNotice)
        settings.setValue(onboardingSetting, true);
}

void TemporaryDirectoriesSettingsAction::checkTemporaryStorage()
{
    const auto temporaryDirectory = Application::current()->getTemporaryDirRoot();
    checkTemporaryStorage(temporaryDirectory, false, _checkTemporaryStoragePeriodicallyAction.isChecked(), false);
}

void TemporaryDirectoriesSettingsAction::checkTemporaryStorage(const QString& directory, const bool notifyWhenHealthy, const bool alwaysNotify, const bool takesEffectAfterRestart)
{
    constexpr std::uint64_t bytesPerGiB = 1024ULL * 1024ULL * 1024ULL;
    const auto thresholdBytes = static_cast<std::uint64_t>(_lowTemporaryStorageThresholdAction.getValue()) * bytesPerGiB;

    QStorageInfo storage(directory);
    storage.refresh();

    const auto canCheckSpace = storage.isValid() && storage.isReady() && storage.bytesAvailable() >= 0;
    if (!canCheckSpace) {
        if (notifyWhenHealthy || alwaysNotify) {
            scheduleTemporaryStorageNotification(
                "Unable to check application temporary storage",
                QString("Free space could not be checked for <code>%1</code>. <a href=\"app://settings/temporary\">Open Temporary settings</a>.%2")
                    .arg(directory.toHtmlEscaped(), takesEffectAfterRestart ? QString(" This configured directory will be used after restarting ManiVault Studio.") : QString()),
                util::SeverityLevel::Warning);
        }

        updateTemporaryStorageStatus();
        return;
    }

    const auto availableBytes = static_cast<std::uint64_t>(storage.bytesAvailable());
    const auto isLow = availableBytes < thresholdBytes;
    const auto isActiveDirectory = QDir::cleanPath(directory) == QDir::cleanPath(Application::current()->getTemporaryDirRoot());
    const auto shouldNotify = isLow ?
        (takesEffectAfterRestart || ((alwaysNotify || notifyWhenHealthy) && !_lowStorageWarningPosted)) :
        notifyWhenHealthy;

    if (isActiveDirectory) {
        if (availableBytes >= thresholdBytes + bytesPerGiB)
            _lowStorageWarningPosted = false;
        else if (isLow && shouldNotify)
            _lowStorageWarningPosted = true;
    }

    if (shouldNotify) {
        const auto restartDescription = takesEffectAfterRestart ? QString(" This configured directory will be used after restarting ManiVault Studio.") : QString();
        const auto title = isLow ? QString("Low application temporary storage") : QString("Application temporary storage");
        const auto description = isLow ?
            QString("The directory <code>%1</code> has %2 available, below the configured warning threshold of %3. Large imports may need more free space. <a href=\"app://settings/temporary\">Open Temporary settings</a>.%4")
                .arg(directory.toHtmlEscaped(), util::getNoBytesHumanReadable(availableBytes), util::getNoBytesHumanReadable(thresholdBytes), restartDescription) :
            QString("The directory <code>%1</code> has %2 available, above the configured warning threshold of %3. <a href=\"app://settings/temporary\">Open Temporary settings</a>.%4")
                .arg(directory.toHtmlEscaped(), util::getNoBytesHumanReadable(availableBytes), util::getNoBytesHumanReadable(thresholdBytes), restartDescription);

        scheduleTemporaryStorageNotification(title, description, isLow ? util::SeverityLevel::Warning : util::SeverityLevel::Info);
    }

    updateTemporaryStorageStatus();
}

void TemporaryDirectoriesSettingsAction::scheduleTemporaryStorageNotification(const QString& title, const QString& description, const util::SeverityLevel severity)
{
    QTimer::singleShot(2500, this, [title, description, severity] {
        mv::help().addNotification(title, description, util::StyledIcon("hard-drive"), util::Notification::DurationType::Calculated, 0, severity);
    });
}

void TemporaryDirectoriesSettingsAction::refreshTemporaryStorageStatus()
{
    updateTemporaryStorageStatus();
}

void TemporaryDirectoriesSettingsAction::updateTemporaryStorageStatus()
{
    constexpr std::uint64_t bytesPerGiB = 1024ULL * 1024ULL * 1024ULL;
    const auto thresholdBytes = static_cast<std::uint64_t>(_lowTemporaryStorageThresholdAction.getValue()) * bytesPerGiB;
    const auto activeDirectory = Application::current()->getTemporaryDirRoot();
    const auto configuredDirectory = _applicationTemporaryDirAction.getDirectory();
    const auto hasPendingDirectory = QDir::cleanPath(configuredDirectory) != QDir::cleanPath(activeDirectory);
    bool anyLow = false;
    bool anyUnknown = false;
    bool anySeekPenalty = false;

    const auto describeDirectory = [&](const QString& directory, const QString& label) {
        const auto seekPenalty = getStorageSeekPenalty(directory);
        anySeekPenalty = anySeekPenalty || seekPenalty == StorageSeekPenalty::Present;
        const auto driveDescription = describeStorageSeekPenalty(seekPenalty);

        QStorageInfo storage(directory);
        storage.refresh();
        if (!storage.isValid() || !storage.isReady() || storage.bytesAvailable() < 0) {
            anyUnknown = true;
            return QString("%1: <code>%2</code> — free space unavailable; %3").arg(label, directory.toHtmlEscaped(), driveDescription);
        }

        const auto availableBytes = static_cast<std::uint64_t>(storage.bytesAvailable());
        const auto isLow = availableBytes < thresholdBytes;
        anyLow = anyLow || isLow;
        const auto state = isLow ? QString("below warning threshold") : QString("above warning threshold");
        return QString("%1: <code>%2</code> — %3 available (%4; threshold %5); %6")
            .arg(label, directory.toHtmlEscaped(), util::getNoBytesHumanReadable(availableBytes), state, util::getNoBytesHumanReadable(thresholdBytes), driveDescription);
    };

    auto description = describeDirectory(activeDirectory, "Active temporary directory");
    if (hasPendingDirectory)
        description += QString("<br>%1").arg(describeDirectory(configuredDirectory, "Configured for next launch"));

    MessageCardAction::ButtonActions buttonActions;
    buttonActions.append({ "Check now", [this] {
        checkTemporaryStorage(Application::current()->getTemporaryDirRoot(), false, true, false);
    } });

    _temporaryStorageStatusAction.setMessage(
        anyLow || anyUnknown || anySeekPenalty ? util::SeverityLevel::Warning : util::SeverityLevel::Info,
        "Temporary storage",
        description,
        buttonActions);
}

}
