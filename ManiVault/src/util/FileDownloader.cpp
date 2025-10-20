// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "FileDownloader.h"
#include "Application.h"
#include "CoreInterface.h"
#include "Task.h"

#include <QThread>
#include <QTimer>
#include <QRegularExpression>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QPromise>

#include <functional>

#ifdef _DEBUG
    #define FILE_DOWNLOADER_VERBOSE
#endif

namespace mv::util {

namespace
{
    /** Single regex handling both filename* (RFC 5987) and filename= (quoted or token) */
    const QRegularExpression kReStarOrPlain(R"re((?:^|;)\s*filename\*\s*=\s*([^;]+)|(?:^|;)\s*filename\s*=\s*(?:"([^"]*)"|([^\s;]*)))re", QRegularExpression::CaseInsensitiveOption);

    /**
     * Strip any path components from \p filePath
     * @param filePath File path to strip
     * @return Safe base name
     */
    QString safeBaseName(const QString& filePath) {
        return QFileInfo(filePath).fileName();
    }

    /**
     * Decode \p ext from RFC 5987 (extValue-value = charset'lang'%XX...)
     * @param extValue Value to decode
     * @return Decoded value
     */
    QString decodeRfc5987(const QString& extValue) {
        static const QRegularExpression rx(R"re(^([^']*)'[^']*'(.+)$)re");

        const auto match = rx.match(extValue);

        if (!match.hasMatch())
            return {};

        const auto charset  = match.captured(1);
        const auto pct      = QByteArray::fromPercentEncoding(match.captured(2).toUtf8());

        if (charset.isEmpty() || charset.compare("UTF-8", Qt::CaseInsensitive) == 0)
            return QString::fromUtf8(pct);

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
        if (auto* codec = QTextCodec::codecForName(charset.toLatin1()))
            return codec->toUnicode(pct);
#endif

        return QString::fromUtf8(pct);
}

    /**
     * Choose a safe file name from content disposition headers from \p reply and \p url
     * @param reply Pointer to the network reply (may not be nullptr)
     * @param url The network reply
     * @return Safe file name
     */
    QString chooseSafeFilenameFromHeaders(QNetworkReply* reply, const QUrl& url) {
        const auto contentDispositionArray = reply->rawHeader("Content-Disposition");

        if (!contentDispositionArray.isEmpty())
        {
            const auto cap = kReStarOrPlain.match(QString::fromUtf8(contentDispositionArray));

            if (cap.hasMatch())
            {
                if (!cap.captured(1).isEmpty()) {
                    if (const auto ext = decodeRfc5987(cap.captured(1)); !ext.isEmpty())
                        return safeBaseName(ext);
                }

                const QString plain = !cap.captured(2).isEmpty() ? cap.captured(2) : cap.captured(3);

                if (!plain.isEmpty())
                    return safeBaseName(plain);
            }
        }

        auto name = url.fileName();

        if (name.isEmpty())
            name = QStringLiteral("download");

        return safeBaseName(name);
    }
}

bool FileDownloader::FileSink::open(QNetworkReply* reply, const QUrl& url, const QString& targetDirectory, bool overwriteAllowed, QString* error)
{
    const QString baseDir   = targetDirectory.isEmpty() ? Application::current()->getTemporaryDir().path() : targetDirectory;
    const QString safeName  = chooseSafeFilenameFromHeaders(reply, url);

    _finalPath = QFileInfo(QDir(baseDir).filePath(safeName)).absoluteFilePath();

    if (!overwriteAllowed && QFileInfo::exists(_finalPath)) {
        if (error)
            *error = QStringLiteral("File exists and overwrite is disabled: %1").arg(_finalPath);

        return false;
    }

    const QString dirPath = QFileInfo(_finalPath).absolutePath();

    if (!QDir().mkpath(dirPath)) {
        if (error)
            *error = QStringLiteral("Unable to create directory: %1").arg(dirPath);

        return false;
    }

    _saveFile.setFileName(_finalPath);

    if (!_saveFile.open(QIODevice::WriteOnly)) {
        if (error)
            *error = QStringLiteral("Failed to open %1 for writing").arg(_finalPath);

        _saveFile.reset();

        return false;
    }

    return true;
}

bool FileDownloader::FileSink::write(const QByteArray& chunk, QString* error)
{
    const auto numberOfBytesWritten = _saveFile.write(chunk);

    if (numberOfBytesWritten != chunk.size()) {
        if (error)
            *error = QStringLiteral("Short write to %1").arg(_finalPath);

        return false;
    }

    return true;
}

bool FileDownloader::FileSink::commit(QString* error)
{
    if (!_saveFile.commit()) {
        if (error)
            *error = QStringLiteral("Failed to commit save file: %1").arg(_finalPath);

        return false;
    }

    return true;
}

void FileDownloader::FileSink::cancel() noexcept
{
    _saveFile.cancelWriting();
}

QString FileDownloader::FileSink::result() const
{
    return _finalPath;
}

bool FileDownloader::ByteArraySink::open(QNetworkReply*, const QUrl&, const QString&, bool, QString*)
{
    _data.clear();

    return true;
}

bool FileDownloader::ByteArraySink::write(const QByteArray& chunk, QString*)
{
    _data.append(chunk);

    return true;
}

bool FileDownloader::ByteArraySink::commit(QString*)
{
    return true;
}

void FileDownloader::ByteArraySink::cancel() noexcept
{
    _data.clear();
}

QByteArray FileDownloader::ByteArraySink::result() const
{
    return _data;
}

FileDownloader::Exception::Exception(const QString& message) :
    BaseException(message, StyledIcon("download"))
{
}

SecureNetworkAccessManager& FileDownloader::sharedManager()
{
    static SecureNetworkAccessManager* instance;
    static bool initialized = false;

    if (!initialized)
    {
        instance = new SecureNetworkAccessManager;

        if (instance->thread() != qApp->thread())
            instance->moveToThread(qApp->thread());
        initialized = true;
    }

    return *instance;
}

QFuture<QByteArray> FileDownloader::downloadToByteArrayAsync(const QUrl& url, Task* task /*= nullptr*/)
{
    return downloadWithSinkAsync<ByteArraySink>(url, QString{}, task, true);
}

QFuture<QString> FileDownloader::downloadToFileAsync(const QUrl& url, const QString& targetDirectory /*= ""*/, Task* task /*= nullptr*/, bool overwriteAllowed /*= true*/)
{
    return downloadWithSinkAsync<FileSink>(url, targetDirectory, task, overwriteAllowed);
}

QFuture<std::uint64_t> FileDownloader::getDownloadSizeAsync(const QUrl& url)
{
    QPromise<std::uint64_t> promise;
    QFuture<std::uint64_t> future = promise.future();

    QMetaObject::invokeMethod(qApp, [promise = std::move(promise), url]() mutable {
        QNetworkRequest request(url);

        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
        request.setMaximumRedirectsAllowed(maximumNumberOfRedirectsAllowed);

        auto reply = sharedManager().head(request);

        connect(reply, &QNetworkReply::finished, [reply, promise = std::move(promise), url]() mutable {
            if (reply->error() == QNetworkReply::NoError) {
                auto lengthHeader = reply->header(QNetworkRequest::ContentLengthHeader);

                if (lengthHeader.isValid()) {
                    promise.addResult(lengthHeader.toULongLong());
                } else {
                    qCritical() << QString("Get download size: Content-Length absent for %1").arg(url.toDisplayString());

                    promise.addResult(0);
                }
            } else {
                qCritical() << QString("Get download size HEAD request failed for %1: %2").arg(url.toDisplayString(), reply->errorString());

                promise.addResult(0);
            }

            reply->deleteLater();

            promise.finish();
        });
    });

    return future;
}

QFuture<QDateTime> FileDownloader::getLastModifiedAsync(const QUrl& url)
{
    QPromise<QDateTime> promise;
    QFuture<QDateTime> future = promise.future();

    QMetaObject::invokeMethod(qApp, [promise = std::move(promise), url]() mutable {
        QNetworkRequest request(url);

        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
        request.setMaximumRedirectsAllowed(maximumNumberOfRedirectsAllowed);

        auto reply = sharedManager().head(request);

        connect(reply, &QNetworkReply::finished, [reply, promise = std::move(promise)]() mutable {
            if (reply->error() != QNetworkReply::NoError) {
                qCritical() << QString("Get last modified HEAD request failed: %1").arg(reply->errorString());

                promise.setException(std::make_exception_ptr(Exception(QString("HEAD request failed: %1").arg(reply->errorString()))));
            }
            else {
                const auto lastModified = reply->header(QNetworkRequest::LastModifiedHeader);

                promise.addResult(lastModified.isValid() ? lastModified.toDateTime() : QDateTime{});
            }

            reply->deleteLater();

            promise.finish();
        });
    });

    return future;
}

void FileDownloader::notifyError(const QString& urlDisplayString, const QString& errorString)
{
    qCritical() << QStringLiteral("Download problem: <a>%1</a> not downloaded: %2").arg(urlDisplayString, errorString.toHtmlEscaped());

    mv::help().addNotification("Download problem", QStringLiteral("Project not downloaded: %1").arg(errorString), StyledIcon("circle-exclamation"));
}

}
