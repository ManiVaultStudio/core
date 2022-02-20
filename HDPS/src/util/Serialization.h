#pragma once

#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QVariantMap>

#define DEFAULT_MAX_BLOCK_SIZE std::numeric_limits<std::int32_t>::max() / 2

namespace hdps {

namespace util {

/**
 * Save raw data to binary file on disk
 * @param bytes Pointer to input buffer
 * @param numberOfBytes Number of input bytes
 * @param filePath Path of the file on disk
 */
void saveRawDataToBinaryFile(const char* bytes, const std::int64_t& numberOfBytes, const QString& filePath);

/**
 * Load raw data from binary file on disk
 * @param bytes Pointer to input buffer
 * @param numberOfBytes Number of input bytes
 * @param filePath Path of the file on disk
 */
void loadRawDataFromBinaryFile(const char* bytes, const std::int64_t& numberOfBytes, const QString& filePath);

/**
 * Convert raw data buffer to variant map
 * @param bytes Pointer to input buffer
 * @param numberOfBytes Number of input bytes 
 * @param saveToDisk Whether to save the raw data to disk
 * @param maxBlockSize Maximum size per block (DEFAULT_MAX_BLOCK_SIZE when maxBlockSize == -1)
 */
QVariantMap rawDataToVariantMap(const char* bytes, const std::int64_t& numberOfBytes, bool saveToDisk = false, std::int64_t maxBlockSize = -1);

/**
 * Convert variant map to raw data
 * @param variantMap Variant map containing the data blocks
 * @param bytes Output buffer to which the data is copied
 */
void populateDataBufferFromVariantMap(const QVariantMap& variantMap, const char* bytes);

/**
 * Raises an exception if an item with key is not found in a variant map
 * @param variantMap Variant map that should contain the key
 * @param key Item name
 */
void variantMapMustContain(const QVariantMap& variantMap, const QString& key);

}
}
