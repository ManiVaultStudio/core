// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Serialization.h"
#include "CoreInterface.h"
#include "CodecRegistry.h"

#include "workflow/WorkflowRuntimeScoped.h"

#include "exception/ManiVaultException.h"

#include <QUuid>
#include <QFileInfo>
#include <QFile>
#include <QDir>

#include <exception>

#include <math.h>
#include <limits>

using namespace mv::workflow;

namespace mv::util {

namespace
{

/**
 * Encode a single raw data block and store the encoded payload on disk.
 *
 * The function encodes the byte range described by `job._offset` and
 * `job._size` from the source buffer `job._data`. The encoded payload is
 * written to a uniquely named file inside `saveDir`, using the file extension
 * provided by the block codec.
 *
 * The returned result contains a block variant map with the metadata required
 * to decode the block later, including offset, decoded size, compressed size,
 * and file URI.
 *
 * @param job Encode job describing the source buffer, byte range, and codec.
 * @param saveDir Directory where the encoded block file should be written.
 * @return Encode result containing the serialized block descriptor.
 *
 * @throws ManiVaultException If the block cannot be encoded.
 */
EncodeBlockResult encodeBlock(const EncodeBlockJob& job, const QString& saveDir)
{
    EncodeBlockResult result;

    try {
        if (job._data == nullptr)
            throw std::invalid_argument("EncodeBlockJob data pointer is null");

        if (job._size > static_cast<std::uint64_t>(std::numeric_limits<qsizetype>::max()))
            throw std::overflow_error("EncodeBlockJob block size exceeds qsizetype range");

        QVariantMap blockVariantMap;

        blockVariantMap["Offset"]   = QVariant::fromValue(job._offset);
        blockVariantMap["Size"]     = QVariant::fromValue(job._size);

        const auto fileName = QUuid::createUuid().toString(QUuid::WithoutBraces) + job._codec->getFileExtension();
        const auto filePath = QDir::cleanPath(saveDir + QDir::separator() + fileName);

        std::uint64_t numberOfEncodedBytes = 0;

        job._codec->encodeToFile(job._data + job._offset, static_cast<qsizetype>(job._size), filePath, &numberOfEncodedBytes);

        blockVariantMap["CompressedSize"]   = QVariant::fromValue<std::uint64_t>(numberOfEncodedBytes);
        blockVariantMap["URI"]              = fileName;

        result._block = std::move(blockVariantMap);
    }
    catch (const ManiVaultException& maniVaultException) {
        throw maniVaultException.withAddedDetails({
            { "Offset", QString::number(job._offset) },
            { "Size", QString::number(job._size) }
            });
    }
    catch (const std::exception& exception) {
        throw ManiVaultException(SeverityLevel::Error, "Unable to encode block", exception.what(), __FUNCTION__,
            {
                { "Offset", QString::number(job._offset) },
                { "Size", QString::number(job._size) },
                { "SaveDir", saveDir }
            });
    }

    return result;
}

/**
 * Create encode jobs that split a contiguous byte buffer into fixed-size blocks.
 *
 * Each returned job references the original input buffer, stores the byte offset
 * and block size for one block, and owns a fresh codec instance created with
 * createCodec(). The final block may be smaller than blockSizeInBytes.
 *
 * @param bytes Pointer to the source byte buffer. May be null only when numberOfBytes is zero.
 * @param numberOfBytes Total number of bytes available in the source buffer.
 * @param createCodec Factory function used to create one codec instance per encode job.
 * @param blockSizeInBytes Maximum size of each encoded block.
 * @return Encode jobs covering the full input buffer.
 *
 * @throws std::invalid_argument If bytes is null while numberOfBytes is non-zero.
 * @throws std::invalid_argument If blockSizeInBytes is zero.
 */
EncodeBlockJobs makeEncodeBlockJobs(const char* bytes, std::uint64_t numberOfBytes, const std::function<SharedCodec()>& createCodec, std::uint64_t blockSizeInBytes)
{
    if (!bytes && numberOfBytes > 0)
        throw std::invalid_argument("bytes is null");

    if (blockSizeInBytes == 0)
        throw std::invalid_argument("blockSizeInBytes is zero");

    EncodeBlockJobs jobs;

    const auto numberOfBlocks = (numberOfBytes + blockSizeInBytes - 1) / blockSizeInBytes;

    jobs.reserve(static_cast<int>(numberOfBlocks));

    for (std::uint64_t offset = 0; offset < numberOfBytes;) {
        const auto blockSize = std::min(blockSizeInBytes, numberOfBytes - offset);

        EncodeBlockJob job;

        job._data       = bytes;
        job._offset     = offset;
        job._size       = blockSize;
        job._codec      = createCodec();

        jobs.push_back(std::move(job));

        offset += blockSize;
    }

    return jobs;
}

/**
 * Create a blob metadata variant map from completed block encoding jobs.
 *
 * The returned variant map contains the original uncompressed size, codec name,
 * block size, number of blocks, and the serialized metadata for each encoded
 * block. All encode jobs must have completed successfully before calling this
 * function.
 *
 * @param numberOfBytes Original size of the uncompressed data.
 * @param blockSizeInBytes Block size used during encoding.
 * @param codecName Name of the codec used to encode the blocks.
 * @param jobs Completed encode jobs containing encoded block results.
 * @return Variant map describing the encoded blob and its blocks.
 *
 * @throws ManiVaultException If any block encoding operation failed.
 */
QVariantMap makeBlobVariantMap(std::uint64_t numberOfBytes, std::uint64_t blockSizeInBytes, const QString& codecName, const EncodeBlockJobs& jobs)
{
    QVariantMap rawData;

    rawData["Size"]     = QVariant::fromValue(numberOfBytes);
    rawData["Codec"]    = codecName;

    QVariantList blocks;
    blocks.reserve(jobs.size());

    for (qsizetype i = 0; i < jobs.size(); ++i) {
        const auto& job = jobs[i];

        if (!job._result._error.isEmpty()) {
            throw ManiVaultException(SeverityLevel::Error, "Failed to encode block", job._result._error, __FUNCTION__, { { "BlockIndex", QString::number(i) } });
        }

        blocks.push_back(job._result._block);
    }

    rawData["BlockSize"]    = QVariant::fromValue(blockSizeInBytes);
    rawData["Blocks"]       = blocks;

    return rawData;
}

/**
 * Create decode jobs from serialized blob metadata.
 *
 * This function validates the blob metadata, creates one decode job per
 * serialized block, instantiates the required codec for each job, and verifies
 * that the blocks form a complete, contiguous, non-overlapping coverage of the
 * original data buffer.
 *
 * Each returned job contains the information required to decode a single block,
 * including its offset, size, compressed size, storage location, encoded data,
 * and codec instance.
 *
 * @param variantMap Serialized blob metadata containing block information.
 * @return Decode jobs describing all blocks in the encoded blob.
 *
 * @throws ManiVaultException If required metadata is missing.
 * @throws ManiVaultException If the decoded size is zero.
 * @throws ManiVaultException If the referenced codec is not registered.
 * @throws ManiVaultException If blocks overlap, contain gaps, or do not fully
 *         cover the expected data size.
 */
DecodeBlockJobs makeDecodeBlockJobs(const QVariantMap& variantMap)
{
    variantMapMustContain(variantMap, "BlockSize");
    variantMapMustContain(variantMap, "Blocks");
    variantMapMustContain(variantMap, "Size");

    bool totalSizeOk = false;

    const auto blocks       = variantMap.value("Blocks").toList();
    const bool hasCodec     = variantMap.contains("Codec");
    const auto codecName    = hasCodec ? variantMap.value("Codec").toString() : QStringLiteral("none");
    const auto totalSize    = variantMap.value("Size").toULongLong(&totalSizeOk);

    if (blocks.isEmpty()) {
        return {};  // No blocks to decode, return empty job list
    }

    if (!totalSizeOk) {
        throw ManiVaultException(SeverityLevel::Error, "Failed to populate data buffer from variant map", "Decoded buffer size is invalid", __FUNCTION__, {
            { "VariantMap", variantMap }
        });
    }

    if (totalSize == 0)
        throw ManiVaultException(SeverityLevel::Error, "Failed to populate data buffer from variant map", "Decoded buffer size is zero", __FUNCTION__);

    if (hasCodec && !codecRegistry().isRegistered(codecName))
        throw ManiVaultException(SeverityLevel::Error, "Failed to populate data buffer from variant map", QString("Unable to load raw data, codec %1 is not registered").arg(codecName), __FUNCTION__);

    auto createCodec = [hasCodec, codecName]() -> std::shared_ptr<BlobCodec> {
        return hasCodec ? codecRegistry().createCodec(nullptr, codecName) : codecRegistry().createCodec(nullptr, BlobCodec::Type::None);
    };

    DecodeBlockJobs jobs;

	jobs.reserve(blocks.size());

    for (qsizetype i = 0; i < blocks.size(); ++i) {
        const auto blockMap = blocks[i].toMap();

        variantMapMustContain(blockMap, "Offset");
        variantMapMustContain(blockMap, "Size");

        bool offsetOk           = false;
        bool sizeOk             = false;
        bool compressedSizeOk   = true;

        const auto offset           = blockMap.value("Offset").toULongLong(&offsetOk);
        const auto size             = blockMap.value("Size").toULongLong(&sizeOk);
        const auto compressedSize   = blockMap.contains("CompressedSize") ? blockMap.value("CompressedSize").toULongLong(&compressedSizeOk) : 0;

        if (!offsetOk || !sizeOk || !compressedSizeOk) {
            throw ManiVaultException(
                SeverityLevel::Error,
                "Failed to populate data buffer from variant map",
                QString("Invalid raw-data block metadata at index %1").arg(i),
                __FUNCTION__,
                {
                    { "BlockIndex", QString::number(i) },
                    { "BlockMap", blockMap },
                    { "VariantMap", variantMap }
                });
        }

        DecodeBlockJob job;

    	job._offset             = offset;
        job._size               = size;
        job._compressedSize     = compressedSize;
        job._codec              = createCodec();
        job._uri                = blockMap.value("URI").toString();
        job._encodedData        = blockMap.value("Data").toString();

        jobs.push_back(std::move(job));
    }

    // Overlap check
    std::sort(jobs.begin(), jobs.end(), [](const DecodeBlockJob& a, const DecodeBlockJob& b) {
        return a._offset < b._offset;
    });

    quint64 expectedOffset = 0;

    for (qsizetype i = 0; i < jobs.size(); ++i) {
        const auto& job = jobs[i];

        if (job._offset > totalSize || job._size > totalSize - job._offset) {
            throw ManiVaultException(SeverityLevel::Error, "Failed to populate data buffer from variant map", QString("Raw-data block range exceeds total size at index %1").arg(i), __FUNCTION__, {
                { "CodecName", codecName },
                { "VariantMap", variantMap }
            });
        }

        const auto end = job._offset + job._size;

        if (job._offset < expectedOffset) {
            throw ManiVaultException(SeverityLevel::Error, "Failed to populate data buffer from variant map", QString("Raw-data block overlap detected at offset %1").arg(job._offset), __FUNCTION__, {
                { "CodecName", codecName },
                { "VariantMap", variantMap }
            });
        }

        if (job._offset != expectedOffset) {
            throw ManiVaultException(SeverityLevel::Error, "Failed to populate data buffer from variant map", QString("Raw-data block gap detected: expected offset %1, got %2").arg(expectedOffset).arg(job._offset), __FUNCTION__, {
                { "CodecName", codecName },
                { "VariantMap", variantMap }
            });
        }

        expectedOffset = end;
    }

    if (expectedOffset != totalSize) {
        throw ManiVaultException(SeverityLevel::Error, "Failed to populate data buffer from variant map", QString("Raw-data blocks do not cover total size. Expected %1 bytes, got %2 bytes").arg(totalSize).arg(expectedOffset), __FUNCTION__, {
            { "CodecName", codecName },
            { "VariantMap", variantMap },
        });
    }

    return jobs;
}

/**
 * Decode a serialized block directly into a destination buffer.
 *
 * The block is decoded from the file referenced by the decode job and written
 * into the destination buffer at the offset specified by the job. The function
 * validates the destination buffer, destination range, codec instance, and
 * block metadata before decoding.
 *
 * Any standard exceptions thrown during decoding are upgraded to a
 * ManiVaultException with additional diagnostic context.
 *
 * @param decodeBlockJob Decode job describing the block to decode.
 * @param destination Destination buffer that receives the decoded block data.
 * @param destinationSize Size of the destination buffer in bytes.
 * @return Result describing the decoded block.
 *
 * @throws ManiVaultException If the destination buffer is invalid, the decode
 *         range is out of bounds, the codec cannot be created, the block URI
 *         is invalid, or decoding fails.
 */
DecodeBlockResult decodeBlockFromFileTo(const DecodeBlockJob& decodeBlockJob, char* destination, std::uint64_t destinationSize)
{
    DecodeBlockResult result;

    try {
        result._offset  = decodeBlockJob._offset;
        result._size    = decodeBlockJob._size;

        if (!destination)
            throw ManiVaultException(SeverityLevel::Error, "Unable to decode block from file to buffer", "Destination buffer is null", __FUNCTION__, {
                { "URI", decodeBlockJob._uri }
            });

        if (decodeBlockJob._offset > static_cast<std::uint64_t>(std::numeric_limits<qsizetype>::max()) || decodeBlockJob._size > static_cast<std::uint64_t>(std::numeric_limits<qsizetype>::max())) {
            throw ManiVaultException(SeverityLevel::Error, "Unable to decode block from file to buffer", "Decode block offset or size exceeds std::uint64_t range", __FUNCTION__, {
                { "URI", decodeBlockJob._uri }
            });
        }

        const auto offset   = static_cast<std::uint64_t>(decodeBlockJob._offset);
        const auto size     = static_cast<std::uint64_t>(decodeBlockJob._size);

        if (offset > destinationSize || size > destinationSize - offset) {
            throw ManiVaultException(SeverityLevel::Error, "Unable to decode block from file to buffer", QString("Decode block destination range is out of bounds. offset=%1, size=%2, destinationSize=%3, uri=%4")
                .arg(offset)
                .arg(size)
                .arg(destinationSize)
                .arg(decodeBlockJob._uri),
                __FUNCTION__,
                {
                    { "Offset", QString::number(offset) },
                    { "Size", QString::number(size) },
                    { "DestinationSize", QString::number(destinationSize) },
                    { "URI", decodeBlockJob._uri }
                }
            );
        }

        auto codec = decodeBlockJob._codec;

        if (!codec)
            throw ManiVaultException(SeverityLevel::Error, "Unable to decode block from file to buffer", "Failed to create blob codec", __FUNCTION__, {
                { "URI", decodeBlockJob._uri }
            });

        if (decodeBlockJob._uri.isEmpty())
            throw ManiVaultException(SeverityLevel::Error, "Unable to decode block from file to buffer", "Block URI is empty", __FUNCTION__, {
                { "URI", decodeBlockJob._uri }
            });

        //ActiveDecodeGuard activeDecodeGuard({ decodeBlockJob._uri, destination, offset, size });

        codec->decodeFromFileTo(decodeBlockJob._uri, destination + offset, size);
    }
    catch (const ManiVaultException&) {

        // Rethrow ManiVaultExceptions as they are already properly constructed
        throw;
    }
    catch (const std::exception& exception) {

        // Upgrade to ManiVaultException with context
        throw ManiVaultException(SeverityLevel::Error, "Unable to decode block from file to buffer", exception.what(), __FUNCTION__, {
            { "Offset", QString::number(decodeBlockJob._offset) },
            { "Size", QString::number(decodeBlockJob._size) },
            { "DestinationSize", QString::number(destinationSize) },
            { "URI", decodeBlockJob._uri }
        });
    }
    catch (...) {

        // Handle any other types of exceptions
        throw ManiVaultException(SeverityLevel::Error, "Unable to decode block from file to buffer", "Unknown error", __FUNCTION__, {
            { "Offset", QString::number(decodeBlockJob._offset) },
            { "Size", QString::number(decodeBlockJob._size) },
            { "DestinationSize", QString::number(destinationSize) },
            { "URI", decodeBlockJob._uri }
        });
    }

    return result;
}

/**
 * Decode a base64-encoded block directly into a destination buffer.
 *
 * The encoded block data is decoded from the base64 string contained in the
 * decode job and subsequently decompressed or decoded using the associated
 * codec. The resulting bytes are written into the destination buffer at the
 * offset specified by the job.
 *
 * The function validates the destination buffer, destination range, and codec
 * instance before performing the decode operation. Any standard exceptions
 * thrown during decoding are upgraded to ManiVaultException instances with
 * additional diagnostic context.
 *
 * @param decodeBlockJob Decode job describing the block to decode.
 * @param destination Destination buffer that receives the decoded block data.
 * @param destinationSize Size of the destination buffer in bytes.
 * @return Result describing the decoded block.
 *
 * @throws ManiVaultException If the destination buffer is invalid, the decode
 *         range is out of bounds, the codec cannot be created, the base64 data
 *         is invalid, or decoding fails.
 */
DecodeBlockResult decodeBlockFromBase64To(const DecodeBlockJob& decodeBlockJob, char* destination, std::uint64_t destinationSize)
{
    DecodeBlockResult result;

    try {
        result._offset  = decodeBlockJob._offset;
        result._size    = decodeBlockJob._size;

        if (!destination) {
            throw ManiVaultException(SeverityLevel::Error, "Unable to decode block from base64 to buffer", "Destination buffer is null", __FUNCTION__, {
                { "URI", decodeBlockJob._uri }
            });
        }

        const auto offset   = static_cast<std::uint64_t>(decodeBlockJob._offset);
        const auto size     = static_cast<std::uint64_t>(decodeBlockJob._size);

        if (offset > destinationSize || size > destinationSize - offset) {
            throw ManiVaultException(SeverityLevel::Error,
                "Unable to decode block from base64 to buffer",
                QString("Decode block destination range is out of bounds. offset=%1, size=%2, destinationSize=%3")
                .arg(offset)
                .arg(size)
                .arg(destinationSize),
                __FUNCTION__,
                {
                    { "Offset", QString::number(decodeBlockJob._offset) },
                    { "Size", QString::number(decodeBlockJob._size) },
                    { "DestinationSize", QString::number(destinationSize) },
                    { "URI", decodeBlockJob._uri }
                });
        }

        auto codec = decodeBlockJob._codec;

        if (!codec) {
            throw ManiVaultException(SeverityLevel::Error, "Unable to decode block from base64 to buffer", "Failed to create blob codec", __FUNCTION__, {
                { "URI", decodeBlockJob._uri }
            });
        }

        const auto encodedBytes = QByteArray::fromBase64(decodeBlockJob._encodedData.toUtf8());

        codec->decodeTo(encodedBytes, destination + offset, size);
    }
    catch (const ManiVaultException&) {

        // Rethrow ManiVaultExceptions as they are already properly constructed with severity and message
        throw;
    }
    catch (const std::exception& exception) {

        // Upgrade to ManiVaultException with context
        throw ManiVaultException(SeverityLevel::Error,
            "Unable to decode block from base64 to buffer",
            exception.what(),
            __FUNCTION__,
            {
                { "Offset", QString::number(decodeBlockJob._offset) },
                { "Size", QString::number(decodeBlockJob._size) },
                { "DestinationSize", QString::number(destinationSize) },
                { "URI", decodeBlockJob._uri }
            });
    }
    catch (...) {

        // Handle any other types of exceptions
        throw ManiVaultException(
            SeverityLevel::Error,
            "Unable to decode block from base64 to buffer",
            "Unknown error",
            __FUNCTION__,
            {
                { "Offset", QString::number(decodeBlockJob._offset) },
                { "Size", QString::number(decodeBlockJob._size) },
                { "DestinationSize", QString::number(destinationSize) },
                { "URI", decodeBlockJob._uri }
            });
    }

    return result;
}

/**
 * Determine whether a variant map represents a serialized raw-data block object.
 *
 * The check is based on the presence of the required metadata fields used by the
 * block-based serialization format. No validation of field types or values is
 * performed.
 *
 * @param map Variant map to inspect.
 * @return True if the map contains all required raw-data block metadata fields;
 *         otherwise false.
 */
bool isVariantMapRawBlockObject(const QVariantMap& map)
{
    return map.contains("BlockSize") && map.contains("Blocks") && map.contains("Size");
}

/**
 * Find the first serialized raw-data block object contained within a variant.
 *
 * The search is performed recursively through nested QVariantMaps and
 * QVariantLists. Traversal stops as soon as a variant map satisfying
 * isVariantMapRawBlockObject() is found.
 *
 * @param value Variant to search.
 * @return The first raw-data block object found, or an empty QVariantMap if no
 *         matching object exists within the variant hierarchy.
 */
QVariantMap findRawBlockObject(const QVariant& value)
{
    if (value.canConvert<QVariantMap>()) {
        auto map = value.toMap();

        if (isVariantMapRawBlockObject(map))
            return map;

        for (auto it = map.begin(); it != map.end(); ++it) {
            auto found = findRawBlockObject(it.value());

            if (!found.isEmpty())
                return found;
        }
    }

    if (value.canConvert<QVariantList>()) {
        for (const QVariant& item : value.toList()) {
            auto found = findRawBlockObject(item);

            if (!found.isEmpty())
                return found;
        }
    }

    return {};
}

/**
 * Get the uncompressed size stored in a serialized raw-data block object.
 *
 * The size is read from the "Size" field and converted to a 64-bit unsigned
 * integer. If the field is missing or cannot be converted, zero is returned.
 *
 * @param map Serialized raw-data block object.
 * @return Uncompressed size in bytes, or zero if the size value is invalid.
 */
std::uint64_t getRawBlockObjectSize(const QVariantMap& map)
{
    bool ok = false;
    const auto size = map.value("Size").toULongLong(&ok);

    return ok ? size : 0;
}

}

QVariantMap bytesToBlobVariantMap(const char* bytes, std::uint64_t numberOfBytes)
{
    try {
        if (!mv::projects().hasProject())
            throw mv::ManiVaultException(SeverityLevel::Error, "Unable to save raw data", "No project is currently open", __FUNCTION__, {});

        auto createCodec = []() -> SharedCodec {
            return mv::projects().getCurrentProject()->getCompressionAction().createCodec(nullptr);
        };

        const auto blockSizeInBytes = static_cast<std::uint64_t>(mv::projects().getCurrentProject()->getCompressionAction().getCodecSettingsAction()->getBlockSizeAction().getValue()) << 20;

        auto jobs = makeEncodeBlockJobs(bytes, numberOfBytes, createCodec, blockSizeInBytes);

        const auto saveDir = QDir::cleanPath(projects().getTemporaryDirPath(AbstractProjectManager::TemporaryDirType::Save));

        for (auto& job : jobs)
            job._result = encodeBlock(job, saveDir);

        return makeBlobVariantMap(numberOfBytes, blockSizeInBytes, createCodec()->getName(), jobs);
    }
    catch (const ManiVaultException& exception) {
        throw exception.withAddedDetails({{ "NumberOfBytes", QString::number(numberOfBytes) }});
    }
    catch (const std::exception& exception) {
        throw ManiVaultException(SeverityLevel::Error, "Failed to convert raw data to variant map", exception.what(), __FUNCTION__, {
            { "NumberOfBytes", QString::number(numberOfBytes) }
        });
    }
}

UniqueWorkflowPlan bytesToBlobVariantMapWorkflow(const char* bytes, std::uint64_t numberOfBytes)
{
    try {
        if (!bytes && numberOfBytes > 0)
            throw std::invalid_argument("bytes is null");

        if (!mv::projects().hasProject()) {
            throw mv::ManiVaultException(SeverityLevel::Error, "Unable to save raw data", "No project is currently open", __FUNCTION__, {});
        }

        auto createCodec = []() -> SharedCodec {
            return mv::projects().getCurrentProject()->getCompressionAction().createCodec(nullptr);
        };

        const auto maxBlockSizeInBytes = static_cast<std::uint64_t>(mv::projects().getCurrentProject()->getCompressionAction().getCodecSettingsAction()->getBlockSizeAction().getValue()) << 20;

        auto encodeBlockJobs = std::make_shared<EncodeBlockJobs>(makeEncodeBlockJobs(bytes, numberOfBytes, createCodec, maxBlockSizeInBytes));

        const auto codecName    = createCodec()->getName();
        const auto saveDir      = QDir::cleanPath(projects().getTemporaryDirPath(AbstractProjectManager::TemporaryDirType::Save));

        auto plan = std::make_unique<workflow::WorkflowPlan>("Encode blob");

        WorkflowPlan::Jobs encodeJobs;
        encodeJobs.reserve(encodeBlockJobs->size());

        for (qsizetype i = 0; i < encodeBlockJobs->size(); ++i) {
            encodeJobs.emplace_back(QString("Encode block %1").arg(i), [encodeBlockJobs, i, saveDir](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
                auto& encodeBlockJob = (*encodeBlockJobs)[i];

                encodeBlockJob._result = encodeBlock(encodeBlockJob, saveDir);
            }, WorkflowPlan::JobThreadAffinity::CurrentWorkerThread, WorkflowPlan::JobProgressMode::Atomic);
        }

        if (!encodeJobs.empty()) {
            plan->addBatchedParallelStage("Encode blocks", std::move(encodeJobs), [](const SharedWorkflowExecutionContext& executionContext) {
                return executionContext->getState()->getOptions().batching.dataBlockEncodingBatchSize;
            });
        }

        plan->addSequentialStage("Publish blob map", [encodeBlockJobs, numberOfBytes, maxBlockSizeInBytes, codecName](const WorkflowPlan::Job&, const workflow::SharedWorkflowExecutionContext& executionContext) {
            executionContext->setOutput(makeBlobVariantMap(numberOfBytes, maxBlockSizeInBytes, codecName, *encodeBlockJobs));
        }, WorkflowPlan::JobThreadAffinity::CurrentWorkerThread, 1.0);

        return plan;
    }
    catch (const ManiVaultException& exception) {
        throw exception.withAddedDetails({
            { "NumberOfBytes", QString::number(numberOfBytes) }
        });
    }
    catch (const std::exception& exception) {
        throw ManiVaultException(SeverityLevel::Error, "Failed to create byte-to-blob workflow", exception.what(), __FUNCTION__, {
            { "NumberOfBytes", QString::number(numberOfBytes) }
        });
    }
    catch (...) {
        throw ManiVaultException(SeverityLevel::Error, "Failed to create byte-to-blob workflow", "An unknown error occurred", __FUNCTION__, {
            { "NumberOfBytes", QString::number(numberOfBytes) }
        });
    }
}

void populateBytesFromBlobMap(QVariantMap variantMap, char* destination, std::uint64_t destinationSize)
{
    if (variantMap.isEmpty()) {
        qWarning("Cannot populate data buffer from variant map, variant map is empty");
        return;
    }

    if (!destination)
        throw std::invalid_argument("populateBytesFromBlobMap destination is null");

    if (destinationSize == 0)
        return;

    auto jobs = makeDecodeBlockJobs(variantMap);

    for (const auto& job : jobs) {
        if (job._offset > destinationSize || job._size > destinationSize - job._offset)
            throw std::runtime_error("Decode block range exceeds destination buffer");

        if (job._uri.isEmpty())
            decodeBlockFromBase64To(job, destination, destinationSize);
        else
            decodeBlockFromFileTo(job, destination, destinationSize);
    }
}

UniqueWorkflowPlan populateBytesFromBlobMapWorkflow(QVariantMap variantMap, char* destination, std::uint64_t destinationSize, const workflow::WorkflowOptions& options /*= {}*/)
{
    if (variantMap.isEmpty()) {
        qWarning("Failed to populate data buffer from variant map, variant map is empty");

        return std::make_unique<WorkflowPlan>("Decode Blocks (Empty Variant Map)");
    }

    auto decodeBlockJobs = makeDecodeBlockJobs(variantMap);

    auto plan = std::make_unique<WorkflowPlan>("Decode Blocks");

    WorkflowPlan::Jobs jobs;
    std::int32_t jobIndex = 0;

    for (const auto& decodeBlockJob : decodeBlockJobs) {
        jobs.emplace_back(QString("Decode Block %1").arg(jobIndex++), [decodeBlockJob, destination, destinationSize](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
            if (decodeBlockJob._uri.isEmpty())
                decodeBlockFromBase64To(decodeBlockJob, destination, destinationSize);
            else
                decodeBlockFromFileTo(decodeBlockJob, destination, destinationSize);
        });
    }

    plan->addBatchedParallelStage("Decode Blocks", std::move(jobs), options.batching.dataBlockDecodingBatchSize);

    return plan;
}

QByteArray bytesFromBlobVariantMap(QVariantMap variantMap)
{
    if (variantMap.isEmpty()) {
        throw ManiVaultException(SeverityLevel::Error, "Failed to populate data buffer from variant map", "Variant map is empty", __FUNCTION__, {
            { "VariantMap", variantMap },
        });
    }

    variantMapMustContain(variantMap, "BlockSize");
    variantMapMustContain(variantMap, "Blocks");
    variantMapMustContain(variantMap, "Size");

    bool totalSizeOk = false;

    const auto totalSize    = variantMap.value("Size").toULongLong(&totalSizeOk);

    if (!totalSizeOk) {
        throw ManiVaultException(SeverityLevel::Error, "Failed to populate data buffer from variant map", "Decoded buffer size is invalid", __FUNCTION__, {
            { "VariantMap", variantMap },
        });
    }

    if (totalSize == 0) {
        throw ManiVaultException(SeverityLevel::Error, "Failed to populate data buffer from variant map", "Decoded buffer size is zero", __FUNCTION__, {
            { "VariantMap", variantMap },
        });
    }

    if (totalSize > static_cast<std::uint64_t>(std::numeric_limits<qsizetype>::max())) {
        throw ManiVaultException(SeverityLevel::Error, "Failed to populate data buffer from variant map", "Decoded buffer size exceeds QByteArray size limit", __FUNCTION__, {
            { "Size", QString::number(totalSize) },
            { "MaxSize", QString::number(std::numeric_limits<qsizetype>::max()) }
        });
    }

    QByteArray bytes;

    bytes.resize(static_cast<qsizetype>(totalSize));

    populateBytesFromBlobMap(variantMap, bytes.data(), static_cast<std::uint64_t>(bytes.size()));

    return bytes;
}

void variantMapMustContain(const QVariantMap& variantMap, const QString& key, std::source_location sourceLocation)
{
    if (variantMap.contains(key))
        return;

    if (settings().getMiscellaneousSettings().getIgnoreLoadingErrorsAction().isChecked()) {
        throw ManiVaultException(SeverityLevel::Warning, "Missing key in QVariantMap", QString("Variant map is missing key '%1', but loading errors are set to be ignored").arg(key), __FUNCTION__, {
            { "Key", key },
            { "VariantMap", describeVariantMapKeys(variantMap) }
        }, sourceLocation);
    }

    throw ManiVaultException(SeverityLevel::Error, "Missing key in QVariantMap", QString("Variant map is missing required key '%1'").arg(key),
        __FUNCTION__,
        {
            { "Key", key },
            { "VariantMap", describeVariantMapKeys(variantMap) }
        }, sourceLocation);
}

QVariantMap storeOnDisk(const QStringList& list)
{
    QByteArray byteArray;
    QDataStream dataStream(&byteArray, QIODevice::WriteOnly);

	dataStream << list;

    return bytesToBlobVariantMap((const char*)byteArray.data(), byteArray.size());
}

QVariantMap storeOnDisk(const QVector<uint32_t>& vector)
{
    QByteArray byteArray;
    QDataStream dataStream(&byteArray, QIODevice::WriteOnly);

	dataStream << vector;

    return bytesToBlobVariantMap((const char*)byteArray.data(), byteArray.size());
}

void loadFromDisk(const QVariantMap& variantMap, QStringList& list)
{
    std::vector<char> bytes(variantMap["Size"].value<uint64_t>());

	populateBytesFromBlobMap(variantMap, (char*)bytes.data(), static_cast<std::uint64_t>(bytes.size()));

	QByteArray byteArray(bytes.data(), bytes.size());
    QDataStream dataStream(byteArray);

	dataStream >> list;
}

void loadFromDisk(const QVariantMap& variantMap, QVector<uint32_t>& vec)
{
    std::vector<char> bytes(variantMap["Size"].value<uint64_t>());

	populateBytesFromBlobMap(variantMap, (char*)bytes.data(), static_cast<std::uint64_t>(bytes.size()));

	QByteArray byteArray(bytes.data(), bytes.size());
    QDataStream dataStream(byteArray);

	dataStream >> vec;
}

std::uint64_t estimateRawBlockTotalSize(const QVariant& value)
{
    std::uint64_t totalSize = 0;

    if (value.metaType() == QMetaType::fromType<QVariantMap>()) {
        const QVariantMap map = value.toMap();

        if (isVariantMapRawBlockObject(map))
            return getRawBlockObjectSize(map);

        for (auto it = map.cbegin(); it != map.cend(); ++it)
            totalSize += estimateRawBlockTotalSize(it.value());

        return totalSize;
    }

    if (value.metaType() == QMetaType::fromType<QVariantList>()) {
        const QVariantList list = value.toList();

        for (const QVariant& item : list)
            totalSize += estimateRawBlockTotalSize(item);

        return totalSize;
    }

    return 0;
}

}
