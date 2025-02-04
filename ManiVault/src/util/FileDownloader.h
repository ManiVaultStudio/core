// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "ModalTask.h"

#include <QObject>
#include <QUrl>
#include <QByteArray>
#include <QNetworkReply>

namespace mv::util {

/**
 * File downloader class
 * Help class for simplifying file download
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT FileDownloader : public QObject
{
    Q_OBJECT

public:

    /** Describes the storage modes */
    enum StorageMode {
        File        = 0x00001,      /** Stored as a file */
        ByteArray    = 0x00002,     /** Stored as a byte array */

        All = File | ByteArray
    };

    /**
     * Construct with \p mode and pointer to \p parent object
     * @param mode Download mode
     * @param taskGuiScope Type of GUI task reporting
     * @param parent Pointer to parent object
     */
    explicit FileDownloader(const StorageMode& mode = StorageMode::All, const Task::GuiScope& taskGuiScope = Task::GuiScope::None, QObject* parent = nullptr);

    /** No need for custom destructor */
    virtual ~FileDownloader() = default;

    /**
     * Download file with \p url
     * @param url URL of the file to download
     */
    void download(const QUrl& url);

    /**
     * Get whether a download is taking place
     * @return Boolean determining whether a download is taking place
     */
	bool isDownloading() const;

    /**
     * Get downloaded file path
     * @return Downloaded file path
     */
    QString getDownloadedFilePath() const;

    /**
     * Get downloaded data
     * @return Downloaded data as byte array
     */
    QByteArray downloadedData() const;

    /**
     * Invoked when the file is downloaded
     * @param reply Pointer to the network reply
     */
    void downloadFinished(QNetworkReply* reply);

signals:

    /** Signals that the file is correctly downloaded */
    void downloaded();

    /** Signals that the file download process was aborted (by the user) */
    void aborted();

    /**
     * Invoked when there is download progress update
     * @param progress Progress [0, 1]
     */
    void downloadProgress(float progress);

private:
    const StorageMode       _storageMode;               /** Download mode */
    QNetworkAccessManager   _networkAccessManager;      /** For network access */
    QUrl                    _url;                       /** URL being downloaded */
    bool                    _isDownloading;             /** Boolean determining whether a download is taking place */
    QByteArray              _downloadedData;            /** Downloaded data as byte array */
    QString                 _downloadedFilePath;        /** Location where the downloaded file is stored */
    Task                    _task;                      /** Task for reporting progress */
};

}
