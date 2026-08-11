// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ErrorManager.h"
#include "ErrorLoggingConsentDialog.h"
#include "AbstractErrorLogger.h"

#include <QRegularExpression>

#ifdef MV_ENABLE_CPPTRACE
#include <cpptrace/cpptrace.hpp>
#endif
using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
    //#define ERROR_MANAGER_VERBOSE
#endif

#ifdef MV_USE_ERROR_LOGGING
    #include "private/SentryErrorLogger.h"
#endif

namespace mv
{

ErrorManager::ErrorManager(QObject* parent) :
    AbstractErrorManager(parent),
    _loggingAskConsentDialogAction(this, "Consent..."),
    _loggingUserHasOptedAction(this, "User has opted", false),
    _loggingEnabledAction(this, "Toggle error reporting", false),
    _loggingDsnAction(this, "Sentry DSN", "https://211289c773dcc267b1bb536b6c3a23f7@lkebsentry.nl/2"),
    _loggingReportHandledExceptionsAction(this, "Report handled exceptions", false),
    _loggingShowCrashReportDialogAction(this, "Ask for feedback after a crash", true)
{
    _loggingAskConsentDialogAction.setToolTip("Show the error logging consent dialog");
    _loggingAskConsentDialogAction.setDefaultWidgetFlags(TriggerAction::IconText);

    _loggingUserHasOptedAction.setSettingsPrefix(QString("%1Logging/UserHasOpted").arg(getSettingsPrefix()));
    _loggingUserHasOptedAction.setToolTip("Whether the user has opted in or out");

    _loggingEnabledAction.setSettingsPrefix(QString("%1Logging/Enabled").arg(getSettingsPrefix()));
    _loggingEnabledAction.setToolTip("Toggle Sentry error logging");

//#ifdef _DEBUG
//    _loggingEnabledAction.setEnabled(false);
//#endif
    
    _loggingDsnAction.setSettingsPrefix(QString("%1Logging/DSN").arg(getSettingsPrefix()));
    _loggingDsnAction.setToolTip("The Sentry Data Source Name (DSN). Hosted and self-hosted HTTP(S) DSNs with optional ports and path prefixes are supported.");

    // Current Sentry DSNs do not require a 32-character hexadecimal public
    // key or a numeric project identifier. Keep this structural validator in
    // line with sentry-native: scheme, public key, optional secret, host and
    // optional port/path prefix, followed by a non-empty project identifier.
    _loggingDsnAction.getValidator().setRegularExpression(QRegularExpression(R"(^https?://[A-Z0-9._~%-]+(?::[A-Z0-9._~%-]+)?@(?:\[[0-9A-F:.]+\]|[A-Z0-9.-]+)(?::[0-9]{1,5})?/(?:[A-Z0-9._~!$&'()*+,;=:@%-]+/)*[A-Z0-9._~!$&'()*+,;=:@%-]+/?$)", QRegularExpression::CaseInsensitiveOption));

    _loggingReportHandledExceptionsAction.setSettingsPrefix(QString("%1Logging/ReportHandledExceptions").arg(getSettingsPrefix()));
    _loggingReportHandledExceptionsAction.setToolTip("Send handled error- and fatal-level exceptions shown in an exception dialog to Sentry. Informational and warning-level exceptions remain local.");

    _loggingShowCrashReportDialogAction.setSettingsPrefix(QString("%1Logging/ShowCrashReportDialog").arg(getSettingsPrefix()));
    _loggingShowCrashReportDialogAction.setToolTip("Ask for optional feedback on the next launch after a crash");

    const auto allowErrorReportingChanged = [this]() -> void {
        _loggingShowCrashReportDialogAction.setEnabled(_loggingEnabledAction.isChecked());
        _loggingReportHandledExceptionsAction.setEnabled(_loggingEnabledAction.isChecked());
        _loggingDsnAction.setEnabled(_loggingEnabledAction.isChecked());
    };

    allowErrorReportingChanged();

    connect(&_loggingEnabledAction, &ToggleAction::toggled, this, allowErrorReportingChanged);
}

ErrorManager::~ErrorManager()
{
    reset();
}

void ErrorManager::initialize()
{
#ifdef ERROR_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractErrorManager::initialize();

    if (isInitialized())
        return;

    beginInitialization();
    {
        auto& errorLoggingSettingsAction = mv::settings().getErrorLoggingSettingsAction();

        errorLoggingSettingsAction.addAction(&getLoggingAskConsentDialogAction());
        errorLoggingSettingsAction.addAction(&getLoggingEnabledAction());
        errorLoggingSettingsAction.addAction(&getLoggingDsnAction());
        errorLoggingSettingsAction.addAction(&getLoggingReportHandledExceptionsAction());
        errorLoggingSettingsAction.addAction(&getLoggingShowCrashReportDialogAction());

#ifdef MV_USE_ERROR_LOGGING
        setErrorLogger(new SentryErrorLogger(this));

        getErrorLogger()->initialize();

        connect(&getLoggingAskConsentDialogAction(), &TriggerAction::triggered, this, &ErrorManager::showErrorLoggingConsentDialog);

        if (!getLoggingUserHasOptedAction().isChecked())
            showErrorLoggingConsentDialog();

        getErrorLogger()->requestStart();
#endif
    }
    endInitialization();
}

void ErrorManager::reset()
{
#ifdef ERROR_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
        if (getErrorLogger())
            getErrorLogger()->requestStop();
    }
    endReset();
}

void ErrorManager::showErrorLoggingConsentDialog()
{
#ifdef MV_USE_ERROR_LOGGING
    ErrorLoggingConsentDialog errorLoggingConsentDialog;
    errorLoggingConsentDialog.exec();
#endif
}

QString ErrorManager::getFormattedDebugStackTrace() const
{
#ifndef MV_ENABLE_CPPTRACE
    return {};
#else
    QStringList lines = QString::fromStdString(cpptrace::generate_trace().to_string()).split('\n', Qt::SkipEmptyParts);

    // Remove the header.
    if (!lines.isEmpty() && lines.first().startsWith("Stack trace"))
        lines.removeFirst();

    // Remove frame numbers (#0, #1, ...).
    static const QRegularExpression frameNumberRegex(R"(^\s*#\d+\s+)");

    for (QString& line : lines)
        line.remove(frameNumberRegex);

    return lines.join('\n');
#endif
}

util::StackTrace ErrorManager::getDebugStackTrace() const
{
    util::StackTrace stackTrace;

#ifdef MV_ENABLE_CPPTRACE
    const auto trace = cpptrace::generate_trace();

    stackTrace.reserve(static_cast<qsizetype>(trace.frames.size()));

    for (const auto& frame : trace.frames) {
        StackFrame stackFrame;

        stackFrame.function = QString::fromStdString(frame.symbol);
        stackFrame.file     = QString::fromStdString(frame.filename);
        stackFrame.module   = {};
        stackFrame.raw      = QString::fromStdString(frame.to_string());
        stackFrame.line     = frame.line.has_value()? static_cast<int>(frame.line.value()) : -1;
        stackFrame.address  = QString("0x%1").arg(static_cast<quintptr>(frame.raw_address), QT_POINTER_SIZE * 2, 16, QLatin1Char('0'));

        stackTrace.push_back(std::move(stackFrame));
    }
#endif

    return stackTrace;
}

void ErrorManager::reportHandledException(const QString& title, const QString& exceptionType, const QString& reason, util::SeverityLevel severity, const util::StackTrace& stackTrace, const QString& diagnosticId, const QString& where)
{
    if (!getLoggingReportHandledExceptionsAction().isChecked())
        return;

    switch (severity) {
        case util::SeverityLevel::Info:
        case util::SeverityLevel::Warning:
            return;

        case util::SeverityLevel::Error:
        case util::SeverityLevel::Fatal:
            break;
    }

    if (auto* errorLogger = getErrorLogger())
        errorLogger->reportHandledException(title, exceptionType, reason, severity, stackTrace, diagnosticId, where);
}

}
