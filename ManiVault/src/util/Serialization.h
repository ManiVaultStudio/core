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
 */
CORE_EXPORT QVariantMap rawDataToVariantMap(const char* bytes, const std::uint64_t& numberOfBytes, const BlobCodec* blobCodecOverride = nullptr);

/**
 * Decode a block of data from a file on disk and populate the provided output buffer with the decoded data
 * @param decodeBlockJob DecodeBlockJob containing the block information
 * @param createCodec Function that creates a blob codec
 * @return DecodeBlockResult containing the decoded data or an error message
 */
CORE_EXPORT DecodeBlockResult decodeBlockFromFile(const DecodeBlockJob& decodeBlockJob, const std::function<std::shared_ptr<BlobCodec>()>& createCodec);

/**
* Decode a block of data from a file on disk and populate the provided output buffer with the decoded data
* @param decodeBlockJob DecodeBlockJob containing the block information
* @param createCodec Function that creates a blob codec
* @param destination Output buffer to which the decoded data is copied 
* @return DecodeBlockResult containing the decoded data or an error message
*/
CORE_EXPORT DecodeBlockResult decodeBlockFromFileTo(const DecodeBlockJob& decodeBlockJob, const std::function<std::shared_ptr<BlobCodec>()>& createCodec, char* destination);

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
 * @param bytes Output buffer to which the raw data is copied
 * @warning This function does not perform any bounds checking on the output buffer, so it is the caller's responsibility to ensure that the buffer is
 * large enough to hold the decoded data. It is recommended to use \p decodeDataBufferFromVariantMap(...) that takes a QByteArray as output buffer, which automatically resizes
 * to fit the decoded data.
 */
CORE_EXPORT void populateDataBufferFromVariantMap(const QVariantMap& variantMap, char* bytes);

/**
* Convert variant map to raw data buffer (blocks are loaded from disk and decoded if necessary)
* @param variantMap Variant map containing the raw data or file information
* @param bytes Output buffer to which the raw data is copied
* @warning This function does not perform any bounds checking on the output buffer, so it is the caller's responsibility to ensure that the buffer is
* large enough to hold the decoded data. It is recommended to use \p decodeDataBufferFromVariantMap(...) that takes a QByteArray as output buffer, which automatically resizes
* to fit the decoded data.
*/
CORE_EXPORT void populateDataBufferFromVariantMap(const QVariantMap& variantMap, QByteArray& bytes);

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

/**
 * Check whether a QVariantMap contains the necessary keys to be considered a raw block object (i.e. a block of raw data that is stored on disk or as base64 encoded string in the variant map)
 * @param map QVariantMap to check
 * @return true if the map contains the necessary keys, false otherwise
 */
CORE_EXPORT bool isVariantMapRawBlockObject(const QVariantMap& map);

/**
 * Recursively search for a raw block object in a QVariant (which can be a QVariantMap, QVariantList, or any other type) and return the first found raw block object as a QVariantMap. If no raw block object is found, an empty QVariantMap is returned.
 * @param value QVariant to search
 * @return QVariantMap containing the first found raw block object, or an empty QVariantMap if no raw block object is found
*/
CORE_EXPORT QVariantMap findRawBlockObject(const QVariant& value);

/**
 * Get the size of the raw block object contained in a QVariantMap (if the map is a raw block object), or return 0 if the map is not a raw block object or if the size information is not available. This function is useful to estimate the size of a raw block when only the variant map containing the block information is available (e.g. when estimating the total size of all raw blocks contained in a larger variant map).
 * @param map QVariantMap containing the raw block object information
 * @return Size of the raw block object in bytes, or 0 if the map is not a raw block object or if the size information is not available
 */
CORE_EXPORT std::uint64_t getRawBlockObjectSize(const QVariantMap& map);

/**
 * Recursively estimate the total size of all raw blocks contained in a QVariant (which can be a QVariantMap, QVariantList, or any other type). This function is useful to estimate the total size of the data that needs to be loaded from disk when decoding a variant map containing raw block objects.
 * @param value QVariant to search
 * @return Total size of all raw blocks contained in the input QVariant
 */
CORE_EXPORT std::uint64_t estimateRawBlockTotalSize(const QVariant& value);

/**
 * Serialize a QVariantMap to a QByteArray using QDataStream
 * @param map QVariantMap to serialize
 * @return QByteArray containing the serialized QVariantMap
 */
CORE_EXPORT QByteArray serializeVariantMap(const QVariantMap& map);

/**
 * Deserialize a QByteArray to a QVariantMap using QDataStream
 * @param bytes QByteArray containing the serialized QVariantMap
 * @return QVariantMap deserialized from the input QByteArray
 */
CORE_EXPORT QVariantMap deserializeVariantMap(const QByteArray& bytes);

}
