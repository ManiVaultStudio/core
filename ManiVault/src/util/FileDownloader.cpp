// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "FileDownloader.h"

#include "Task.h"
#include "Application.h"

#include <QThread>

#ifdef _DEBUG
	#define FILE_DOWNLOADER_VERBOSE
#endif

namespace mv::util {

FileDownloader::FileDownloader(const StorageMode& mode, const Task::GuiScope& taskGuiScope, QObject* parent) :
    QObject(parent),
    _storageMode(mode),
    _isDownloading(false),
    _task(QThread::currentThread() == QCoreApplication::instance()->thread() ? new Task(this, "Downloading", { taskGuiScope }, Task::Status::Undefined, true) : nullptr)
{
    if (_task)
		_task->setEnabled(taskGuiScope != Task::GuiScope::None);
}

void FileDownloader::download(const QUrl& url)
{
    if (_isDownloading)
        return;

#ifdef FILE_DOWNLOADER_VERBOSE
    qDebug() << __FUNCTION__ << url.toString();
#endif

    _url            = url;
    _isDownloading  = true;

	QNetworkRequest request(_url);

    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    auto* networkReply = sharedManager().get(request);

    connect(networkReply, &QNetworkReply::finished, this, [this, networkReply]() -> void {
        downloadFinished(networkReply);
    });

    const auto fileName = QFileInfo(_url.toString()).fileName();

    if (_task) {
        _task->setName(QString("Download %1").arg(fileName));
        _task->setIcon(StyledIcon("download"));
        _task->setRunning();

        disconnect(_task, &Task::requestAbort, this, nullptr);

        connect(_task, &Task::requestAbort, this, [this, networkReply]() -> void {
            networkReply->abort();

            _task->setAborted();

            emit aborted();
		});
    }

    connect(networkReply, &QNetworkReply::downloadProgress, this, [&](qint64 downloaded, qint64 total) -> void {
        if (_task && _task->isAborting())
            return;
			
        const auto progress = static_cast<float>(downloaded) / static_cast<float>(total);

        if (_task)
			_task->setProgress(progress);

        emit downloadProgress(progress);
    });
}

bool FileDownloader::isDownloading() const
{
    return _isDownloading;
}

QString FileDownloader::getDownloadedFilePath() const
{
    return _downloadedFilePath;
}

void FileDownloader::downloadFinished(QNetworkReply* reply)
{
#ifdef FILE_DOWNLOADER_VERBOSE
    qDebug() << __FUNCTION__ << reply->error();
#endif

    QString filename = QFileInfo(_url.toString()).fileName();

    QVariant dispositionHeader = reply->rawHeader("Content-Disposition");

    if (dispositionHeader.isValid()) {
        QRegularExpression re("filename\\*=UTF-8''([^;]+)|filename=\"?([^\";]+)\"?");
        QRegularExpressionMatch match = re.match(dispositionHeader.toString());

    	if (match.hasMatch()) {
            filename = QUrl::fromPercentEncoding(match.captured(1).toUtf8());

            if (filename.isEmpty())
                filename = match.captured(2); // fallback
        }
    }

    if (_storageMode & StorageMode::File)
    {
        if (_targetDirectory.isEmpty())
    		_downloadedFilePath = QDir(Application::current()->getTemporaryDir().path()).filePath(filename);
        else
            _downloadedFilePath = QDir(_targetDirectory).filePath(filename);

#ifdef FILE_DOWNLOADER_VERBOSE
        qDebug() << filename << ": " << _downloadedFilePath;
#endif

        QFile localFile(_downloadedFilePath);

        if (!localFile.open(QIODevice::WriteOnly))
            return;

        _downloadedData = reply->readAll();

        localFile.write(_downloadedData);
    	localFile.close();
    }

    if (_storageMode == StorageMode::ByteArray) {
        _downloadedData = reply->readAll();
    }

    reply->deleteLater();

    _isDownloading = false;

    if (reply->error() == QNetworkReply::NoError) {
        if (_task)
            _task->setFinished();

		emit downloaded();
    }
}

const QString& FileDownloader::getTargetDirectory() const
{
    return _targetDirectory;
}

void FileDownloader::setTargetDirectory(const QString& targetDirectory)
{
    _targetDirectory = targetDirectory;
}

QPointer<Task> FileDownloader::getTask()
{
    return _task;
}

QPointer<Task> FileDownloader::getTask() const
{
    return _task;
}

QFuture<std::uint64_t> FileDownloader::getDownloadSizeAsync(const QUrl& url)
{
    QPromise<std::uint64_t> promise;
    QFuture<std::uint64_t> future = promise.future();

    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    QNetworkReply* reply = sharedManager().head(request);

    connect(reply, &QNetworkReply::finished, reply, [reply, p = std::move(promise)]() mutable {
        reply->deleteLater();

        if (reply->error() == QNetworkReply::NoError) {
            auto lengthHeader = reply->header(QNetworkRequest::ContentLengthHeader);
            if (lengthHeader.isValid()) {
                p.addResult(lengthHeader.toULongLong());
                p.finish();
                return;
            }
            else {
                p.setException(std::make_exception_ptr(std::runtime_error("Content-Length header not present")));
                return;
            }
        }

        p.setException(std::make_exception_ptr(std::runtime_error(reply->errorString().toStdString())));
    });

    return future;
}

QFuture<QDateTime> FileDownloader::getLastModifiedAsync(const QUrl& url)
{
    QPromise<QDateTime> promise;
    QFuture<QDateTime> future = promise.future();

    auto manager    = new QNetworkAccessManager();
    auto reply      = manager->head(QNetworkRequest(url));

    QNetworkRequest request(url);

	request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    connect(reply, &QNetworkReply::finished, [reply, manager, promise = std::move(promise)]() mutable {
        reply->deleteLater();
        manager->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            const auto error = reply->errorString();

            promise.setException(std::make_exception_ptr(std::runtime_error(QString("HEAD request failed: %1").arg(error).toStdString())));
            promise.finish();
            return;
        }

        const auto lastModifiedVar = reply->header(QNetworkRequest::LastModifiedHeader);

        if (!lastModifiedVar.isValid()) {
            promise.setException(std::make_exception_ptr(std::runtime_error("Last-Modified header not found")));
            promise.finish();
            return;
        }

        promise.addResult(lastModifiedVar.toDateTime());
        promise.finish();
    });

    return future;
}

QByteArray FileDownloader::downloadedData() const {
    return _downloadedData;
}

}
