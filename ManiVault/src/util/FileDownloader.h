// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QObject>
#include <QUrl>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace mv::util {

/**
 * File downloader class
 * Help class for simplifying file download
 * 
 * @author Thomas Kroes
 */
class FileDownloader : public QObject
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    explicit FileDownloader(QObject* parent = 0);

    /** No need for custom destructor */
    virtual ~FileDownloader() = default;

    /**
     * Download file with \p url
     * @param url URL of the file to download
     */
    void download(const QUrl& url);

    /**
     * Get downloaded data
     * @return Downloaded data as byte array
     */
    QByteArray downloadedData() const;

signals:

    /** Signals that the file is correctly downloaded */
    void downloaded();

private slots:

    /**
     * Invoked when the file is downloaded
     * @param reply Pointer to the network reply
     */
    void fileDownloaded(QNetworkReply* reply);

private:
    QNetworkAccessManager   _networkAccessManager;      /** For network access */
    QByteArray              _downloadedData;            /** Downloaded data as byte array */
};

}
