// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QVariantMap>
#include <QStringList>
#include <QVector>

namespace mv::util {

class BlobCodec;

enum class ConcurrencyMode
{
    Sequential,
    Parallel
};

/**
 * Save raw data to binary file on disk
 * @param bytes Pointer to input buffer
 * @param numberOfBytes Number of input bytes
 * @param filePath Path of the file on disk
 */
CORE_EXPORT void saveRawDataToBinaryFile(const char* bytes, const std::uint64_t& numberOfBytes, const QString& filePath);

/**
 * Load raw data from binary file on disk
 * @param bytes Pointer to input buffer
 * @param numberOfBytes Number of input bytes
 * @param filePath Path of the file on disk
 */
CORE_EXPORT void loadRawDataFromBinaryFile(char* bytes, const std::uint64_t& numberOfBytes, const QString& filePath);

/**
 * Convert raw data buffer to variant map (divide up in blocks when the total number of bytes exceeds maxBlockSize)
 * @param bytes Pointer to input buffer
 * @param numberOfBytes Number of input bytes 
 * @param saveToDisk Whether to save the raw data to disk or inline in the variant
 * @param blobCodecOverride Optional blob codec to use for encoding the data blocks (defaults to nullptr, which means no compression)
 * @param concurrencyMode Whether to encode the blocks sequentially or in parallel (defaults to sequential)
 */
CORE_EXPORT QVariantMap rawDataToVariantMap(const char* bytes, const std::uint64_t& numberOfBytes, bool saveToDisk = false, const BlobCodec* blobCodecOverride = nullptr, ConcurrencyMode concurrencyMode = ConcurrencyMode::Sequential);

/**
 * Convert variant map to raw data
 * @param variantMap Variant map containing the data blocks
 * @param bytes Output buffer to which the data is copied
 * @param concurrencyMode Whether to decode the blocks sequentially or in parallel (defaults to sequential)
 */
CORE_EXPORT void populateDataBufferFromVariantMap(const QVariantMap& variantMap, char* bytes, ConcurrencyMode concurrencyMode = ConcurrencyMode::Parallel);

/**
 * Raises an exception if an item with key is not found in a variant map
 * @param variantMap Variant map that should contain the key
 * @param key Item name
 */
CORE_EXPORT void variantMapMustContain(const QVariantMap& variantMap, const QString& key);

/**
* Store QVariant on disk if it is too large (divide up in blocks when the total number of bytes exceeds maxBlockSize) and return a QVariantMap with the file information, otherwise return the QVariant parameter.
* @param variant QVariant to store
*/
QVariant storeQVariant(const QVariant& variant);

/**
 * Load QVariant from disk if it was large, othewise return QVariant.
 * @param variant QVariant to load
 */
QVariant loadQVariant(const QVariant& variant);

/**
* Convenience function to store QStringList on disk
*/
CORE_EXPORT QVariantMap storeOnDisk(const QStringList& list);

/**
* Convenience function to store QVector of uints on disk
*/
CORE_EXPORT QVariantMap storeOnDisk(const QVector<uint32_t>& vec);

/**
* Convenience function to load QStringList from disk
*/
CORE_EXPORT void loadFromDisk(const QVariantMap& variantMap, QStringList& list);

/**
* Convenience function to store QVector of uints on disk
*/
CORE_EXPORT void loadFromDisk(const QVariantMap& variantMap, QVector<uint32_t>& vec);

CORE_EXPORT QVariantMap loadJsonToVariantMap(const QString& filePath);
}
