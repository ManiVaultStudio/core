#pragma once

#include <QString>
#include <QDir>

class QuaZip;

namespace hdps {

namespace util {

/**
 * Archiver class
 *
 * Class for archiving file and directories
 *
 * @author Thomas Kroes
 */
class Archiver
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    Archiver() = delete;
 
    /**
     * Compresses an entire directory and possibly password-protects it
     * @param sourceDirectory Path of the source directory
     * @param destinationFileName File name of the destination compressed file
     * @param recursive Whether to pack sub-directories as well or only files
     * @param compressionLevel Compression level
     * @param password Password string if files need to be secured
     * @param filters File include filter
     */
    static void compressDirectory(const QString& sourceDirectory, const QString& destinationFileName, bool recursive = true, std::int32_t compressionLevel = -1, const QString& password = "", QDir::Filters filters = QDir::Filter::Files);

    /**
     * Decompresses an entire directory
     * @param compressedFile Path of the compressed source file
     * @param destinationDirectory Path of the destination directory where files will be extracted
     * @param password Password string if files need to be secured
     * @return Boolean indicating whether decompression was successful
     */
    static void decompressDirectory(const QString& compressedFile, const QString& destinationDirectory, const QString& password = "");

protected:

    /**
     * Compresses a sub directory
     * @param parentZip Opened zip containing the parent directory
     * @param directory The full path to the directory to pack
     * @param parentDirectory The full path to the directory corresponding to the root of the ZIP
     * @param recursive Whether to pack sub-directories as well or only files
     * @param compressionLevel Compression level
     * @param password Password string if files need to be secured
     * @param filters File include filter
     */
    static void compressSubDirectory(QuaZip* parentZip, const QString& directory, const QString& parentDirectory, bool recursive = true, std::int32_t compressionLevel = -1, const QString& password = "", QDir::Filters filters = QDir::Filter::Files);

    /**
     * Compresses a file
     * @param zip Pointer to quazip instance
     * @param sourceFilePath Path of the source file
     * @param compressedFilePath Path of the compressed file
     * @param compressionLevel Compression level
     * @param password Password string if files need to be secured
     */
    static void compressFile(QuaZip* zip, const QString& sourceFilePath, const QString& compressedFilePath, std::int32_t compressionLevel = -1, const QString& password = "");
};



}
}
