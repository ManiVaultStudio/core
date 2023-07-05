// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QObject>
#include <QString>
#include <QDir>
#include <QCryptographicHash>

class QuaZip;

namespace hdps::util {

/**
 * Archiver class
 *
 * Class for archiving files and directories (wraps QuaZip)
 *
 * @author Thomas Kroes
 */
class Archiver : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    Archiver() = default;
 
    /**
     * Compresses an entire directory and possibly password-protects it
     * @param sourceDirectory Path of the source directory
     * @param compressedFilePath File path of the compressed destination file
     * @param recursive Whether to pack sub-directories as well or only files
     * @param compressionLevel Compression level (zero means no compression)
     * @param password Password string if files need to be secured
     * @param fileDoneFn Callback which is called when a file compression is complete
     * @param filters File include filter
     */
    void compressDirectory(const QString& sourceDirectory, const QString& compressedFilePath, bool recursive = true, std::int32_t compressionLevel = 0, const QString& password = "", QDir::Filters filters = QDir::Filter::Files);

    /**
     * Decompresses a compressed file to a destination directory
     * @param compressedFile Path of the compressed source file
     * @param destinationDirectory Path of the destination directory where files will be extracted
     * @param password Password string if files need to be secured
     * @return Boolean indicating whether decompression was successful
     */
    void decompress(const QString& compressedFile, const QString& destinationDirectory, const QString& password = "");

    /**
     * Get task names for directory compression
     * @param directory Path of the directory
     * @param recursive Whether to recursively scan for sub-directories
     * @param level Directory level
     * @return String list consisting of the task names for the files that will be compressed
     */
    QStringList getTaskNamesForDirectoryCompression(const QString& directory, bool recursive = true, std::int32_t level = 0);

    /**
     * Get task names for archive decompression
     * @param compressedFilePath File path of the compressed file
     * @return String list consisting of the task names for the files that will be compressed
     */
    QStringList getTaskNamesForDecompression(const QString& compressedFilePath);

    /**
     * Extracts a file
     * Might throw a std::runtime_error exception if an error occurs during extraction
     * @param compressedFilePath File path of the compressed input file
     * @param sourceFileName File name of the compressed source file
     * @param targetFilePath Path of the extracted target file
     * @param password Password string if the compressed file is encrypted with a password
     */
    void extractSingleFile(const QString& compressedFilePath, const QString& sourceFileName, const QString& targetFilePath, const QString& password = "");

protected:

    /**
     * Compresses a sub directory
     * @param parentZip Opened zip containing the parent directory
     * @param directory The full path to the directory to pack
     * @param parentDirectory The full path to the directory corresponding to the root of the ZIP
     * @param recursive Whether to pack sub-directories as well or only files
     * @param compressionLevel Compression level (zero means no compression)
     * @param password Password string if files need to be secured
     * @param filters File include filter
     */
    void compressSubDirectory(QuaZip* parentZip, const QString& directory, const QString& parentDirectory, bool recursive = true, std::int32_t compressionLevel = 0, const QString& password = "", QDir::Filters filters = QDir::Filter::Files);

    /**
     * Compresses a file
     * @param zip Pointer to quazip instance
     * @param sourceFilePath Path of the source file
     * @param compressedFilePath Path of the compressed file
     * @param compressionLevel Compression level (zero means no compression)
     * @param password Password string if files need to be secured
     */
    void compressFile(QuaZip* zip, const QString& sourceFilePath, const QString& compressedFilePath, std::int32_t compressionLevel = 0, const QString& password = "");

    /**
     * Extracts a file
     * @param zip Pointer to quazip instance
     * @param compressedFilePath File path of the compressed input file
     * @param targetFilePath Path of the extracted target file
     * @param password Password string if the compressed file is encrypted with a password
     */
    void extractFile(QuaZip* zip, const QString& compressedFilePath, const QString& targetFilePath, const QString& password = "");

    /**
     * Removes a file
     * @param filesToRemove Files to remove
     */
    void removeFiles(const QStringList& filesToRemove);

signals:

    /**
     * Signals that a task has started
     * @param taskName Name of the task that started
     */
    void taskStarted(const QString& taskName);

    /**
     * Signals that a task has finished
     * @param taskName Name of the task that finished
     */
    void taskFinished(const QString& taskName);
};

}
