// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Archiver.h"

#include <Application.h>
#include <util/Exception.h>

#include <stdexcept>
#include <algorithm>
#include <limits>
#include <deque>
#include <condition_variable>
#include <exception>
#include <mutex>
#include <thread>

#include <QDebug>
#include <QDir>
#include <QThread>
#include <QMutexLocker>
#include <QSet>
#include <QFile>
#include <QFileInfo>
#include <QUuid>
#include <QDirIterator>
#include <QElapsedTimer>
#include <QStorageInfo>
#include <QCryptographicHash>
#include <QHash>
#include <QSaveFile>
#include <QTemporaryDir>

#include <quazip/JlCompress.h>

namespace mv {

namespace util {

namespace {

/**
 * Establish whether a path is contained by a directory after resolving the
 * existing part of the path. A plain string-prefix check is insufficient:
 * /tmp/archive-escape also starts with /tmp/archive.
 */
QString canonicalPathWithNonExistingTail(const QString& path)
{
    QFileInfo current(path);
    QStringList nonExistingTail;

    while (!current.exists()) {
        nonExistingTail.prepend(current.fileName());

        const auto parentPath = current.absolutePath();

        if (parentPath == current.absoluteFilePath())
            return {};

        current = QFileInfo(parentPath);
    }

    auto canonicalPath = current.canonicalFilePath();

    if (canonicalPath.isEmpty())
        return {};

    for (const auto& pathPart : nonExistingTail)
        canonicalPath = QDir(canonicalPath).filePath(pathPart);

    return QDir::cleanPath(canonicalPath);
}

bool isPathWithinDirectory(const QString& path, const QString& directory)
{
    const auto canonicalDirectory = canonicalPathWithNonExistingTail(directory);
    const auto canonicalPath      = canonicalPathWithNonExistingTail(path);

    if (canonicalDirectory.isEmpty() || canonicalPath.isEmpty())
        return false;

    const auto directoryPrefix = canonicalDirectory + QDir::separator();

    return canonicalPath == canonicalDirectory || canonicalPath.startsWith(directoryPrefix);
}

std::uint64_t getArchiveEntriesSize(const QString& archivePath, const QStringList& entryNames)
{
    if (entryNames.isEmpty())
        return 0;

    QuaZip zip(archivePath);
    if (!zip.open(QuaZip::mdUnzip))
        throw std::runtime_error(QString("Unable to open source project archive: %1").arg(archivePath).toStdString());

    QSet<QString> remainingEntries;
    for (const auto& entryName : entryNames)
        remainingEntries.insert(entryName);

    std::uint64_t totalSize = 0;
    if (!zip.goToFirstFile())
        throw std::runtime_error("No files found in source project archive");

    do
    {
        const auto currentFileName = zip.getCurrentFileName();
        if (!remainingEntries.contains(currentFileName))
            continue;

        QuaZipFileInfo64 fileInfo;
        if (!zip.getCurrentFileInfo(&fileInfo))
            throw std::runtime_error("Unable to read packed file information from source project archive");

        totalSize += static_cast<std::uint64_t>(fileInfo.uncompressedSize);
        remainingEntries.remove(currentFileName);
    } while (zip.goToNextFile() && !remainingEntries.isEmpty());

    zip.close();
    if (!remainingEntries.isEmpty())
        throw std::runtime_error(QString("Source project archive is missing %1 packed tile entry(s)").arg(remainingEntries.size()).toStdString());

    return totalSize;
}

struct ArchiveIoMetrics
{
    qint64 sourceReadMs = 0;
    qint64 zipWriteMs = 0;
    std::uint64_t sourceReadBytes = 0;
    std::uint64_t zipWriteBytes = 0;
    std::uint64_t readOperations = 0;
};

thread_local ArchiveIoMetrics* activeArchiveIoMetrics = nullptr;

void writeArchiveData(QuaZipFile& destinationFile, const QByteArray& data, const QString& entryName)
{
    QElapsedTimer timer;
    if (activeArchiveIoMetrics)
        timer.start();
    qint64 bytesWritten = 0;
    while (bytesWritten < data.size())
    {
        const auto remainingBytes = data.size() - bytesWritten;
        const auto writeResult = destinationFile.write(data.constData() + bytesWritten, remainingBytes);
        if (writeResult <= 0)
        {
            throw std::runtime_error(QString("Unable to write archive entry %1: %2 (ZIP error %3; wrote %4 of %5 bytes)")
                .arg(entryName)
                .arg(destinationFile.errorString())
                .arg(destinationFile.getZipError())
                .arg(bytesWritten)
                .arg(data.size())
                .toStdString());
        }
        bytesWritten += writeResult;
    }
    if (activeArchiveIoMetrics) {
        activeArchiveIoMetrics->zipWriteMs += timer.elapsed();
        activeArchiveIoMetrics->zipWriteBytes += static_cast<std::uint64_t>(data.size());
    }
}

void pipeArchiveSourceToZip(QIODevice& source, QuaZipFile& destination, const QString& entryName, const std::function<void(std::uint64_t)>& bytesWritten)
{
    constexpr std::size_t chunkSize = 1024 * 1024;
    constexpr std::size_t maxQueuedChunks = 8;
    struct ReadAheadQueue {
        std::mutex mutex;
        std::condition_variable hasData;
        std::condition_variable hasSpace;
        std::deque<QByteArray> chunks;
        bool done = false;
        bool cancelled = false;
        std::exception_ptr error;
        qint64 readMs = 0;
        std::uint64_t readBytes = 0;
        std::uint64_t readOperations = 0;
    } queue;

    std::thread reader([&source, &queue, chunkSize, maxQueuedChunks]() {
        try {
            while (!source.atEnd()) {
                QElapsedTimer timer;
                timer.start();
                auto data = source.read(static_cast<qint64>(chunkSize));
                const auto readMs = timer.elapsed();
                if (data.isEmpty() && !source.atEnd())
                    throw std::runtime_error(QString("Unable to read archive source: %1").arg(source.errorString()).toStdString());

                if (data.isEmpty())
                    break;

                std::unique_lock<std::mutex> lock(queue.mutex);
                queue.hasSpace.wait(lock, [&queue, maxQueuedChunks]() { return queue.chunks.size() < maxQueuedChunks || queue.cancelled; });
                if (queue.cancelled)
                    break;
                queue.readMs += readMs;
                queue.readBytes += static_cast<std::uint64_t>(data.size());
                ++queue.readOperations;
                queue.chunks.push_back(std::move(data));
                lock.unlock();
                queue.hasData.notify_one();
            }
        } catch (...) {
            std::lock_guard<std::mutex> lock(queue.mutex);
            queue.error = std::current_exception();
        }
        {
            std::lock_guard<std::mutex> lock(queue.mutex);
            queue.done = true;
        }
        queue.hasData.notify_all();
        queue.hasSpace.notify_all();
    });

    try {
        while (true) {
            QByteArray data;
            {
                std::unique_lock<std::mutex> lock(queue.mutex);
                queue.hasData.wait(lock, [&queue]() { return !queue.chunks.empty() || queue.done; });
                if (queue.chunks.empty())
                    break;
                data = std::move(queue.chunks.front());
                queue.chunks.pop_front();
            }
            queue.hasSpace.notify_one();
            writeArchiveData(destination, data, entryName);
            if (bytesWritten)
                bytesWritten(static_cast<std::uint64_t>(data.size()));
        }
    } catch (...) {
        {
            std::lock_guard<std::mutex> lock(queue.mutex);
            queue.cancelled = true;
        }
        queue.hasSpace.notify_all();
        reader.join();
        throw;
    }

    reader.join();
    if (activeArchiveIoMetrics) {
        activeArchiveIoMetrics->sourceReadMs += queue.readMs;
        activeArchiveIoMetrics->sourceReadBytes += queue.readBytes;
        activeArchiveIoMetrics->readOperations += queue.readOperations;
    }
    if (queue.error)
        std::rethrow_exception(queue.error);
}

}

void Archiver::compressDirectory(const QString& sourceDirectory, const QString& compressedFilePath, bool recursive /*= true*/, std::int32_t compressionLevel /*= 0*/, const QString& password /*= ""*/, QDir::Filters filters /*= QDir::Filter::Files*/, const QString& sourceArchivePath /*= ""*/, const std::function<void(float)>& progress /*= {}*/)
{
    QElapsedTimer archiveTimer;
    archiveTimer.start();
    ArchiveIoMetrics ioMetrics;
    activeArchiveIoMetrics = &ioMetrics;
    const auto metricsGuard = qScopeGuard([]() { activeArchiveIoMetrics = nullptr; });
    const auto manifestPath = sourceDirectory + QStringLiteral(".packed-archive-entries");
    QStringList archiveEntriesToCopy;
    QFile manifest(manifestPath);
    if (manifest.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while (!manifest.atEnd())
        {
            const auto entryName = QString::fromUtf8(manifest.readLine()).trimmed();
            if (!entryName.isEmpty())
                archiveEntriesToCopy.push_back(entryName);
        }
        manifest.close();
    }

    auto archiveOutputPath = compressedFilePath;
    const auto sourceAbsolutePath = QDir::cleanPath(QFileInfo(sourceArchivePath).absoluteFilePath());
    const auto destinationAbsolutePath = QDir::cleanPath(QFileInfo(compressedFilePath).absoluteFilePath());
#ifdef Q_OS_WIN
    const auto sameSourceAndDestination = !archiveEntriesToCopy.isEmpty() && !sourceArchivePath.isEmpty() && QString::compare(sourceAbsolutePath, destinationAbsolutePath, Qt::CaseInsensitive) == 0;
#else
    const auto sameSourceAndDestination = !archiveEntriesToCopy.isEmpty() && !sourceArchivePath.isEmpty() && sourceAbsolutePath == destinationAbsolutePath;
#endif
    if (sameSourceAndDestination)
        archiveOutputPath += QStringLiteral(".save-") + QUuid::createUuid().toString(QUuid::WithoutBraces);

    std::uint64_t totalBytes = 0;
    QDirIterator fileIterator(sourceDirectory, filters, QDirIterator::Subdirectories);
    while (fileIterator.hasNext())
    {
        const QFileInfo fileInfo(fileIterator.next());
        if (fileInfo.isFile())
            totalBytes += static_cast<std::uint64_t>(fileInfo.size());
    }
    totalBytes += getArchiveEntriesSize(sourceArchivePath, archiveEntriesToCopy);

    const QStorageInfo destinationStorage(QFileInfo(archiveOutputPath).absolutePath());
    constexpr std::uint64_t archiveMetadataReserveBytes = 16ULL * 1024ULL * 1024ULL;
    if (destinationStorage.isValid() && destinationStorage.isReady() && destinationStorage.bytesAvailable() >= 0 && totalBytes <= std::numeric_limits<std::uint64_t>::max() - archiveMetadataReserveBytes)
    {
        const auto requiredBytes = totalBytes + archiveMetadataReserveBytes;
        const auto availableBytes = static_cast<std::uint64_t>(destinationStorage.bytesAvailable());
        if (availableBytes < requiredBytes)
        {
            throw std::runtime_error(QString("Insufficient free space to save project archive: approximately %1 GiB required, %2 GiB available on %3")
                .arg(static_cast<double>(requiredBytes) / (1024.0 * 1024.0 * 1024.0), 0, 'f', 1)
                .arg(static_cast<double>(availableBytes) / (1024.0 * 1024.0 * 1024.0), 0, 'f', 1)
                .arg(destinationStorage.rootPath())
                .toStdString());
        }
    }

    std::uint64_t processedBytes = 0;
    QElapsedTimer progressTimer;
    progressTimer.start();
    if (progress)
        progress(0.0f);

    const auto bytesWritten = [&](const std::uint64_t byteCount) {
        processedBytes += byteCount;
        if (progress && (progressTimer.elapsed() >= 50 || processedBytes >= totalBytes))
        {
            progress(totalBytes == 0 ? 1.0f : static_cast<float>(static_cast<double>(processedBytes) / static_cast<double>(totalBytes)));
            progressTimer.restart();
        }
    };

    // Clean up and throw exception if error(s) occurred
    const auto except = [&archiveOutputPath](const QString& errorMessage) {

        // Remove destination file
        QFile::remove(archiveOutputPath);

        // Except
        throw std::runtime_error(errorMessage.toLatin1());
    };

    // Compressed file
    QuaZip zip(archiveOutputPath);

    //// Create the destination file
    QDir().mkpath(QFileInfo(archiveOutputPath).absolutePath());

    // Except if not created
    if (!zip.open(QuaZip::mdCreate))
        except("Unable to create destination file");

    // Except if a zip error occurred
    if (zip.getZipError() != 0)
        except("Zip error(s) occurred");

    emit taskStarted("Save to disk");

    // Compress the sub directory
    // The writer remains serial; these timings indicate whether source reads or ZIP writes dominate.
    Archiver::compressSubDirectory(&zip, sourceDirectory, sourceDirectory, recursive, compressionLevel, password, filters, bytesWritten);

    if (!archiveEntriesToCopy.isEmpty())
    {
        if (sourceArchivePath.isEmpty())
            except("Packed archive entries were requested without a source project archive");
        copyArchiveEntries(&zip, sourceArchivePath, archiveEntriesToCopy, bytesWritten);
    }

    // Close the zip file
    zip.close();

    qInfo().noquote() << QStringLiteral("Archive I/O: %1 ms total; %2 MiB read in %3 ms (%4 reads); ZIP write time %5 ms; %6 MiB written; copied archive entries %7; compression level %8")
        .arg(archiveTimer.elapsed())
        .arg(static_cast<double>(ioMetrics.sourceReadBytes) / (1024.0 * 1024.0), 0, 'f', 1)
        .arg(ioMetrics.sourceReadMs)
        .arg(ioMetrics.readOperations)
        .arg(ioMetrics.zipWriteMs)
        .arg(static_cast<double>(ioMetrics.zipWriteBytes) / (1024.0 * 1024.0), 0, 'f', 1)
        .arg(archiveEntriesToCopy.size())
        .arg(compressionLevel);

    if (sameSourceAndDestination)
    {
        const auto backupPath = compressedFilePath + QStringLiteral(".backup-") + QUuid::createUuid().toString(QUuid::WithoutBraces);
        if (!QFile::rename(compressedFilePath, backupPath))
        {
            QFile::remove(archiveOutputPath);
            throw std::runtime_error("Unable to move the original project archive for replacement");
        }
        if (!QFile::rename(archiveOutputPath, compressedFilePath))
        {
            QFile::rename(backupPath, compressedFilePath);
            QFile::remove(archiveOutputPath);
            throw std::runtime_error("Unable to replace the project archive with the saved archive");
        }
        QFile::remove(backupPath);
    }

    QFile::remove(manifestPath);
    if (progress)
        progress(1.0f);

    // Notify others that directory compression finished
    emit taskFinished("Save to disk");
}

void Archiver::copyArchiveEntries(QuaZip* destinationZip, const QString& sourceArchivePath, const QStringList& entryNames, const std::function<void(std::uint64_t)>& bytesWritten)
{
    if (!destinationZip || destinationZip->getMode() != QuaZip::mdCreate)
        throw std::runtime_error("Invalid destination ZIP archive");

    QuaZip sourceZip(sourceArchivePath);
    if (!sourceZip.open(QuaZip::mdUnzip))
        throw std::runtime_error(QString("Unable to open source project archive: %1").arg(sourceArchivePath).toStdString());

    QSet<QString> remainingEntries;
    for (const auto& entryName : entryNames)
        remainingEntries.insert(entryName);

    for (const auto& entryName : remainingEntries)
    {
        if (entryName.isEmpty() || QFileInfo(entryName).fileName() != entryName || entryName == QStringLiteral(".") || entryName == QStringLiteral(".."))
            throw std::invalid_argument("Only root-level archive entries can be copied");

        if (!sourceZip.setCurrentFile(entryName))
            throw std::runtime_error(QString("Source project archive is missing packed tile entry: %1").arg(entryName).toStdString());

        QuaZipFile sourceFile(&sourceZip);
        if (!sourceFile.open(QIODevice::ReadOnly))
            throw std::runtime_error(QString("Unable to open packed tile entry: %1").arg(entryName).toStdString());

        QuaZipFile destinationFile(destinationZip);
        if (!destinationFile.open(QIODevice::WriteOnly, QuaZipNewInfo(entryName), nullptr, 0U, 0, 0))
            throw std::runtime_error(QString("Unable to create packed tile entry: %1").arg(entryName).toStdString());

        pipeArchiveSourceToZip(sourceFile, destinationFile, entryName, bytesWritten);

        destinationFile.close();
        sourceFile.close();
        if (sourceFile.getZipError() != UNZ_OK || destinationFile.getZipError() != UNZ_OK)
            throw std::runtime_error(QString("Failed while copying packed tile entry: %1").arg(entryName).toStdString());
    }

    sourceZip.close();
    if (sourceZip.getZipError() != 0)
        throw std::runtime_error("Error occurred while closing the source project archive");
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

        // Ensure the destination exists before resolving paths below. This
        // also lets the containment check detect symlinks in its parent path.
        if (!directory.exists() && !directory.mkpath(QStringLiteral(".")))
            throw std::runtime_error("Unable to create destination directory");

        const auto canonicalDestinationDirectory = directory.canonicalPath();

        if (canonicalDestinationDirectory.isEmpty())
            throw std::runtime_error("Unable to resolve destination directory");

        // Exit prematurely if there is no first file
        if (!zip.goToFirstFile())
            throw std::runtime_error("No files found");

        do {
            const auto currentFileName      = zip.getCurrentFileName();
            const auto absoluteFilePath     = directory.absoluteFilePath(currentFileName);
            const auto relativeFilePath    = directory.relativeFilePath(absoluteFilePath);

            if (relativeFilePath == QLatin1String("..")
                || relativeFilePath.startsWith(QLatin1String("../"))
                || relativeFilePath.startsWith(QLatin1String("..\\"))
                || !isPathWithinDirectory(absoluteFilePath, canonicalDestinationDirectory))
                throw std::runtime_error("Archive entry resolves outside destination directory");

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

    if (!zip.setCurrentFile(sourceFileName))
        throw std::runtime_error(QString("Unable to locate archive entry: %1").arg(sourceFileName).toStdString());

    extractFile(&zip, QLatin1String(""), targetFilePath, password);

    zip.close();

    if (zip.getZipError() != 0)
        throw std::runtime_error("Decompression error occurred");
}

void Archiver::extractSingleFileCancellable(const QString& compressedFilePath, const QString& sourceFileName, const QString& targetFilePath, const QString& password, const std::function<bool()>& cancellationCheck, const std::function<void(std::uint64_t, std::uint64_t)>& progress)
{
    QuaZip zip(compressedFilePath);
    if (!zip.open(QuaZip::mdUnzip))
        throw std::runtime_error(QString("Unable to open ZIP archive: %1").arg(compressedFilePath).toStdString());

    if (!zip.setCurrentFile(sourceFileName))
        throw std::runtime_error(QString("Unable to locate archive entry: %1").arg(sourceFileName).toStdString());

    QuaZipFile inputFile(&zip);
    const auto passwordBytes = password.toLocal8Bit();
    if (!inputFile.open(QIODevice::ReadOnly, password.isEmpty() ? nullptr : passwordBytes.constData()) || inputFile.getZipError() != UNZ_OK)
        throw std::runtime_error(QString("Unable to open archive entry: %1").arg(sourceFileName).toStdString());

    QuaZipFileInfo64 info;
    if (!zip.getCurrentFileInfo(&info))
        throw std::runtime_error(QString("Unable to read archive entry metadata: %1").arg(sourceFileName).toStdString());
    if (info.isSymbolicLink())
        throw std::runtime_error(QString("Symbolic links are not supported in archives: %1").arg(sourceFileName).toStdString());

    const auto destinationDirectory = QFileInfo(targetFilePath).absolutePath();
    if (!QDir().mkpath(destinationDirectory))
        throw std::runtime_error(QString("Unable to create destination directory for archive entry: %1").arg(sourceFileName).toStdString());

    QFile outputFile(targetFilePath);
    if (!outputFile.open(QIODevice::WriteOnly))
        throw std::runtime_error(QString("Unable to create staged archive entry: %1").arg(sourceFileName).toStdString());

    constexpr qint64 bufferSize = 1024 * 1024;
    QByteArray buffer(static_cast<qsizetype>(bufferSize), Qt::Uninitialized);
    std::uint64_t copiedBytes = 0;
    const auto totalBytes = static_cast<std::uint64_t>(info.uncompressedSize);
    try
    {
        if (progress)
            progress(0, totalBytes);

        for (;;)
        {
            if (cancellationCheck && cancellationCheck())
                throw std::runtime_error("Project resource extraction canceled");

            const auto bytesRead = inputFile.read(buffer.data(), buffer.size());
            if (bytesRead < 0)
                throw std::runtime_error("Failed while reading archive entry");
            if (bytesRead == 0)
                break;

            if (outputFile.write(buffer.constData(), bytesRead) != bytesRead)
                throw std::runtime_error("Failed while writing staged archive entry");

            copiedBytes += static_cast<std::uint64_t>(bytesRead);
            if (progress)
                progress(copiedBytes, totalBytes);
        }

        inputFile.close();
        outputFile.close();
        if (inputFile.getZipError() != UNZ_OK || zip.getZipError() != 0)
            throw std::runtime_error("ZIP error occurred while extracting archive entry");

        if (info.getPermissions() != 0)
            outputFile.setPermissions(info.getPermissions());
    }
    catch (...)
    {
        inputFile.close();
        outputFile.close();
        QFile::remove(targetFilePath);
        zip.close();
        throw;
    }

    zip.close();
}

QByteArray Archiver::readZipEntryToMemory(const QString& zipPath, const QString& entryName)
{
    static QMutex zipReadMutex;

    QByteArray data;

    QMutexLocker lock(&zipReadMutex);
    {
        QuaZip zip(zipPath);

        if (!zip.open(QuaZip::mdUnzip)) {
            throw std::runtime_error(
                QString("Unable to open ZIP archive: %1").arg(zipPath).toStdString());
        }

        if (!zip.setCurrentFile(entryName)) {
            throw std::runtime_error(
                QString("Unable to locate ZIP entry: %1").arg(entryName).toStdString());
        }

        QuaZipFile file(&zip);

        if (!file.open(QIODevice::ReadOnly)) {
            throw std::runtime_error(
                QString("Unable to open ZIP entry for reading: %1").arg(entryName).toStdString());
        }

        data = file.readAll();

        if (file.getZipError() != UNZ_OK) {
            throw std::runtime_error(
                QString("Failed while reading ZIP entry: %1").arg(entryName).toStdString());
        }
    }

	return data;
}

QByteArray Archiver::readZipEntryRangeToMemory(const QString& zipPath, const QString& entryName, const std::uint64_t offset, const std::uint64_t size)
{
    static QMutex zipReadMutex;
    struct StagedEntryCache
    {
        struct Entry
        {
            QString path;
            std::uint64_t size = 0;
            std::uint64_t lastAccess = 0;
        };

        QTemporaryDir directory { QDir(Application::current()->getTemporaryDir().path()).filePath(QStringLiteral("Zip/ZIP-XXXXXX")) };
        QHash<QString, Entry> entries;
        std::uint64_t cachedBytes = 0;
        std::uint64_t accessSequence = 0;
    };
    static StagedEntryCache stagedEntries;
    constexpr std::uint64_t stagedEntryCacheLimit = 8ull * 1024ull * 1024ull * 1024ull;

    if (offset > static_cast<std::uint64_t>(std::numeric_limits<qint64>::max()) || size > static_cast<std::uint64_t>(std::numeric_limits<qint64>::max()))
        throw std::runtime_error("ZIP entry range exceeds Qt size limits");

    QMutexLocker lock(&zipReadMutex);

    if (!entryName.endsWith(QStringLiteral(".ptpack"), Qt::CaseInsensitive))
    {
        QuaZip zip(zipPath);

        if (!zip.open(QuaZip::mdUnzip))
            throw std::runtime_error(QString("Unable to open ZIP archive: %1").arg(zipPath).toStdString());

        if (!zip.setCurrentFile(entryName))
            throw std::runtime_error(QString("Unable to locate ZIP entry: %1").arg(entryName).toStdString());

        QuaZipFile file(&zip);
        if (!file.open(QIODevice::ReadOnly))
            throw std::runtime_error(QString("Unable to open ZIP entry for reading: %1").arg(entryName).toStdString());

        std::uint64_t bytesToDiscard = offset;
        while (bytesToDiscard > 0)
        {
            const auto chunkSize = static_cast<qint64>(std::min<std::uint64_t>(bytesToDiscard, 1024 * 1024));
            const auto chunk = file.read(chunkSize);
            if (chunk.isEmpty())
                throw std::runtime_error(QString("Unable to skip to ZIP entry range: %1").arg(entryName).toStdString());
            bytesToDiscard -= static_cast<std::uint64_t>(chunk.size());
        }

        const auto data = file.read(static_cast<qint64>(size));
        if (data.size() != static_cast<qsizetype>(size))
            throw std::runtime_error(QString("Unable to read ZIP entry range: %1").arg(entryName).toStdString());
        if (file.getZipError() != UNZ_OK)
            throw std::runtime_error(QString("Failed while reading ZIP entry: %1").arg(entryName).toStdString());

        return data;
    }

    if (!stagedEntries.directory.isValid())
        throw std::runtime_error("Unable to create temporary staging directory for ZIP entry reads");

    const QFileInfo archiveInfo(zipPath);
    const auto cacheKey = QStringLiteral("%1|%2|%3|%4")
        .arg(archiveInfo.absoluteFilePath())
        .arg(archiveInfo.size())
        .arg(archiveInfo.lastModified().toMSecsSinceEpoch())
        .arg(entryName);
    auto cacheEntry = stagedEntries.entries.find(cacheKey);
    auto stagedPath = cacheEntry == stagedEntries.entries.end() ? QString() : cacheEntry->path;

    if (cacheEntry != stagedEntries.entries.end() && !QFileInfo::exists(stagedPath))
    {
        stagedEntries.cachedBytes -= cacheEntry->size;
        stagedEntries.entries.erase(cacheEntry);
        cacheEntry = stagedEntries.entries.end();
        stagedPath.clear();
    }

    if (stagedPath.isEmpty() || !QFileInfo::exists(stagedPath))
    {
        QuaZip sourceZip(zipPath);

        if (!sourceZip.open(QuaZip::mdUnzip))
            throw std::runtime_error(QString("Unable to open ZIP archive: %1").arg(zipPath).toStdString());

        if (!sourceZip.setCurrentFile(entryName))
            throw std::runtime_error(QString("Unable to locate ZIP entry: %1").arg(entryName).toStdString());

        QuaZipFileInfo64 entryInfo;
        if (!sourceZip.getCurrentFileInfo(&entryInfo))
            throw std::runtime_error(QString("Unable to inspect ZIP entry: %1").arg(entryName).toStdString());

        if (offset > entryInfo.uncompressedSize || size > entryInfo.uncompressedSize - offset)
            throw std::runtime_error(QString("Requested range exceeds ZIP entry size: %1").arg(entryName).toStdString());

        QuaZipFile sourceFile(&sourceZip);
        if (!sourceFile.open(QIODevice::ReadOnly))
            throw std::runtime_error(QString("Unable to open ZIP entry for staging: %1").arg(entryName).toStdString());

        const auto stagedFileName = QCryptographicHash::hash(cacheKey.toUtf8(), QCryptographicHash::Sha256).toHex() + ".entry";
        stagedPath = QDir(stagedEntries.directory.path()).filePath(QString::fromLatin1(stagedFileName));
        QSaveFile stagedFile(stagedPath);

        if (!stagedFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
            throw std::runtime_error(QString("Unable to create staged ZIP entry: %1").arg(stagedPath).toStdString());

        while (!sourceFile.atEnd())
        {
            const auto chunk = sourceFile.read(1024 * 1024);
            if (chunk.isEmpty() && !sourceFile.atEnd())
                throw std::runtime_error(QString("Unable to read ZIP entry while staging: %1").arg(entryName).toStdString());

            if (!chunk.isEmpty() && stagedFile.write(chunk) != chunk.size())
                throw std::runtime_error(QString("Unable to write staged ZIP entry: %1").arg(stagedPath).toStdString());
        }

        sourceFile.close();
        sourceZip.close();

        if (sourceFile.getZipError() != UNZ_OK || sourceZip.getZipError() != 0)
            throw std::runtime_error(QString("Failed while staging ZIP entry: %1").arg(entryName).toStdString());

        if (!stagedFile.commit())
            throw std::runtime_error(QString("Unable to finalize staged ZIP entry: %1").arg(stagedPath).toStdString());

        const auto stagedSize = static_cast<std::uint64_t>(QFileInfo(stagedPath).size());
        stagedEntries.entries.insert(cacheKey, { stagedPath, stagedSize, ++stagedEntries.accessSequence });
        stagedEntries.cachedBytes += stagedSize;
    }
    else
    {
        cacheEntry->lastAccess = ++stagedEntries.accessSequence;
    }

    QFile stagedFile(stagedPath);
    if (!stagedFile.open(QIODevice::ReadOnly))
        throw std::runtime_error(QString("Unable to open staged ZIP entry: %1").arg(stagedPath).toStdString());

    if (offset > static_cast<std::uint64_t>(stagedFile.size()) || size > static_cast<std::uint64_t>(stagedFile.size()) - offset)
        throw std::runtime_error(QString("Requested range exceeds ZIP entry size: %1").arg(entryName).toStdString());

    if (!stagedFile.seek(static_cast<qint64>(offset)))
        throw std::runtime_error(QString("Unable to seek in staged ZIP entry: %1").arg(stagedPath).toStdString());

    const auto data = stagedFile.read(static_cast<qint64>(size));
    if (data.size() != static_cast<qsizetype>(size))
        throw std::runtime_error(QString("Unable to read staged ZIP entry range: %1").arg(entryName).toStdString());
    stagedFile.close();

    while (stagedEntries.cachedBytes > stagedEntryCacheLimit && !stagedEntries.entries.isEmpty())
    {
        auto oldestEntry = stagedEntries.entries.end();
        for (auto entry = stagedEntries.entries.begin(); entry != stagedEntries.entries.end(); ++entry)
        {
            if (entry.key() == cacheKey && stagedEntries.entries.size() > 1)
                continue;

            if (oldestEntry == stagedEntries.entries.end() || entry->lastAccess < oldestEntry->lastAccess)
                oldestEntry = entry;
        }

        if (oldestEntry == stagedEntries.entries.end())
            break;

        QFile::remove(oldestEntry->path);
        stagedEntries.cachedBytes -= oldestEntry->size;
        stagedEntries.entries.erase(oldestEntry);
    }

    return data;
}

void Archiver::compressSubDirectory(QuaZip* parentZip, const QString& directory, const QString& parentDirectory, bool recursive /*= true*/, std::int32_t compressionLevel /*= 0*/, const QString& password /*= ""*/, QDir::Filters filters /*= QDir::Filter::Files*/, const std::function<void(std::uint64_t)>& bytesWritten)
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

        const auto method = compressionLevel == 0 ? 0 : Z_DEFLATED;

        // Attempt to open the directory zip file
        if (!dirZipFile.open(QIODevice::WriteOnly, QuaZipNewInfo(origDirectory.relativeFilePath(directory) + QLatin1String("/"), directory), password.isEmpty() ? nullptr : password.toLatin1(), 0U, method, compressionLevel, compressionLevel == -1))
            throw std::runtime_error("Unable to open directory zip file");

        // Close the directory zip file
        dirZipFile.close();
    }

    // Compress sub directories if needed
    if (recursive) {

        // Get a list of sub directories to compress
        QFileInfoList files = QDir(directory).entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | filters);

        // Compress all sub directories
        for (const auto file : files) {

            // Continue if not a directory
            if (!file.isDir())
                continue;

            // Compress sub directory
            compressSubDirectory(parentZip, file.absoluteFilePath(), parentDirectory, recursive, compressionLevel, password, filters, bytesWritten);
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
        compressFile(parentZip, file.absoluteFilePath(), filename, compressionLevel, password, bytesWritten);
    }
}

void Archiver::compressFile(QuaZip* zip, const QString& sourceFilePath, const QString& compressedFilePath, std::int32_t compressionLevel /*= 0*/, const QString& password /*= ""*/, const std::function<void(std::uint64_t)>& bytesWritten)
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
    const auto method = compressionLevel == 0 ? 0 : Z_DEFLATED;

    if (!compressedFile.open(QIODevice::WriteOnly, QuaZipNewInfo(compressedFilePath, sourceFilePath), password.isEmpty() ? nullptr : password.toLocal8Bit().data(), 0U, method, compressionLevel))
        throw std::runtime_error("Unable to open zip file");

    QFileInfo input(sourceFilePath);

    if (quazip_is_symlink(input)) {
        const auto path         = quazip_symlink_target(input);
        const auto relativePath = input.dir().relativeFilePath(path);

        const auto data = QFile::encodeName(relativePath);
        writeArchiveData(compressedFile, data, compressedFilePath);
        if (bytesWritten)
            bytesWritten(static_cast<std::uint64_t>(data.size()));
    }
    else {

        // Create input file
        QFile inFile;

        // Set its file name
        inFile.setFileName(sourceFilePath);
        
        // Except if unable to open the input file
        if (!inFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open input file");

        pipeArchiveSourceToZip(inFile, compressedFile, compressedFilePath, bytesWritten);

        if (inFile.error() != QFileDevice::NoError || compressedFile.getZipError() != UNZ_OK)
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

    // Never materialize archive-provided symbolic links. Their targets are
    // attacker-controlled and can point outside the extraction directory.
    if (info.isSymbolicLink())
        throw std::runtime_error("Symbolic links are not supported in archives");

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
