// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "BlobCodec.h"

#include "workflow/WorkflowPlan.h"

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
    const char*         _data = nullptr;        /** Pointer to the start of the original raw data buffer */
    std::uint64_t       _offset = 0;            /** Offset of the block in the original data buffer */
    std::uint64_t       _size = 0;              /** Size of the block in the original data buffer */
    EncodeBlockResult   _result;                /** Result of encoding the block, populated after the block is encoded */
    SharedCodec         _codec;                 /** Codec to use for encoding the block (if applicable) */
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
    std::uint64_t       _offset = 0;            /** Offset of the block in the original data buffer */
    std::uint64_t       _size = 0;              /** Size of the block in the original data buffer */
    std::uint64_t       _compressedSize = 0;    /** Size of the compressed data block */
    QString             _uri;                   /** URI of the file containing the compressed data block (if applicable) */
    QString             _encodedData;           /** Base64 encoded string containing the compressed data block (if applicable) */
    DecodeBlockResult   _result;                /** Result of decoding the block, populated after the block is decoded */
    SharedCodec         _codec;                 /** Codec to use for encoding the block (if applicable) */
};

using DecodeBlockJobs = QVector<DecodeBlockJob>;

CORE_EXPORT QVariantMap bytesToBlobVariantMap(const char* bytes, const std::uint64_t& numberOfBytes, const workflow::SharedWorkflowExecutionContext& executionContext = nullptr);

/**
 * Decode a block of data from a file on disk and populate the provided output buffer with the decoded data
 * @param decodeBlockJob DecodeBlockJob containing the block information
 * @return DecodeBlockResult containing the decoded data or an error message
 */
CORE_EXPORT DecodeBlockResult decodeBlockFromFile(const DecodeBlockJob& decodeBlockJob);

/**
* Decode a block of data from a file on disk and populate the provided output buffer with the decoded data
* @param decodeBlockJob DecodeBlockJob containing the block information
* @param destination Output buffer to which the decoded data is copied 
* @return DecodeBlockResult containing the decoded data or an error message
*/
CORE_EXPORT DecodeBlockResult decodeBlockFromFileTo(const DecodeBlockJob& decodeBlockJob, char* destination);

/**
 * Decode a block of data from a base64 encoded string and populate the provided output buffer with the decoded data
 * @param decodeBlockJob DecodeBlockJob containing the block information
 * @param createCodec Function that creates a blob codec
 * @return DecodeBlockResult containing the decoded data or an error message
 */
CORE_EXPORT DecodeBlockResult decodeBlockFromBase64(const DecodeBlockJob& decodeBlockJob, const std::function<std::shared_ptr<BlobCodec>()>& createCodec);

CORE_EXPORT workflow::UniqueWorkflowPlan populateBytesFromBlobMapWorkflow(QVariantMap variantMap, char* destination, std::uint64_t destinationSize, const workflow::SharedWorkflowExecutionContext& executionContext = nullptr);
CORE_EXPORT void                         populateBytesFromBlobMap(QVariantMap variantMap, char* destination, std::uint64_t destinationSize, const workflow::SharedWorkflowExecutionContext& executionContext = nullptr);
CORE_EXPORT QByteArray                   bytesFromBlobVariantMap(QVariantMap variantMap, const workflow::SharedWorkflowExecutionContext& executionContext = nullptr);

/**
 * Raises an exception if an item with key is not found in a variant map
 * @param variantMap Variant map that should contain the key
 * @param key Item name
 */
CORE_EXPORT void variantMapMustContain(const QVariantMap& variantMap, const QString& key);

/**
* Convenience function to store QStringList on disk
*/
CORE_EXPORT QVariantMap storeOnDisk(const QStringList& list);

/**
* Convenience function to store QVector of uints on disk
*/
CORE_EXPORT QVariantMap storeOnDisk(const QVector<uint32_t>& vector);

/**
* Convenience function to load QStringList from disk
*/
CORE_EXPORT void loadFromDisk(const QVariantMap& variantMap, QStringList& list);

/**
* Convenience function to store QVector of uints on disk
*/
CORE_EXPORT void loadFromDisk(const QVariantMap& variantMap, QVector<uint32_t>& vec);

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
