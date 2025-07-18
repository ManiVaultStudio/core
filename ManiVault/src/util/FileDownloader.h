// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "Task.h"

#include <QObject>
#include <QUrl>
#include <QByteArray>
#include <QNetworkReply>
#include <QPointer>
#include <QFuture>

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

    /**
     * Get the shared network access manager instance
     * @return Reference to the shared QNetworkAccessManager instance
     */
	static QNetworkAccessManager& sharedManager()
    {
        static QNetworkAccessManager instance;
        return instance;
    }

    /** 
     * Download file to byte array asynchronously
     * @param url URL of the file to download
     * @param task Optional task to associate with the download operation (must live in the main/GUI thread)
     * @return Future containing the downloaded data as byte array
     */
    static QFuture<QByteArray> downloadToByteArrayAsync(const QUrl& url, Task* task = nullptr);

    /** 
     * Download file to byte array synchronously
     * @param url URL of the file to download
     * @param task Optional task to associate with the download operation (must live in the main/GUI thread)
     * @return Downloaded data as byte array
     */
    static QByteArray downloadToByteArraySync(const QUrl& url, Task* task = nullptr);

    /**
     * Download file from \p url to \p targetDirectory asynchronously
     * @param url URL of the file to download
     * @param targetDirectory Directory where the file should be saved (OS temporary dir when empty)
     * @param task Optional task to associate with the download operation (must live in the main/GUI thread)
     * @return Future containing the path to the downloaded file
     */
    static QFuture<QString> downloadToFileAsync(const QUrl& url, const QString& targetDirectory = "", Task* task = nullptr);

    /**
     * Download file from \p url to \p targetDirectory synchronously
     * @param url URL of the file to download
     * @param targetDirectory Directory where the file should be saved (OS temporary dir when empty)
     * @param task Optional task to associate with the download operation (must live in the main/GUI thread)
     * @return File path of the downloaded file
     */
    static QString downloadToFileSync(const QUrl& url, const QString& targetDirectory = "", Task* task = nullptr);

    /**
     * Get the size of the file to be downloaded asynchronously
     * @param url URL of the file to be downloaded
     * @return Size of the file to be downloaded in bytes, 0 if size cannot be determined
     */
    static QFuture<std::uint64_t> getDownloadSizeAsync(const QUrl& url);

    /**
     * Get the size of the file to be downloaded synchronously
     * @param url URL of the file to be downloaded
     * @return Size of the file to be downloaded in bytes, 0 if size cannot be determined
     */
    static std::uint64_t getDownloadSizeSync(const QUrl& url);

    /**
     * Get the last modified date of the file at \p url asynchronously
     * @param url URL of the file to check
     * @return Last modified date of the file, QDateTime() if it cannot be determined
     */
    static QFuture<QDateTime> getLastModifiedAsync(const QUrl& url);

    /**
     * Get the last modified date of the file at \p url synchronously
     * @param url URL of the file to check
     * @return Last modified date of the file, QDateTime() if it cannot be determined
     */
    static QDateTime getLastModifiedSync(const QUrl& url);
};

}
