// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "BlobCodec.h"

#include "workflow/WorkflowPlan.h"

#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>
#include <QVector>

#include <cstdint>

namespace mv::util {

/** Result produced by encoding a single data block. */
struct EncodeBlockResult
{
    std::uint64_t       _offset = 0;            /** Offset of the block in the original data buffer. */
    std::uint64_t       _size = 0;              /** Size of the block in the original data buffer. */
    QByteArray          _encodedData;           /** Encoded block payload when stored inline. */
    QString             _error;                 /** Error message if encoding failed. */
    QVariantMap         _block;                 /** Serialized block descriptor included in the final blob variant map. */
};

using EncodeBlockResults = QVector<EncodeBlockResult>;

/** Description of a block that should be encoded. */
struct EncodeBlockJob
{
    const char* _data = nullptr;        /** Pointer to the source data buffer. */
    std::uint64_t       _offset = 0;            /** Offset of the block in the source buffer. */
    std::uint64_t       _size = 0;              /** Size of the block in bytes. */
    EncodeBlockResult   _result;                /** Encoding result populated after execution. */
    SharedCodec         _codec;                 /** Codec used to encode the block. */
};

using EncodeBlockJobs = QVector<EncodeBlockJob>;

/** Result produced by decoding a single data block. */
struct DecodeBlockResult
{
    std::uint64_t       _offset = 0;            /** Offset of the block in the destination buffer. */
    std::uint64_t       _size = 0;              /** Size of the decoded block in bytes. */
    QByteArray          _decodedData;           /** Decoded block data when stored in memory. */
};

using DecodeBlockResults = QVector<DecodeBlockResult>;

/** Description of a block that should be decoded. */
struct DecodeBlockJob
{
    std::uint64_t       _offset = 0;            /** Offset of the block in the destination buffer. */
    std::uint64_t       _size = 0;              /** Size of the decoded block in bytes. */
    std::uint64_t       _compressedSize = 0;    /** Size of the encoded block payload. */
    QString             _uri;                   /** Relative path to the encoded block file, if stored on disk. */
    QString             _encodedData;           /** Inline encoded block payload, if stored in the variant map. */
    DecodeBlockResult   _result;                /** Decoding result populated after execution. */
    SharedCodec         _codec;                 /** Codec used to decode the block. */
};

using DecodeBlockJobs = QVector<DecodeBlockJob>;

/**
 * Serialize a raw byte buffer into a blob variant map.
 *
 * The buffer is partitioned into blocks, encoded using the active project
 * compression settings, and described by a variant map that can later be
 * restored using populateBytesFromBlobMap() or bytesFromBlobVariantMap().
 *
 * @param bytes Source byte buffer.
 * @param numberOfBytes Number of bytes in the source buffer.
 * @return Serialized blob variant map.
 */
CORE_EXPORT QVariantMap bytesToBlobVariantMap(const char* bytes, std::uint64_t numberOfBytes);

/**
 * Create a workflow that serializes a raw byte buffer into a blob variant map.
 *
 * The workflow performs block encoding in parallel and publishes the resulting
 * blob variant map as its output.
 *
 * @param bytes Source byte buffer.
 * @param numberOfBytes Number of bytes in the source buffer.
 * @return Workflow that produces a blob variant map.
 */
CORE_EXPORT workflow::UniqueWorkflowPlan bytesToBlobVariantMapWorkflow(const char* bytes, std::uint64_t numberOfBytes);

/**
 * Populate a destination buffer from a serialized blob variant map.
 *
 * All encoded blocks described by the variant map are decoded and written into
 * the destination buffer at their recorded offsets.
 *
 * @param variantMap Serialized blob variant map.
 * @param destination Destination buffer.
 * @param destinationSize Size of the destination buffer in bytes.
 */
CORE_EXPORT void populateBytesFromBlobMap(QVariantMap variantMap, char* destination, std::uint64_t destinationSize);

/**
 * Create a workflow that populates a destination buffer from a serialized blob
 * variant map.
 *
 * The workflow decodes blocks in parallel according to the supplied workflow
 * execution options.
 *
 * @param variantMap Serialized blob variant map.
 * @param destination Destination buffer.
 * @param destinationSize Size of the destination buffer in bytes.
 * @param executionOptions Workflow execution options used to determine decode
 *        batching behavior.
 * @return Workflow that populates the destination buffer.
 */
CORE_EXPORT workflow::UniqueWorkflowPlan populateBytesFromBlobMapWorkflow(QVariantMap variantMap, char* destination, std::uint64_t destinationSize, const workflow::WorkflowExecutionOptions& executionOptions = {});

/**
 * Deserialize a blob variant map into a byte array.
 *
 * The required storage is allocated automatically and all encoded blocks are
 * decoded into the returned byte array.
 *
 * @param variantMap Serialized blob variant map.
 * @return Decoded byte array.
 */
CORE_EXPORT QByteArray bytesFromBlobVariantMap(QVariantMap variantMap);

/**
 * Ensure that a variant map contains a required key.
 *
 * @param variantMap Variant map to inspect.
 * @param key Required key.
 *
 * @throws ManiVaultException If the key is not present.
 */
CORE_EXPORT void variantMapMustContain(const QVariantMap& variantMap, const QString& key);

/**
 * Serialize a string list using the blob serialization format.
 *
 * @param list String list to serialize.
 * @return Serialized representation.
 */
CORE_EXPORT QVariantMap storeOnDisk(const QStringList& list);

/**
 * Serialize a vector using the blob serialization format.
 *
 * @param vector Vector to serialize.
 * @return Serialized representation.
 */
CORE_EXPORT QVariantMap storeOnDisk(const QVector<uint32_t>& vector);

/**
 * Restore a string list from a serialized blob representation.
 *
 * @param variantMap Serialized representation.
 * @param list Destination string list.
 */
CORE_EXPORT void loadFromDisk(const QVariantMap& variantMap, QStringList& list);

/**
 * Restore a vector from a serialized blob representation.
 *
 * @param variantMap Serialized representation.
 * @param vec Destination vector.
 */
CORE_EXPORT void loadFromDisk(const QVariantMap& variantMap, QVector<uint32_t>& vec);

/**
 * Estimate the total decoded size of all serialized blob objects contained
 * within a variant hierarchy.
 *
 * @param value Variant value to inspect recursively.
 * @return Estimated total decoded size in bytes.
 */
CORE_EXPORT std::uint64_t estimateRawBlockTotalSize(const QVariant& value);

/**
 * Serialize a variant map to a binary byte array.
 *
 * @param map Variant map to serialize.
 * @return Serialized byte array.
 */
CORE_EXPORT QByteArray serializeVariantMap(const QVariantMap& map);

/**
 * Deserialize a variant map from a binary byte array.
 *
 * @param bytes Serialized byte array.
 * @return Deserialized variant map.
 */
CORE_EXPORT QVariantMap deserializeVariantMap(const QByteArray& bytes);

}
