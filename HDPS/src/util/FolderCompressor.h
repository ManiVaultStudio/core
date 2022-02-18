#pragma once

#include <QString>

#include "hdps_public_export.h"

namespace hdps {

namespace util {

/**
 * Folder compression class
 *
 * Class for compressing/decompressing folders
 *
 * @author Thomas Kroes
 */
class FolderCompressor
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    FolderCompressor() = default;
 
    
};

/**
 * Compresses an entire directory and possibly password-protects it
 * @param destinationFile Path of the compressed file
 * @param sourceDirectory Path of the source directory
 * @param password Password (in case of password-protection)
 * @return Boolean indicating whether compression was successful
 */
bool HDPS_PUBLIC_EXPORT compressFolder(const QString& destinationFile, const QString& sourceDirectory, const QString& password = "");

/**
 * Decompresses an entire directory
 * @param compressedFile Path of the compressed source file
 * @param destinationDirectory Path of the destination directory where files will be extracted
 * @param password Password (in case of password-protection)
 * @return Boolean indicating whether decompression was successful
 */
bool HDPS_PUBLIC_EXPORT decompressFolder(const QString& compressedFile, const QString& destinationDirectory, const QString& password = "");

}
}
