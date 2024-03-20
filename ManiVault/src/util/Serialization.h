// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "VariantMap.h"

#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QVariantMap>

#define DEFAULT_MAX_BLOCK_SIZE std::numeric_limits<std::int32_t>::max() / 2

namespace mv::util {

/**
 * Save raw data to binary file on disk
 * @param bytes Pointer to input buffer
 * @param numberOfBytes Number of input bytes
 * @param filePath Path of the file on disk
 */
void saveRawDataToBinaryFile(const char* bytes, const std::uint64_t& numberOfBytes, const QString& filePath);

/**
 * Load raw data from binary file on disk
 * @param bytes Pointer to input buffer
 * @param numberOfBytes Number of input bytes
 * @param filePath Path of the file on disk
 */
void loadRawDataFromBinaryFile(char* bytes, const std::uint64_t& numberOfBytes, const QString& filePath);

/**
 * Convert raw data buffer to variant map (divide up in blocks when the total number of bytes exceeds maxBlockSize)
 * @param bytes Pointer to input buffer
 * @param numberOfBytes Number of input bytes 
 * @param saveToDisk Whether to save the raw data to disk or inline in the variant
 * @param maxBlockSize Maximum size per block (defaults to DEFAULT_MAX_BLOCK_SIZE)
 */
QVariantMap rawDataToVariantMap(const char* bytes, const std::uint64_t& numberOfBytes, bool saveToDisk = false, std::uint64_t maxBlockSize = DEFAULT_MAX_BLOCK_SIZE);

/**
 * Convert variant map to raw data
 * @param variantMap Variant map containing the data blocks
 * @param bytes Output buffer to which the data is copied
 */
void populateDataBufferFromVariantMap(const QVariantMap& variantMap, char* bytes);

/**
 * Raises an exception if an item with key is not found in a variant map
 * @param variantMap Variant map that should contain the key
 * @param key Item name
 */
void variantMapMustContain(const VariantMap& variantMap, const QString& key);

}
