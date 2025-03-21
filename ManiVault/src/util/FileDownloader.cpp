// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "FileDownloader.h"

#include "Task.h"
#include "Application.h"

#ifdef _DEBUG
	#define FILE_DOWNLOADER_VERBOSE
#endif

namespace mv::util {

FileDownloader::FileDownloader(const StorageMode& mode, const Task::GuiScope& taskGuiScope, QObject* parent) :
    QObject(parent),
    _storageMode(mode),
    _isDownloading(false),
    _task(this, "Downloading", { taskGuiScope }, Task::Status::Undefined, true)
{
    connect(&_networkAccessManager, &QNetworkAccessManager::finished, this, &FileDownloader::downloadFinished);

    _task.setEnabled(taskGuiScope != Task::GuiScope::None);
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

    auto* networkReply = _networkAccessManager.get(request);

    const auto fileName = QFileInfo(_url.toString()).fileName();

    _task.setName(QString("Download %1").arg(fileName));
    _task.setIcon(StyledIcon("download"));
    _task.setRunning();

    connect(&_task, &Task::requestAbort, this, [this, networkReply]() -> void {
        networkReply->abort();

        _task.setAborted();

        emit aborted();
    });

    connect(networkReply, &QNetworkReply::downloadProgress, this, [this, networkReply](qint64 downloaded, qint64 total) -> void {
        if (_task.isAborting())
            return;
			
        const auto progress = static_cast<float>(downloaded) / static_cast<float>(total);

        _task.setProgress(progress);

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

    if (_storageMode & StorageMode::File)
    {
        const auto fileName = QFileInfo(_url.toString()).fileName();

    	_downloadedFilePath = QDir(Application::current()->getTemporaryDir().path()).filePath(fileName);

#ifdef FILE_DOWNLOADER_VERBOSE
        qDebug() << fileName << _downloadedFilePath;
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
        _task.setFinished();

		emit downloaded();
    }
}

QByteArray FileDownloader::downloadedData() const {
    return _downloadedData;
}

}
