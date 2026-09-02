// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "DeveloperMenu.h"
#include "ParallelPhantomTestSuite.h"

#include <CoreInterface.h>
#include <exception/ManiVaultException.h>
#include <util/Exception.h>
#include <util/StyledIcon.h>

#include <QDir>
#include <QMessageBox>

#include <cstdlib>

using namespace mv;

DeveloperMenu::DeveloperMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent)
{
    setTitle("Dev");
    setToolTip("Development and integration test tools");

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
