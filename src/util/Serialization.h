// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "BlobCodec.h"

#include "workflow/WorkflowPlan.h"
#include "workflow/WorkflowOptions.h"

#include <QByteArray>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>
#include <QVector>

#include <cstdint>
#include <cstddef>

namespace mv::util {

/** Location used to store encoded blob payloads during project serialization. */
enum class BlobStorageLocation
{
    FileInProject,         /** Store each encoded block as a file in the project archive. */
    InlineInProjectJson    /** Store each encoded block inline in the project JSON. */
};

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
 * @brief Describes one independently encoded block in a packed blob file.
 *
 * The offset points directly to the encoded payload, allowing callers to read
 * and decode one block without materializing the other blocks in the file.
 *
 * @author Thomas Kroes
 */
struct PackedBlobBlock
{
    std::uint64_t offset           = 0; /**< Byte offset of the encoded payload. */
    std::uint64_t compressedSize   = 0; /**< Number of encoded bytes. */
    std::uint64_t uncompressedSize = 0; /**< Number of decoded bytes. */
};

/**
 * @brief Appends independently encoded blobs to one packed project file.
 *
 * The file is an uncompressed container. Each appended payload is encoded
 * independently with the supplied codec, so a reader can fetch one block
 * without decoding unrelated blocks.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT PackedBlobFileWriter
{
public:
    /** Opens or creates the file path and uses the codec for each block. */
    PackedBlobFileWriter(const QString& filePath, const SharedCodec& codec);

    /** Appends one non-empty payload and returns its encoded range. */
    PackedBlobBlock append(const char* data, std::size_t size);

    /** Appends one non-empty payload and returns its encoded range. */
    PackedBlobBlock append(const QByteArray& data);

    /** Flushes and closes the file; subsequent append operations fail. */
    void close();

private:
    QFile       _file;
    SharedCodec _codec;
    bool        _closed = false;
};

/**
 * @brief Reads independently encoded ranges from a packed blob file.
 *
 * A reader owns only the file path and codec. Each read obtains the selected
 * encoded range and decodes it into a new or caller-provided buffer.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT PackedBlobFileReader
{
public:
    /** Opens the file path and validates its local packed-file header. */
    PackedBlobFileReader(const QString& filePath, const SharedCodec& codec);

    /** Reads and decodes one packed block. */
    QByteArray read(const PackedBlobBlock& block) const;

    /** Decodes one packed block into a caller-provided buffer. */
    void readTo(const PackedBlobBlock& block, char* destination, std::size_t destinationSize) const;

private:
    QString     _filePath;
    SharedCodec _codec;
};

/**
 * Serialize a raw byte buffer into a blob variant map.
 *
 * The buffer is partitioned into blocks, encoded using the active project
 * compression settings, and described by a variant map that can later be
 * restored using populateBytesFromBlobMap() or bytesFromBlobVariantMap().
 *
 * @param bytes Source byte buffer.
 * @param numberOfBytes Number of bytes in the source buffer.
 * @param storageLocation Location used to store the encoded blob payloads.
 * @return Serialized blob variant map.
 */
CORE_EXPORT QVariantMap bytesToBlobVariantMap(const char* bytes, std::uint64_t numberOfBytes, BlobStorageLocation storageLocation = BlobStorageLocation::FileInProject);

/**
 * Create a workflow that serializes a raw byte buffer into a blob variant map.
 *
 * The workflow performs block encoding in parallel and publishes the resulting
 * blob variant map as its output.
 *
 * @param bytes Source byte buffer.
 * @param numberOfBytes Number of bytes in the source buffer.
 * @param storageLocation Location used to store the encoded blob payloads.
 * @return Workflow that produces a blob variant map.
 */
CORE_EXPORT workflow::UniqueWorkflowPlan bytesToBlobVariantMapWorkflow(const char* bytes, std::uint64_t numberOfBytes, BlobStorageLocation storageLocation = BlobStorageLocation::FileInProject);

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
 * @param options Workflow options used to determine decode batching behavior.
 * @return Workflow that populates the destination buffer.
 */
CORE_EXPORT workflow::UniqueWorkflowPlan populateBytesFromBlobMapWorkflow(QVariantMap variantMap, char* destination, std::uint64_t destinationSize, const workflow::WorkflowOptions& options = {});

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
 * @param sourceLocation Source location used to report errors.
 *
 * @throws ManiVaultException If the key is not present.
 */
CORE_EXPORT void variantMapMustContain(const QVariantMap& variantMap, const QString& key, std::source_location sourceLocation = std::source_location::current());

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
