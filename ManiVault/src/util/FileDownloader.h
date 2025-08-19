// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "CoreInterface.h"

#include "util/Exception.h"

#include "Task.h"

#include <QObject>
#include <QUrl>
#include <QByteArray>
#include <QNetworkReply>
#include <QPointer>
#include <QFuture>
#include <QFuture>
#include <QSaveFile>

namespace mv::util {

class SecureNetworkAccessManager;


/**
 * File downloader class
 * Help class for simplifying file download
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
        QString                     _finalPath;     /** Final download path */
        std::unique_ptr<QSaveFile>  _save;          /** For saving the file atomically */
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

        QPromise<ResultT> promise;
        auto future = promise.future();

        QMetaObject::invokeMethod(qApp, [promise = std::move(promise), url, targetDirectory, task, overwriteAllowed]() mutable {
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
                std::shared_ptr<SinkT> sink = std::make_shared<SinkT>();
                bool opened     = false;
                bool finished   = false;
                bool aborted    = false;
            };
            const auto state = std::make_shared<State>();

            auto finishOnce = [state, &promise](const std::function<void()>& finishFunction) mutable {
                if (state->finished)
                    return;

                state->finished = true;

                if (finishFunction)
                    finishFunction();

                promise.finish();
            };

            // Abort handling
            if (task) {
                connect(task, &Task::requestAbort, reply, [reply, state, task]() {
                    state->aborted = true;
                    reply->abort();              // triggers finished()
                    if (task) task->setAborted();
                });
            }

            // Progress
            connect(reply, &QNetworkReply::downloadProgress, [task](qint64 done, qint64 total) {
                if (task && task->isAborting())
                    return;
                
                if (task)
                    task->setProgress((total > 0) ? static_cast<float>(done) / static_cast<float>(total) : 0.0f);
            });

            // Lazy open (on first data)
            auto openIfNeeded = [state, reply, &promise, &finishOnce, &url, &targetDirectory, overwriteAllowed]() -> bool {
                if (state->opened)
                    return true;

                QString errorString;

                if (!state->sink->open(reply, url, targetDirectory, overwriteAllowed, &errorString)) {
                    reply->abort();

                    promise.setException(std::make_exception_ptr(Exception(errorString)));

                    finishOnce([reply]() {
                        reply->deleteLater();
                    });

                    return false;
                }

                state->opened = true;

                return true;
            };

            // Stream data
            connect(reply, &QNetworkReply::readyRead, [reply, state, &promise, &finishOnce, &openIfNeeded]() mutable {
                if (!openIfNeeded())
                    return;

                while (reply->bytesAvailable() > 0) {
                    const auto  chunk = reply->read(64 * 1024);

                    if (chunk.isEmpty())
                        break;

                    QString errorString;

                    if (!state->sink->write(chunk, &errorString)) {
                        state->sink->cancel();

                        promise.setException(std::make_exception_ptr(Exception(errorString)));

                        finishOnce([reply]() {
                            reply->deleteLater();
                        });

                        reply->abort();

                        return;
                    }
                }
            });

            // Complete
            connect(reply, &QNetworkReply::finished, [reply, state, task, &promise, &finishOnce, &openIfNeeded]() mutable {
                if (!openIfNeeded())
                    return; // zero-byte case handled inside

                if (reply->error() == QNetworkReply::NoError) {
                    QString errorString;

                    if (!state->sink->commit(&errorString)) {
                        promise.setException(std::make_exception_ptr(Exception(errorString)));

                        finishOnce([reply]() {
                            reply->deleteLater();
                        });

                        return;
                    }

                    promise.addResult(state->sink->result());

                    finishOnce([reply, task]() {
                        reply->deleteLater();

                        if (task && !task->isAborting())
                            task->setFinished();
                    });
                }
                else {
                    state->sink->cancel();

                    const auto errorString      = (reply->error() == QNetworkReply::OperationCanceledError) ? QStringLiteral("Download aborted by user") : reply->errorString();
                    const auto urlDisplayString = reply->url().toDisplayString().toHtmlEscaped();

                    //QTimer::singleShot(250, [urlDisplayString, errorString]() {
                    //    qCritical() << QStringLiteral("Download problem: %1 not downloaded: %2").arg(urlDisplayString, errorString);
                    //    mv::help().addNotification("Download problem", QStringLiteral("<i>%1</i> not downloaded: %2").arg(urlDisplayString, errorString), StyledIcon("circle-exclamation"));
                    //});

                    promise.setException(std::make_exception_ptr(Exception(errorString)));

                    finishOnce([reply]() {
                        reply->deleteLater();
                    });
                }
            });
        });

        return future;
    }

private:

    /**
     * Takes care of gracefully aborting the download if the user wants the download task to be aborted
     * @param task Pointer to the Task that is associated with the download operation (may not be nullptr)
     * @param reply Pointer to the QNetworkReply that is performing the download operation (may not be nullptr)
     */
    static void handleAbort(Task* task, QNetworkReply* reply);

    static constexpr std::int32_t maximumNumberOfRedirectsAllowed = 10; /** Puts a cap on the number of redirects. Malicious URLs could loop indefinitely */
};

}
