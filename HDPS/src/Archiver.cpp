#include "Archiver.h"

#include <util/Exception.h>

#include <stdexcept>

#include <QDebug>
#include <QDir>
#include <QThread>

#include <quazip/JlCompress.h>

namespace hdps {

namespace util {

void Archiver::compressDirectory(const QString& sourceDirectory, const QString& destinationFileName, bool recursive /*= true*/, std::int32_t compressionLevel /*= 0*/, const QString& password /*= ""*/, QDir::Filters filters /*= QDir::Filter::Files*/)
{
    // Clean up and throw exception
    const auto except = [&destinationFileName](const QString& errorMessage) {

        // Remove destination file
        QFile::remove(destinationFileName);

        // Except
        throw std::runtime_error(errorMessage.toLatin1());
    };

    // Compressed file
    QuaZip zip(destinationFileName);

    //// Create the destination file
    QDir().mkpath(QFileInfo(destinationFileName).absolutePath());

    // Except if not created
    if (!zip.open(QuaZip::mdCreate))
        except("Unable to create destination file");

    // Except if a zip error occurred
    if (zip.getZipError() != 0)
        except("Zip error(s) occurred");

    // Compress the sub directory
    Archiver::compressSubDirectory(&zip, sourceDirectory, sourceDirectory, recursive, compressionLevel, password, filters);

    // Notify others that directory compression started
    emit taskStarted("Save to disk");

    // Close the zip file
    zip.close();

    // Notify others that directory compression finished
    emit taskFinished("Save to disk");
}

void Archiver::decompressDirectory(const QString& compressedFile, const QString& destinationDirectory, const QString& password /*= ""*/)
{
    //JlCompress::extractDir(compressedFile, destinationDirectory);
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
        taskNames << "Packing " + fileInDirectory;

    // For scanning subdirectories in the directory
    QDir subDirectoriesInDirectory(directory);

    // Add number of files of sub directories if required
    if (recursive) {
        for (const auto subDirectory : subDirectoriesInDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
            taskNames << getTaskNamesForDirectoryCompression(subDirectory, recursive, level);
    }

    return taskNames;
}

QByteArray Archiver::getFileChecksum(const QString& fileName, const QCryptographicHash::Algorithm& hashAlgorithm)
{
    QFile file(fileName);

    if (file.open(QFile::ReadOnly)) {
        QCryptographicHash hash(hashAlgorithm);

        if (hash.addData(&file))
            return hash.result();
    }

    return QByteArray();
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
    // Establish task name
    const auto taskName = "Packing " + compressedFilePath;

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

}
}
