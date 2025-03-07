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

inline constexpr auto DEFAULT_MAX_BLOCK_SIZE = std::numeric_limits<std::int32_t>::max() / 2;

namespace mv::util {

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
 * @param maxBlockSize Maximum size per block (defaults to DEFAULT_MAX_BLOCK_SIZE)
 */
CORE_EXPORT QVariantMap rawDataToVariantMap(const char* bytes, const std::uint64_t& numberOfBytes, bool saveToDisk = false, std::uint64_t maxBlockSize = DEFAULT_MAX_BLOCK_SIZE);

/**
 * Convert variant map to raw data
 * @param variantMap Variant map containing the data blocks
 * @param bytes Output buffer to which the data is copied
 */
CORE_EXPORT void populateDataBufferFromVariantMap(const QVariantMap& variantMap, char* bytes);

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

}
