// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowPlan.h"

#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QVariantMap>
#include <QStringList>
#include <QVector>

namespace mv::util {

class BlobCodec;

struct EncodeBlockResult
{
    std::uint64_t       _offset = 0;            /** Offset of the block in the original data buffer */
    std::uint64_t       _size = 0;              /** Size of the block in the original data buffer */
    QByteArray          _encodedData;           /** Base64 encoded string containing the encoded data block (empty if encoding failed) */
    QString             _error;                 /** Error message in case encoding the block failed (empty if encoding was successful) */
    QVariantMap         _block;                 /** Variant map containing the block information to be included in the final variant map (e.g. file URI or base64 encoded data) */
};

using EncodeBlockResults = QVector<EncodeBlockResult>;

struct EncodeBlockJob
{
    std::uint64_t       _offset = 0;            /** Offset of the block in the original data buffer */
    std::uint64_t       _size = 0;              /** Size of the block in the original data buffer */
    QByteArray          _rawData;               /** Raw data block to encode */
    EncodeBlockResult   _result;                 /** Result of encoding the block, populated after the block is encoded */
};

using EncodeBlockJobs = QVector<EncodeBlockJob>;

/** Result of decoding a block of data */
struct DecodeBlockResult
{
    std::uint64_t       _offset = 0;            /** Offset of the block in the original data buffer */
    std::uint64_t       _size = 0;              /** Size of the block in the original data buffer */
    QByteArray          _decodedData;           /** Decoded data block */
};

using DecodeBlockResults = QVector<DecodeBlockResult>;

/** Information about a block of data to decode */
struct DecodeBlockJob
{
    quint64             _offset = 0;            /** Offset of the block in the original data buffer */
    quint64             _size = 0;              /** Size of the block in the original data buffer */
    quint64             _compressedSize = 0;    /** Size of the compressed data block */
    QString             _uri;                   /** URI of the file containing the compressed data block (if applicable) */
    QString             _encodedData;           /** Base64 encoded string containing the compressed data block (if applicable) */
    DecodeBlockResult   _result;                /** Result of decoding the block, populated after the block is decoded */
};

using DecodeBlockJobs = QVector<DecodeBlockJob>;

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
 * @param blobCodecOverride Optional blob codec to use for encoding the data blocks (defaults to nullptr, which means no compression)
 * @param concurrencyMode Whether to encode the blocks sequentially or in parallel (defaults to sequential)
 */
CORE_EXPORT QVariantMap rawDataToVariantMap(const char* bytes, const std::uint64_t& numberOfBytes, const BlobCodec* blobCodecOverride = nullptr, WorkflowPlan::ConcurrencyMode concurrencyMode = WorkflowPlan::ConcurrencyMode::Parallel);

/**
 * Decode a block of data from a file on disk and populate the provided output buffer with the decoded data
 * @param decodeBlockJob DecodeBlockJob containing the block information
 * @param createCodec Function that creates a blob codec
 * @return DecodeBlockResult containing the decoded data or an error message
 */
CORE_EXPORT DecodeBlockResult decodeBlockFromFile(const DecodeBlockJob& decodeBlockJob, const std::function<std::shared_ptr<BlobCodec>()>& createCodec);

/**
 * Decode a block of data from a base64 encoded string and populate the provided output buffer with the decoded data
 * @param decodeBlockJob DecodeBlockJob containing the block information
 * @param createCodec Function that creates a blob codec
 * @return DecodeBlockResult containing the decoded data or an error message
 */
CORE_EXPORT DecodeBlockResult decodeBlockFromBase64(const DecodeBlockJob& decodeBlockJob, const std::function<std::shared_ptr<BlobCodec>()>& createCodec);

/**
 * Convert variant map to raw data buffer (blocks are loaded from disk and decoded if necessary)
 * @param variantMap Variant map containing the raw data or file information
 * @param bytes Output buffer to which the raw data is copied (resizes to fit the decoded data)
 * @param concurrencyMode Whether to decode the blocks sequentially or in parallel (defaults to sequential)
 */
CORE_EXPORT void decodeDataBufferFromVariantMap(const QVariantMap& variantMap, QByteArray& bytes, WorkflowPlan::ConcurrencyMode concurrencyMode = WorkflowPlan::ConcurrencyMode::Parallel);

/**
 * Convert variant map to raw data buffer (blocks are loaded from disk and decoded if necessary)
 * @param variantMap Variant map containing the raw data or file information
 * @param bytes Output buffer to which the raw data is copied
 * @param concurrencyMode Whether to decode the blocks sequentially or in parallel (defaults to sequential)
 * @warning This function does not perform any bounds checking on the output buffer, so it is the caller's responsibility to ensure that the buffer is
 * large enough to hold the decoded data. It is recommended to use \p decodeDataBufferFromVariantMap(...) that takes a QByteArray as output buffer, which automatically resizes
 * to fit the decoded data.
 */
CORE_EXPORT void populateDataBufferFromVariantMap(const QVariantMap& variantMap, char* bytes, WorkflowPlan::ConcurrencyMode concurrencyMode = WorkflowPlan::ConcurrencyMode::Parallel);

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
