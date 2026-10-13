// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "DeveloperMenu.h"
#include "GifRecorder.h"
#include "ParallelPhantomTestSuite.h"

#include <CoreInterface.h>
#include <exception/ManiVaultException.h>
#include <util/Exception.h>
#include <util/StyledIcon.h>

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include <cstdlib>

using namespace mv;

DeveloperMenu::DeveloperMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent)
{
    setTitle("Dev");
    setToolTip("Development and integration test tools");

    _gifRecorder = std::make_unique<GifRecorder>(parentWidget(), this);
    auto gifRecordingMenu = addMenu(util::StyledIcon("video"), tr("Behavioral testing"));
    _gifRecordingAction = gifRecordingMenu->addAction(util::StyledIcon("record-vinyl"), tr("Start GIF recording…"));
    _gifRecordingAction->setToolTip(tr("Capture the ManiVault Studio main window as an animated GIF"));
    connect(_gifRecordingAction, &QAction::triggered, this, &DeveloperMenu::toggleGifRecording);
    connect(_gifRecorder.get(), &GifRecorder::recordingStarted, this, [this] {
        mv::help().addNotification(tr("GIF recording started"), tr("The ManiVault Studio main window is being recorded at 8 FPS."), util::StyledIcon("circle-dot"));
    });
    connect(_gifRecorder.get(), &GifRecorder::recordingFinished, this, [this](const QString& path, int frameCount, qint64 durationMs, qint64 fileSize) {
        _gifRecordingAction->setText(tr("Start GIF recording…"));
        mv::help().addNotification(tr("GIF recording saved"), tr("Saved %1 (%2 frames, %3 ms, %4 bytes).").arg(path).arg(frameCount).arg(durationMs).arg(fileSize), util::StyledIcon("check"));
    });
    connect(_gifRecorder.get(), &GifRecorder::recordingFailed, this, [this](const QString& error) {
        _gifRecordingAction->setText(tr("Start GIF recording…"));
        mv::help().addNotification(tr("GIF recording failed"), error, util::StyledIcon("circle-exclamation"));
    });

    auto workflowTestingMenu = addMenu(util::StyledIcon("diagram-project"), tr("Workflow testing"));

    workflowTestingMenu->setToolTip(tr("Run parallel workflow test scenarios"));

    detail::ParallelPhantomTestSuite::populateMenu(*workflowTestingMenu, parentWidget());

    auto errorReportingMenu = addMenu(util::StyledIcon("bug"), tr("Error reporting testing"));
    auto handledExceptionAction = errorReportingMenu->addAction(util::StyledIcon("triangle-exclamation"), tr("Handled exception"));

    handledExceptionAction->setToolTip(tr("Create a safe handled exception and show the normal exception dialog"));

    connect(handledExceptionAction, &QAction::triggered, this, [this] {
        testHandledException();
    });

#ifdef MV_USE_ERROR_LOGGING
    auto fatalCrashAction = errorReportingMenu->addAction(util::StyledIcon("skull-crossbones"), tr("Fatal crash..."));

    fatalCrashAction->setToolTip(tr("Deliberately crash ManiVault Studio to test Crashpad and crash feedback"));

    connect(fatalCrashAction, &QAction::triggered, this, [this] {
        testFatalCrash();
    });
#endif
}

DeveloperMenu::~DeveloperMenu() = default;

void DeveloperMenu::toggleGifRecording()
{
    if (_gifRecorder->isRecording()) {
        QString error;
        if (_gifRecorder->stop(error))
            _gifRecordingAction->setText(tr("Encoding GIF recording…"));
        return;
    }

    if (_gifRecorder->state() != GifRecorder::State::Idle)
        return;

    auto outputPath = QFileDialog::getSaveFileName(parentWidget(), tr("Save GIF recording"), QString(), tr("Animated GIF (*.gif)"));
    if (outputPath.isEmpty())
        return;
    if (!outputPath.endsWith(QStringLiteral(".gif"), Qt::CaseInsensitive))
        outputPath += QStringLiteral(".gif");

    QString error;
    if (!_gifRecorder->start(outputPath, error))
        mv::help().addNotification(tr("GIF recording failed"), error, util::StyledIcon("circle-exclamation"));
    else
        _gifRecordingAction->setText(tr("Stop GIF recording"));
}

void DeveloperMenu::testHandledException()
{
    try {
        const auto technicalReason = QString("Simulated handled exception containing privacy test values: home=%1, temp=%2, email=test.user@example.com, url=https://test-user:test-password@example.com/private.").arg(QDir::homePath(), QDir::tempPath());

        throw ManiVaultException(util::SeverityLevel::Error, tr("This is a simulated handled exception. The application can continue normally."), technicalReason, QString("Handled exception simulator at %1").arg(QDir::homePath()), { { "test_only", true } });
    } catch (const ManiVaultException& exception) {
        util::exceptionMessageBox(tr("Handled exception reporting test"), exception, parentWidget());
    }
}

void DeveloperMenu::testFatalCrash()
{
#ifdef MV_USE_ERROR_LOGGING
    const auto& errorManager = mv::errors();

    if (!errorManager.getLoggingUserHasOptedAction().isChecked() || !errorManager.getLoggingEnabledAction().isChecked()) {
        QMessageBox::information(parentWidget(), tr("Crash reporting test"), tr("Sentry error reporting must be enabled before running the crash reporting test."));
        return;
    }

    QMessageBox confirmationDialog(parentWidget());

    confirmationDialog.setWindowIcon(util::StyledIcon("bug"));
    confirmationDialog.setWindowTitle(tr("Crash reporting test"));
    confirmationDialog.setText(tr("This test will deliberately crash ManiVault Studio. Any unsaved work will be lost.\n\nImmediately after the crash, a separate crash feedback dialog should appear. Continue?"));
    confirmationDialog.setIcon(QMessageBox::Warning);
    confirmationDialog.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    confirmationDialog.setDefaultButton(QMessageBox::Cancel);

    if (confirmationDialog.exec() == QMessageBox::Yes)
        std::abort();
#endif
}
