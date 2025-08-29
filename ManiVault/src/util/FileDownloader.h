// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "SecureNetworkAccessManager.h"

#include "util/Exception.h"

#include "Task.h"

#include <QObject>
#include <QUrl>
#include <QByteArray>
#include <QNetworkReply>
#include <QPointer>
#include <QFuture>
#include <QSaveFile>
#include <QCoreApplication>

namespace mv::util {

class SecureNetworkAccessManager;

/**
 * File downloader class
 *
 * Helper class for simplifying file download
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT FileDownloader : public QObject
{
protected:

    // Generic sink concept used by the streaming core
    template <typename T>
    struct ISink {
        using Result = T;
        virtual ~ISink() = default;

        /**
         * Finalize successfully (e.g., commit QSaveFile)
         * @param reply Pointer to network reply (may not be nullptr)
         * @param url Url to download
         * @param targetDirectory Target directory where to put the download
         * @param overwriteAllowed Whether it is allowed to overwrite an existing download
         * @param error Pointer to error
         * @return Whether successfully opened
         */
        virtual bool open(QNetworkReply* reply, const QUrl& url, const QString& targetDirectory, bool overwriteAllowed, QString* error) = 0;

        /**
         * Write \p chunk with \p error
         * @param chunk Chunk of data
         * @param error Pointer to error
         * @return Whether successfully written
         */
        virtual bool write(const QByteArray& chunk, QString* error) = 0;

        /**
         * Finalize successfully (e.g., commit QSaveFile)
         * @param error Pointer to error
         * @return Whether commit was successful
         */
        virtual bool commit(QString* error) = 0;

        /** Discards partial output */
        virtual void cancel() noexcept = 0;

        /**
         * Get the resulting value to emit into the promise
         * @return The promise result
         */
        virtual Result result() const = 0;
    };

    /** Saves the download to a file on disk */
    class FileSink final : public ISink<QString> {
    public:

        /**
         * Finalize successfully (e.g., commit QSaveFile)
         * @param reply Pointer to network reply (may not be nullptr)
         * @param url Url to download
         * @param targetDirectory Target directory where to put the download
         * @param overwriteAllowed Whether it is allowed to overwrite an existing download
         * @param error Pointer to error
         * @return Whether successfully opened
         */
        bool open(QNetworkReply* reply, const QUrl& url, const QString& targetDirectory, bool overwriteAllowed, QString* error) override;

        /**
         * Write \p chunk with \p error
         * @param chunk Chunk of data
         * @param error Pointer to error
         * @return Whether successfully written
         */
        bool write(const QByteArray& chunk, QString* error) override;

        /**
         * Finalize successfully (e.g., commit QSaveFile)
         * @param error Pointer to error
         * @return Whether commit was successful
         */
        bool commit(QString* error) override;

        /** Discards partial output */
        void cancel() noexcept override;

        /**
         * Get the resulting value to emit into the promise
         * @return The promise result
         */
        QString result() const override;

    private:
        QString     _finalPath;     /** Final download path */
        QSaveFile   _saveFile;          /** For saving the file atomically */
    };

    /** Saves the download to a byte array */
    class ByteArraySink final : public ISink<QByteArray> {
    public:

        /**
         * Finalize successfully (e.g., commit QSaveFile)
         * @param reply Pointer to network reply (may not be nullptr)
         * @param url Url to download
         * @param targetDirectory Target directory where to put the download
         * @param overwriteAllowed Whether it is allowed to overwrite an existing download
         * @param error Pointer to error
         * @return Whether successfully opened
         */
        bool open(QNetworkReply* reply, const QUrl& url, const QString& targetDirectory, bool overwriteAllowed, QString* error) override;

        /**
         * Write \p chunk with \p error
         * @param chunk Chunk of data
         * @param error Pointer to error
         * @return Whether successfully written
         */
        bool write(const QByteArray& chunk, QString* error) override;

        /**
         * Finalize successfully (e.g., commit QSaveFile)
         * @param error Pointer to error
         * @return Whether commit was successful
         */
        bool commit(QString* error) override;

        /** Discards partial output */
        void cancel() noexcept override;

        /**
         * Get the resulting value to emit into the promise
         * @return The promise result
         */
        QByteArray result() const override;

    private:
        QByteArray _data;   /** Download result as bytes */
    };

public:

    /** Exception class for file downloader issues */
    class Exception : public BaseException {
    public:

        /**
         * Construct with \p message
         * @param message Message describing the download issue
         */
        explicit Exception(const QString& message);
    };

    /**
     * Get the shared network access manager instance
     * @return Reference to the shared QNetworkAccessManager instance
     */
	static SecureNetworkAccessManager& sharedManager();

    /** 
     * Download file to byte array asynchronously
     * @param url URL of the file to download
     * @param task Optional task to associate with the download operation (must live in the main/GUI thread)
     * @return Future containing the downloaded data as byte array
     */
    static QFuture<QByteArray> downloadToByteArrayAsync(const QUrl& url, Task* task = nullptr);

    /**
     * Download file from \p url to \p targetDirectory asynchronously
     * @param url URL of the file to download
     * @param targetDirectory Directory where the file should be saved (OS temporary dir when empty)
     * @param task Optional task to associate with the download operation (must live in the main/GUI thread)
     * @param overwriteAllowed Whether it is ok to overwrite existing download
     * @return Future containing the path to the downloaded file
     */
    static QFuture<QString> downloadToFileAsync(const QUrl& url, const QString& targetDirectory = "", Task* task = nullptr, bool overwriteAllowed = true);

    /**
     * Get the size of the file to be downloaded asynchronously
     * @param url URL of the file to be downloaded
     * @return Size of the file to be downloaded in bytes, 0 if size cannot be determined
     */
    static QFuture<std::uint64_t> getDownloadSizeAsync(const QUrl& url);

    /**
     * Get the last modified date of the file at \p url asynchronously
     * @param url URL of the file to check
     * @return Last modified date of the file, QDateTime() if it cannot be determined
     */
    static QFuture<QDateTime> getLastModifiedAsync(const QUrl& url);

private:

    template <typename SinkT>
    static QFuture<typename SinkT::Result> downloadWithSinkAsync(const QUrl& url, const QString& targetDirectory, Task* task, bool overwriteAllowed)
    {
        using ResultT = typename SinkT::Result;

        auto promise    = std::make_shared<QPromise<ResultT>>();
        auto future     = promise->future();

        QMetaObject::invokeMethod(qApp, [promise, url, targetDirectory, task, overwriteAllowed]() mutable {
            if (task) {
                task->setName(QStringLiteral("Download %1").arg(url.fileName()));
                task->setIcon(StyledIcon("download"));
                task->setRunning();
            }

            QNetworkRequest request(url);

            request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

#if QT_VERSION >= QT_VERSION_CHECK(6,7,0)
            request.setMaximumRedirectsAllowed(10);
#endif

            auto reply = sharedManager().get(request);

            // Shared transfer state
            struct State {
                std::shared_ptr<SinkT> sink;
                bool opened = false;
                bool finished = false;
                bool aborted = false;

                State() : sink(std::make_shared<SinkT>()) {}
            };

            const auto state = std::make_shared<State>();

            auto finishOnce = [state, promise](auto&& fn) {
                if (state->finished)
                    return;

                state->finished = true;

            	std::forward<decltype(fn)>(fn)();

            	promise->finish();
			};

            // Abort handling (user clicks on cancel in the UI, task->abort() is called)
            if (task) {
                connect(task, &Task::requestAbort, reply, [reply, state, task]() {

                	// Guard network reply access
                    QPointer<QNetworkReply> safeReply = reply;

                    // Abort the download if we have a valid reply
                    if (!safeReply || !safeReply->isRunning())
                        return;

                    state->aborted = true;

                    safeReply->abort();

                    // Also abort te task
                	if (task)
                        task->setAborted();
                });
            }

            // Progress
            connect(reply, &QNetworkReply::downloadProgress, reply, [task](qint64 done, qint64 total) {

                if (task && task->isAborting())
                    return;
                
                if (task)
                    task->setProgress((total > 0) ? static_cast<float>(done) / static_cast<float>(total) : 0.0f);
            });

            // Lazy open (on first data)
            auto openIfNeeded = [state, promise, finishOnce, url, targetDirectory, overwriteAllowed](QPointer<QNetworkReply> safeReply) -> bool {

                if (!safeReply)
                    return false;

                if (state->opened)
                    return true;

                QString errorString;

                if (!state->sink->open(safeReply, url, targetDirectory, overwriteAllowed, &errorString)) {
                    safeReply->abort();

                    promise->setException(std::make_exception_ptr(Exception(errorString)));

                    finishOnce([safeReply]() {
                        safeReply->deleteLater();
                    });

                    return false;
                }

                state->opened = true;

                return true;
            };

            // Stream data
            connect(reply, &QNetworkReply::readyRead, reply, [reply, state, promise, finishOnce, openIfNeeded]() mutable {

                // Guard network reply access
                auto safeReply = QPointer<QNetworkReply>(reply);

            	if (!openIfNeeded(safeReply))
                    return;

                while (reply->bytesAvailable() > 0) {
                    const auto  chunk = safeReply->read(64 * 1024);

                    if (chunk.isEmpty())
                        break;

                    QString errorString;

                    if (!state->sink->write(chunk, &errorString)) {
                        state->sink->cancel();

                        promise->setException(std::make_exception_ptr(Exception(errorString)));

                        finishOnce([safeReply]() {
                            safeReply->deleteLater();
                        });

                        safeReply->abort();

                        return;
                    }
                }
            });

            // Complete
            connect(reply, &QNetworkReply::finished, reply, [reply, state, task, promise, finishOnce, openIfNeeded]() mutable {

                // Guard network reply access
                auto safeReply = QPointer<QNetworkReply>(reply);

                if (!openIfNeeded(safeReply))
                    return;

                const QVariant statusVar = safeReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

                const auto urlDisplayString = reply->url().toDisplayString().toHtmlEscaped();

				if (statusVar.isValid()) {
                    const int status = statusVar.toInt();

                    if (status < 200 || status >= 300) {
                        state->sink->cancel();

                        const auto reason       = safeReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
                        const auto errorString  = QStringLiteral("HTTP %1 %2").arg(status).arg(reason);

                        if (task)
                            task->setAborted();

                        notifyError(urlDisplayString, errorString);

                        promise->setException(std::make_exception_ptr(Exception(errorString)));

                        finishOnce([safeReply]() {
                            safeReply->deleteLater();
						});

                        return;
					}
                }

                if (safeReply->error() == QNetworkReply::NoError) {
                    QString errorString;

                    if (!state->sink->commit(&errorString)) {

                        if (task) {
                            task->setAborted();
                        }

                        notifyError(urlDisplayString, errorString);

                        promise->setException(std::make_exception_ptr(Exception(errorString)));

                        finishOnce([safeReply]() {
                            safeReply->deleteLater();
                        });

                        return;
                    }

                    promise->addResult(state->sink->result());

                    finishOnce([safeReply, task]() {
                        safeReply->deleteLater();

                        if (task && !task->isAborting())
                            task->setFinished();
                    });
                }
                else {
                    state->sink->cancel();

                    const auto errorString = (safeReply->error() == QNetworkReply::OperationCanceledError) ? QStringLiteral("Download aborted by user") : safeReply->errorString();

                    if (task) {
                        task->setAborted();
                    }

                	notifyError(urlDisplayString, errorString);

                    promise->setException(std::make_exception_ptr(Exception(errorString)));

                    finishOnce([safeReply]() {
                        safeReply->deleteLater();
                    });
                }
            });
        });

        return future;
    }

private:

    /** 
     * Notifies the user about a download error with the given \p urlDisplayString and \p errorString
     * @param urlDisplayString Display string of the URL that was attempted to be downloaded
     * @param errorString Error string describing the download issue
     * 
     */
    static void notifyError(const QString& urlDisplayString, const QString& errorString);

    static constexpr std::int32_t maximumNumberOfRedirectsAllowed = 10; /** Puts a cap on the number of redirects. Malicious URLs could loop indefinitely */
};

}
