// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "FileDownloader.h"

#ifdef _DEBUG
	#define FILE_DOWNLOADER_VERBOSE
#endif

namespace mv::util {

FileDownloader::FileDownloader(const StorageMode& mode, QObject* parent) :
    QObject(parent),
    _storageMode(mode),
    _isDownloading(false),
    _task(this, "Downloading")
{
    connect(&_networkAccessManager, &QNetworkAccessManager::finished, this, &FileDownloader::downloadFinished);
}

void FileDownloader::download(const QUrl& url)
{
    if (_isDownloading)
        return;

    _url            = url;
    _isDownloading  = true;

    QNetworkRequest request(_url);

    auto* networkReply = _networkAccessManager.get(request);

    _task.setRunning();

    connect(networkReply, &QNetworkReply::downloadProgress, this, [this](qint64 downloaded, qint64 total) -> void {
        _task.setProgress(static_cast<float>(downloaded) / static_cast<float>(total));
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

    _task.setFinished();

    emit downloaded();
}

QByteArray FileDownloader::downloadedData() const {
    return _downloadedData;
}

}
