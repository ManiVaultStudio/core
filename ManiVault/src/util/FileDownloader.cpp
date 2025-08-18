// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "FileDownloader.h"
#include "SecureNetworkAccessManager.h"
#include "Application.h"
#include "Task.h"

#include <QThread>
#include <QTimer>
#include <QRegularExpression>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QPromise>

#ifdef _DEBUG
	#define FILE_DOWNLOADER_VERBOSE
#endif

namespace mv::util {

FileDownloader::Exception::Exception(const QString& message) :
    BaseException(message, StyledIcon("download"))
{
}

SecureNetworkAccessManager& FileDownloader::sharedManager()
{
    static SecureNetworkAccessManager instance;
    static bool initialized = false;

    if (!initialized) {
        initialized = true;

        if (instance.thread() != qApp->thread())
            instance.moveToThread(qApp->thread());
    }

    return instance;
}

QFuture<QByteArray> FileDownloader::downloadToByteArrayAsync(const QUrl& url, Task* task /*= nullptr*/)
{
    QPromise<QByteArray> promise;
    QFuture<QByteArray> future = promise.future();

    // Ensure this code runs in the main thread
    QMetaObject::invokeMethod(qApp, [url, promise = std::move(promise), task]() mutable {
        if (task)
            task->setRunning();

        QNetworkRequest request(url);

        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
        request.setMaximumRedirectsAllowed(maximumNumberOfRedirectsAllowed);

        auto reply = sharedManager().get(request);

        if (task)
            handleAbort(task, reply);

        connect(reply, &QNetworkReply::finished, [url, reply, promise = std::move(promise), task]() mutable {
            if (reply->error() != QNetworkReply::NoError) {
                promise.setException(std::make_exception_ptr(Exception(reply->errorString())));

                const auto urlDisplayString = reply->url().toDisplayString().toHtmlEscaped();
                const auto errorString      = reply->errorString();

                QTimer::singleShot(250, [url, urlDisplayString, errorString]() -> void {
                    qCritical() << QString("Download problem: %1 not downloaded: %2").arg(urlDisplayString, errorString);
                    mv::help().addNotification("Download problem", QString("<i>%1<i> not downloaded: %2").arg(urlDisplayString, errorString), StyledIcon("circle-exclamation"));
				});
            }
            else {
                promise.addResult(reply->readAll());
            }

            reply->deleteLater();

        	promise.finish();

            if (task)
                task->setFinished();
		});

        connect(reply, &QNetworkReply::downloadProgress, [task](qint64 downloaded, qint64 total) -> void {
            if (!task)
                return;

            if (task->isAborting())
                return;

            task->setProgress(total > 0 ? static_cast<float>(downloaded) / static_cast<float>(total) : .0f);
		});
    });

    return future;
}

QFuture<QString> FileDownloader::downloadToFileAsync(const QUrl& url, const QString& targetDirectory /*= ""*/, Task* task /*= nullptr*/)
{
    QPromise<QString> promise;
    QFuture<QString> future = promise.future();

    // Ensure this code runs in the main thread
    QMetaObject::invokeMethod(qApp, [promise = std::move(promise), url, targetDirectory, task]() mutable {
        if (task)
            task->setRunning();

        QNetworkRequest request(url);

        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
        request.setMaximumRedirectsAllowed(maximumNumberOfRedirectsAllowed);

        auto reply = sharedManager().get(request);

        if (task)
            handleAbort(task, reply);

        connect(reply, &QNetworkReply::finished, [reply, promise = std::move(promise), url, targetDirectory, task]() mutable {
            if (reply->error() != QNetworkReply::NoError) {
                

                const auto urlDisplayString = reply->url().toDisplayString();
                const auto errorString      = reply->errorString();

                QTimer::singleShot(250, [urlDisplayString, errorString]() -> void {
                    qCritical() << QString("Download problem: %1 not downloaded: %2").arg(urlDisplayString, errorString);
                    mv::help().addNotification("Download problem", QString("<i>%1<i> not downloaded: %2").arg(urlDisplayString, errorString), StyledIcon("circle-exclamation"));
				});

                promise.setException(std::make_exception_ptr(Exception(reply->errorString())));
                promise.finish();

                reply->deleteLater();
            }
            else {
                QString downloadedFilePath;

                QString filename = url.fileName();

                QVariant dispositionHeader = reply->rawHeader("Content-Disposition");

                // Handle Content-Disposition header to extract filename
                if (dispositionHeader.isValid()) {
                    QRegularExpression re("filename\\*=UTF-8''([^;]+)|filename=\"?([^\";]+)\"?");
                    QRegularExpressionMatch match = re.match(dispositionHeader.toString());
            
                    if (match.hasMatch()) {
                        filename = QUrl::fromPercentEncoding(match.captured(1).toUtf8());
            
                        if (filename.isEmpty())
                            filename = match.captured(2);
                    }
                }

                if (targetDirectory.isEmpty())
                    downloadedFilePath = QDir(Application::current()->getTemporaryDir().path()).filePath(filename);
                else
                    downloadedFilePath = QDir(targetDirectory).filePath(filename);

                // Sanitize filename to base name only (avoid path traversal / separators)
                filename = QFileInfo(filename).fileName();
                downloadedFilePath = QFileInfo(QDir(downloadedFilePath).filePath(filename)).absoluteFilePath();
                
                // Ensure target directory exists
                const auto targetDirectoryPath = QFileInfo(downloadedFilePath).absolutePath();

                if (!QDir().mkpath(targetDirectoryPath))
                {
                    promise.setException(std::make_exception_ptr(Exception(QString("Unable to create %1").arg(targetDirectoryPath))));

                    reply->deleteLater();
                    promise.finish();

                    return;
                }

                QFile localFile(downloadedFilePath);

                if (!localFile.open(QIODevice::WriteOnly)) {
                    promise.setException(std::make_exception_ptr(Exception(QString("Failed to open %1 for writing").arg(downloadedFilePath))));

                    reply->deleteLater();
                    promise.finish();

                    return;
                }

                const auto data = reply->readAll();

                if (localFile.write(data) != data.size()) {
                    localFile.close();
                    promise.setException(std::make_exception_ptr(Exception(QString("Short write to %1").arg(downloadedFilePath))));

                    reply->deleteLater();
                    promise.finish();

                    return;
                }

                localFile.close();

                promise.addResult(downloadedFilePath);

                if (task)
                    task->setFinished();

                reply->deleteLater();

                promise.finish();
            }
        });

        connect(reply, &QNetworkReply::downloadProgress, [task](qint64 downloaded, qint64 total) -> void {
            if (!task)
                return;

            if (task->isAborting())
                return;

            task->setProgress(total > 0 ? static_cast<float>(downloaded) / static_cast<float>(total) : .0f);
        });
    });

    return future;
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
                    qCritical() << QString("Get download size HEAD request for %1 failed: Content-Length header not present").arg(url.toDisplayString());

	                promise.setException(std::make_exception_ptr(Exception("Content-Length header not present")));
                }
            } else {
                qCritical() << QString("Get download size HEAD request failed for %1: %2").arg(url.toDisplayString(), reply->errorString());

                promise.setException(std::make_exception_ptr(Exception(reply->errorString())));
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

void FileDownloader::handleAbort(Task* task, QNetworkReply* reply)
{
    Q_ASSERT(task && reply);

    if (!task || !reply)
        return;

    connect(task, &Task::requestAbort, reply, [task, reply]() {
        if (reply->isRunning()) {
            reply->abort();

            task->setAborted();
        }
	});
}

}
