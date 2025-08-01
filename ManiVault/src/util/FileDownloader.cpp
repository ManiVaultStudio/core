// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "FileDownloader.h"
#include "SecureNetworkAccessManager.h"
#include "Application.h"
#include "Task.h"

#include <QThread>

#ifdef _DEBUG
	#define FILE_DOWNLOADER_VERBOSE
#endif

namespace mv::util {

SecureNetworkAccessManager& FileDownloader::sharedManager()
{
	static SecureNetworkAccessManager instance;
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

        QNetworkReply* reply = sharedManager().get(request);

        QObject::connect(reply, &QNetworkReply::finished, [reply, promise = std::move(promise), task]() mutable {
            if (reply->error() != QNetworkReply::NoError) {
                promise.setException(std::make_exception_ptr(std::runtime_error(reply->errorString().toStdString())));
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
            if (task && task->isAborting())
                return;

            const auto progress = static_cast<float>(downloaded) / static_cast<float>(total);

            if (task)
                task->setProgress(progress);
		});
    });

    return future;
}

QByteArray FileDownloader::downloadToByteArraySync(const QUrl& url, Task* task /*= nullptr*/)
{
	QEventLoop loop;
	QByteArray downloadedData;
	std::optional<std::exception_ptr> error;

	auto future = FileDownloader::downloadToByteArrayAsync(url, task);

	future.then([&](const QByteArray& result) {
			downloadedData = result;
			loop.quit();
		}).onFailed([&](const QException& e) {
			error = std::make_exception_ptr(std::runtime_error(e.what()));
			loop.quit();
	});

	loop.exec();

	if (error.has_value()) {
		std::rethrow_exception(error.value());
	}

	return downloadedData;
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

        QNetworkReply* reply = sharedManager().get(request);

        connect(reply, &QNetworkReply::finished, [reply, promise = std::move(promise), url, targetDirectory, task]() mutable {
            if (reply->error() != QNetworkReply::NoError) {
                promise.setException(std::make_exception_ptr(std::runtime_error(reply->errorString().toStdString())));
            }
            else {
                QString downloadedFilePath;

                QString filename = QFileInfo(url.toString()).fileName();

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

                QFile localFile(downloadedFilePath);

                if (!localFile.open(QIODevice::WriteOnly))
                    return;

                localFile.write(reply->readAll());
                localFile.close();

                promise.addResult(downloadedFilePath);

                if (task)
                    task->setFinished();
            }

            reply->deleteLater();

            promise.finish();
        });

        connect(reply, &QNetworkReply::downloadProgress, [task](qint64 downloaded, qint64 total) -> void {
	        if (task && task->isAborting())
				return;

	        const auto progress = static_cast<float>(downloaded) / static_cast<float>(total);

	        if (task)
                task->setProgress(progress);
        });
    });

    return future;
}

QString FileDownloader::downloadToFileSync(const QUrl& url, const QString& targetDirectory /*= ""*/, Task* task /*= nullptr*/)
{
    QEventLoop loop;
    std::optional<std::exception_ptr> error;

    QString downloadedFilePath;

	auto future = FileDownloader::downloadToFileAsync(url, targetDirectory, task);

    future.then([&](const QString& result) {
        downloadedFilePath = result;
        loop.quit();
    }).onFailed([&](const QException& e) {
        error = std::make_exception_ptr(std::runtime_error(e.what()));
        loop.quit();
    });

    loop.exec(); // Block until either then or onFailed is called

    if (error.has_value()) {
        std::rethrow_exception(error.value());
    }

    return downloadedFilePath;
}

QFuture<std::uint64_t> FileDownloader::getDownloadSizeAsync(const QUrl& url)
{
    QPromise<std::uint64_t> promise;
    QFuture<std::uint64_t> future = promise.future();

    // Ensure this code runs in the main thread
    QMetaObject::invokeMethod(qApp, [promise = std::move(promise), url]() mutable {
        QNetworkRequest request(url);

        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

        QNetworkReply* reply = sharedManager().head(request);

        connect(reply, &QNetworkReply::finished, [reply, promise = std::move(promise), url]() mutable {
            reply->deleteLater();

            if (reply->error() == QNetworkReply::NoError) {
                auto lengthHeader = reply->header(QNetworkRequest::ContentLengthHeader);

                if (lengthHeader.isValid()) {
                    //qDebug() << "Content-Length for" << url.toString() << "is" << lengthHeader.toULongLong();
                    promise.addResult(lengthHeader.toULongLong());
                    promise.finish();
                    return;
                }
                else {
                    promise.setException(std::make_exception_ptr(std::runtime_error("Content-Length header not present")));
                    return;
                }
            }

            promise.setException(std::make_exception_ptr(std::runtime_error(reply->errorString().toStdString())));
        });
    });

    return future;
}

std::uint64_t FileDownloader::getDownloadSizeSync(const QUrl& url)
{
    QNetworkRequest request(url);

    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    QNetworkReply* reply = sharedManager().get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        auto errorMsg = reply->errorString();
        reply->deleteLater();
        throw std::runtime_error("HEAD request failed: " + errorMsg.toStdString());
    }

    auto lengthHeader = reply->header(QNetworkRequest::ContentLengthHeader);

    if (!lengthHeader.isValid()) {
        reply->deleteLater();
        throw std::runtime_error("Length header not found");
    }

    const auto result = lengthHeader.toULongLong();

    reply->deleteLater();

    return result;
}

QFuture<QDateTime> FileDownloader::getLastModifiedAsync(const QUrl& url)
{
    QPromise<QDateTime> promise;
    QFuture<QDateTime> future = promise.future();

    // Ensure this code runs in the main thread
    QMetaObject::invokeMethod(qApp, [promise = std::move(promise), url]() mutable {
        QNetworkRequest request(url);

        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

        auto reply = sharedManager().get(request);

        connect(reply, &QNetworkReply::finished, [reply, promise = std::move(promise), url]() mutable {
            if (reply->error() != QNetworkReply::NoError) {
                promise.setException(std::make_exception_ptr(
                    std::runtime_error("HEAD request failed: " + reply->errorString().toStdString())
                ));
            }
            else {
                const auto lastModified = reply->header(QNetworkRequest::LastModifiedHeader);

                if (!lastModified.isValid()) {
                    promise.setException(std::make_exception_ptr(
                        std::runtime_error("Last-Modified header not found")
                    ));
                }
                else {
                    promise.addResult(lastModified.toDateTime());
                }
            }

            reply->deleteLater();
            promise.finish();
        });
    });

    return future;
}

QDateTime FileDownloader::getLastModifiedSync(const QUrl& url)
{
    try {
        QNetworkRequest request(url);

        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

        auto reply = sharedManager().get(request);

        QEventLoop loop;

    	connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    	loop.exec();

        if (reply->error() != QNetworkReply::NoError) {
            auto errorMsg = reply->errorString();

        	reply->deleteLater();

        	throw std::runtime_error("HEAD request failed: " + errorMsg.toStdString());
        }

        QVariant lastModifiedHeader = reply->header(QNetworkRequest::LastModifiedHeader);

        if (!lastModifiedHeader.isValid()) {
            reply->deleteLater();

            throw std::runtime_error("Last-Modified header not found");
        }

        const auto result = lastModifiedHeader.toDateTime();

        reply->deleteLater();

        return result;
    }
    catch (std::exception& e)
    {
        qDebug() << QString("Unable to establish last modified for %1 due to an unhandled exception:").arg(url.toString()) << e.what();
    }
    catch (...) {
	    qDebug() << QString("Unable to establish last modified for %1 due to an unhandled exception").arg(url.toString());
    }

    return {};
}

}
