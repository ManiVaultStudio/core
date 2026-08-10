// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "CrashReportDialog.h"

#include "sentry.h"

#include <QApplication>
#include <QFile>

#include <cstdlib>

namespace {

sentry_envelope_t* readEnvelope(const QString& path)
{
#ifdef Q_OS_WIN
    return sentry_envelope_read_from_filew(
        reinterpret_cast<const wchar_t*>(path.utf16()));
#else
    return sentry_envelope_read_from_file(path.toUtf8().constData());
#endif
}

}

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);

    if (application.arguments().size() != 2)
        return EXIT_FAILURE;

    const auto envelopePath = application.arguments().at(1);
    auto* envelope          = readEnvelope(envelopePath);

    if (!envelope)
        return EXIT_FAILURE;

    const auto dsnValue     = sentry_envelope_get_header(envelope, "dsn");
    const auto eventIdValue = sentry_envelope_get_header(envelope, "event_id");
    const auto dsn          = QByteArray(sentry_value_as_string(dsnValue));
    const auto eventId      = QByteArray(sentry_value_as_string(eventIdValue));

    if (dsn.isEmpty() || eventId.isEmpty()) {
        sentry_envelope_free(envelope);
        return EXIT_FAILURE;
    }

    auto* options = sentry_options_new();

    sentry_options_set_backend(options, nullptr);
    sentry_options_set_dsn(options, dsn.constData());

    if (sentry_init(options) != 0) {
        sentry_envelope_free(envelope);
        return EXIT_FAILURE;
    }

    // The user already opted in to technical crash reporting. Submit the
    // envelope independently of whether they provide additional feedback.
    sentry_capture_envelope(envelope);

    CrashReportDialog crashReportDialog;

    if (crashReportDialog.exec() == QDialog::Accepted) {
        const auto crashUserInfo = crashReportDialog.getCrashUserInfo();
        const auto feedback      = crashUserInfo._feedback.toUtf8();
        const auto contactInfo   = crashUserInfo._contactDetails.toUtf8();
        const auto eventUuid     = sentry_uuid_from_string(eventId.constData());

        sentry_capture_feedback(sentry_value_new_feedback(
            feedback.isEmpty() ? "No additional details were provided." : feedback.constData(),
            contactInfo.isEmpty() ? nullptr : contactInfo.constData(),
            nullptr,
            &eventUuid));
    }

    sentry_close();
    QFile::remove(envelopePath);

    return EXIT_SUCCESS;
}
