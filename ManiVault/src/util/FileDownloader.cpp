// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "FileDownloader.h"

namespace mv::util {

FileDownloader::FileDownloader(QObject* parent) :
    QObject(parent)
{
    connect(&_networkAccessManager, &QNetworkAccessManager::finished, this, &FileDownloader::fileDownloaded);
}

void FileDownloader::download(const QUrl& url)
{
    QNetworkRequest request(url);

    _networkAccessManager.get(request);
}

void FileDownloader::fileDownloaded(QNetworkReply* reply)
{
    _downloadedData = reply->readAll();

    reply->deleteLater();

    emit downloaded();
}

QByteArray FileDownloader::downloadedData() const {
    return _downloadedData;
}

}
