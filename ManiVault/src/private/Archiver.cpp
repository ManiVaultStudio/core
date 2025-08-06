// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Archiver.h"

#include <util/Exception.h>

#include <stdexcept>

#include <QDebug>
#include <QDir>
#include <QThread>

#include <quazip/JlCompress.h>

namespace mv {

namespace util {

void Archiver::compressDirectory(const QString& sourceDirectory, const QString& compressedFilePath, bool recursive /*= true*/, std::int32_t compressionLevel /*= 0*/, const QString& password /*= ""*/, QDir::Filters filters /*= QDir::Filter::Files*/)
{
    // Clean up and throw exception if error(s) occurred
    const auto except = [&compressedFilePath](const QString& errorMessage) {

        // Remove destination file
        QFile::remove(compressedFilePath);

        // Except
        throw std::runtime_error(errorMessage.toLatin1());
    };

    // Compressed file
    QuaZip zip(compressedFilePath);

    // Create the destination file
    if (!QDir().mkpath(QFileInfo(compressedFilePath).absolutePath()))
        except(QString("Unable to make path %1").arg(QFileInfo(compressedFilePath).absolutePath()));

    // Except if not created
    if (!zip.open(QuaZip::mdCreate))
        except("Unable to create destination file");

    // Except if a zip error occurred
    if (zip.getZipError() != 0)
        except("Zip error(s) occurred");

    // Compress the subdirectory
    Archiver::compressSubDirectory(&zip, sourceDirectory, sourceDirectory, recursive, compressionLevel, password, filters);

    // Notify others that directory compression started
    emit taskStarted("Save to disk");

    // Close the zip file
    zip.close();

    // Notify others that directory compression finished
    emit taskFinished("Save to disk");
}

void Archiver::decompress(const QString& compressedFile, const QString& destinationDirectory, const QString& password /*= ""*/)
{
    // Files that were extracted during decompression
    QStringList extracted;

    try
    {
        QuaZip zip(compressedFile);

        // Except if unable to open the zip file
        if (!zip.open(QuaZip::mdUnzip))
            throw std::runtime_error("Unable to open ZIP file");

        // Clean the destination directory
        QString cleanDir = QDir::cleanPath(destinationDirectory);

        QDir directory(cleanDir);

        // Get absolute path to directory
        QString absoluteCleanDir = directory.absolutePath();

        // Sanity check
        if (!absoluteCleanDir.endsWith('/'))
            absoluteCleanDir += '/';

        // Exit prematurely if there is no first file
        if (!zip.goToFirstFile())
            throw std::runtime_error("No files found");

        do {
            const auto currentFileName      = zip.getCurrentFileName();
            const auto absoluteFilePath     = directory.absoluteFilePath(currentFileName);
            const auto absoluteCleanPath    = QDir::cleanPath(absoluteFilePath);

            if (!absoluteCleanPath.startsWith(absoluteCleanDir))
                continue;

            // Extract a single file to the target directory
            extractFile(&zip, QLatin1String(""), absoluteFilePath, password);

            // Add absolute file path to the list of extracted files
            extracted.append(absoluteFilePath);
        } while (zip.goToNextFile());

        // Close the compressed file
        zip.close();

        // Except if a zip error occurred
        if (zip.getZipError() != 0)
            throw std::runtime_error("Decompression error occurred");
    }
    catch (...)
    {
        // Remove extracted files
        removeFiles(extracted);
    }
}

QStringList Archiver::getTaskNamesForDirectoryCompression(const QString& directory, bool recursive /*= true*/, std::int32_t level /*= 0*/)
{
    // For scanning files in the directory
    QDir filesInDirectory(directory);

    // Task names
    QStringList taskNames;

    // Add root task
    if (level == 0)
        taskNames << "Save to disk";

    // Add file paths of the files directly below the directory
    for (const auto fileInDirectory : filesInDirectory.entryList(QDir::Files | QDir::NoDotAndDotDot))
        taskNames << fileInDirectory;

    // For scanning subdirectories in the directory
    QDir subDirectoriesInDirectory(directory);

    // Add number of files of sub directories if required
    if (recursive) {
        for (const auto subDirectory : subDirectoriesInDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
            taskNames << getTaskNamesForDirectoryCompression(subDirectory, recursive, level);
    }

    return taskNames;
}

QStringList Archiver::getTaskNamesForDecompression(const QString& compressedFilePath)
{
    QuaZip zip(compressedFilePath);

    // Except if unable to open the zip file
    if (!zip.open(QuaZip::mdUnzip))
        throw std::runtime_error("Unable to open ZIP file");

    return zip.getFileNameList();
}

void Archiver::extractSingleFile(const QString& compressedFilePath, const QString& sourceFileName, const QString& targetFilePath, const QString& password /*= ""*/)
{
    QuaZip zip(compressedFilePath);

    if (!zip.open(QuaZip::mdUnzip))
        throw std::runtime_error("Unable to open ZIP file");

    const auto directory = QFileInfo(targetFilePath).absoluteDir();

    if (!zip.goToFirstFile())
        throw std::runtime_error("No files found");

    do {
        const auto fileName = zip.getCurrentFileName();

        if (fileName != sourceFileName)
            continue;

        extractFile(&zip, QLatin1String(""), targetFilePath, password);
    } while (zip.goToNextFile());

    zip.close();

    if (zip.getZipError() != 0)
        throw std::runtime_error("Decompression error occurred");
}

void Archiver::compressSubDirectory(QuaZip* parentZip, const QString& directory, const QString& parentDirectory, bool recursive /*= true*/, std::int32_t compressionLevel /*= 0*/, const QString& password /*= ""*/, QDir::Filters filters /*= QDir::Filter::Files*/)
{
    // Except if the parent zip is invalid
    if (!parentZip)
        throw std::runtime_error("Invalid parent zip file");

    // Except if the parent zip mode is invalid
    if (parentZip->getMode() != QuaZip::mdCreate && parentZip->getMode() != QuaZip::mdAppend && parentZip->getMode() != QuaZip::mdAdd)
        throw std::runtime_error("Invalid parent zip mode");

    // Except if the directory does not exist
    if (!QDir(directory).exists())
        throw std::runtime_error("Directory does not exist");

    QDir origDirectory(parentDirectory);

    if (directory != parentDirectory) {

        // Create directory zip file
        QuaZipFile dirZipFile(parentZip);

        std::int32_t method = Z_DEFLATED, level = 0;

        // Attempt to open the directory zip file
        if (!dirZipFile.open(QIODevice::WriteOnly, QuaZipNewInfo(origDirectory.relativeFilePath(directory) + QLatin1String("/"), directory), password.isEmpty() ? nullptr : password.toLatin1(), 0U, Z_DEFLATED, compressionLevel, compressionLevel == -1))
            throw std::runtime_error("Unable to open directory zip file");

        // Close the directory zip file
        dirZipFile.close();
    }

    // Compress subdirectories if needed
    if (recursive) {

        // Get a list of subdirectories to compress
        QFileInfoList files = QDir(directory).entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | filters);

        // Compress all sub directories
        for (const auto file : files) {

            // Continue if not a directory
            if (!file.isDir())
                continue;

            // Compress subdirectory
            compressSubDirectory(parentZip, file.absoluteFilePath(), parentDirectory, recursive, compressionLevel, password, filters);
        }
    }

    // Get a list of files to compress
    QFileInfoList files = QDir(directory).entryInfoList(QDir::Files | filters);

    // Compress all files
    for (const auto file : files) {

        // Sanity check
        if (!file.isFile() || file.absoluteFilePath() == parentZip->getZipName())
            continue;

        // Establish file name of the file that needs to be compressed
        const auto filename = origDirectory.relativeFilePath(file.absoluteFilePath());

        // Compress the file
        compressFile(parentZip, file.absoluteFilePath(), filename, compressionLevel, password);
    }
}

void Archiver::compressFile(QuaZip* zip, const QString& sourceFilePath, const QString& compressedFilePath, std::int32_t compressionLevel /*= 0*/, const QString& password /*= ""*/)
{
    const auto taskName = compressedFilePath;

    // Notify others that a task started
    emit taskStarted(taskName);

    // Except if the zip is invalid
    if (!zip)
        throw std::runtime_error("Invalid zip file");

    // Except if the zip mode is invalid
    if (zip->getMode() != QuaZip::mdCreate && zip->getMode() != QuaZip::mdAppend && zip->getMode() != QuaZip::mdAdd)
        throw std::runtime_error("Invalid zip mode");

    // Create compressed file
    QuaZipFile compressedFile(zip);

    // Open compressed file and except if an error occurs
    if (!compressedFile.open(QIODevice::WriteOnly, QuaZipNewInfo(compressedFilePath, sourceFilePath), password.isEmpty() ? nullptr : password.toLocal8Bit().data(), 0U, Z_DEFLATED, compressionLevel))
        throw std::runtime_error("Unable to open zip file");

    QFileInfo input(sourceFilePath);

    if (quazip_is_symlink(input)) {
        const auto path         = quazip_symlink_target(input);
        const auto relativePath = input.dir().relativeFilePath(path);

        compressedFile.write(QFile::encodeName(relativePath));
    }
    else {

        // Create input file
        QFile inFile;

        // Set its file name
        inFile.setFileName(sourceFilePath);
        
        // Except if unable to open the input file
        if (!inFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open input file");

        // Except if unable to copy data
        if (!JlCompress::copyData(inFile, compressedFile) || compressedFile.getZipError() != UNZ_OK)
            throw std::runtime_error("Unable to copy data");

        // Close the input file
        inFile.close();
    }

    // Close the compressed file
    compressedFile.close();

    // Except if zipping error(s) occurred
    if (compressedFile.getZipError() != UNZ_OK)
        throw std::runtime_error("Zip error(s) occurred");

    // Notify others that a task finished
    emit taskFinished(taskName);
}

void Archiver::extractFile(QuaZip* zip, const QString& compressedFilePath, const QString& targetFilePath, const QString& password /*= ""*/)
{
    // Establish task name
    const auto taskName = QFileInfo(targetFilePath).fileName();

    // Notify others that a task started
    emit taskStarted(taskName);

    if (!zip)
        throw std::runtime_error("Invalid input data");

    if (zip->getMode() != QuaZip::mdUnzip)
        throw std::runtime_error("Invalid decompression mode");

    if (!compressedFilePath.isEmpty())
        zip->setCurrentFile(compressedFilePath);

    QuaZipFile inFile(zip);

    if (!inFile.open(QIODevice::ReadOnly, password.isEmpty() ? nullptr : password.toLocal8Bit().data()) || inFile.getZipError() != UNZ_OK)
        throw std::runtime_error("Decompression error(s) occurred");

    QDir curDir;

    if (targetFilePath.endsWith(QLatin1String("/"))) {
        if (!curDir.mkpath(targetFilePath))
            throw std::runtime_error("Unable to create target file");
    }
    else {
        if (!curDir.mkpath(QFileInfo(targetFilePath).absolutePath()))
            throw std::runtime_error("Unable to create target file");
    }

    QuaZipFileInfo64 info;

    if (!zip->getCurrentFileInfo(&info))
        throw std::runtime_error("Unable to retrieve file info");

    QFile::Permissions srcPerm = info.getPermissions();

    if (targetFilePath.endsWith(QLatin1String("/")) && QFileInfo(targetFilePath).isDir()) {
        if (srcPerm != 0)
            QFile(targetFilePath).setPermissions(srcPerm);
        
        return;
    }

    if (info.isSymbolicLink()) {
        QString target = QFile::decodeName(inFile.readAll());

        if (!QFile::link(target, targetFilePath))
            throw std::runtime_error("Unable to create symbolic link");

        return;
    }

    QFile outFile;

    outFile.setFileName(targetFilePath);
    
    if (!outFile.open(QIODevice::WriteOnly))
        throw std::runtime_error("Unable to open target file for writing");

    if (!JlCompress::copyData(inFile, outFile) || inFile.getZipError() != UNZ_OK) {
        outFile.close();
        removeFiles(QStringList(targetFilePath));
        throw std::runtime_error("Unable to copy data");
    }

    // Close input/output files
    outFile.close();
    inFile.close();

    if (inFile.getZipError() != UNZ_OK) {
        removeFiles(QStringList(targetFilePath));
        throw std::runtime_error("ZIP error(s) occurred");
    }

    if (srcPerm != 0)
        outFile.setPermissions(srcPerm);

    // Notify others that a task finished
    emit taskFinished(taskName);
}

void Archiver::removeFiles(const QStringList& filesToRemove)
{
    // Remove files in the list
    for (const auto& file : filesToRemove)
        QFile::remove(file);
}

}
}
