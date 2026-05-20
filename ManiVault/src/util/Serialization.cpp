// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Serialization.h"
#include "CoreInterface.h"
#include "Application.h"
#include "CodecRegistry.h"

#include "Exception/SerializationException.h"

#include <QUuid>

#include <exception>

#include <math.h>
#include <limits>

namespace mv::util {

void saveRawDataToBinaryFile(const char* bytes, const std::uint64_t& numberOfBytes, const QString& filePath)
{
    // Output directory
    const auto outputDirectory = QFileInfo(filePath).dir();

    // Exit prematurely if the target directory does not exist
    if (!QFileInfo(filePath).dir().exists())
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to save raw data to binary file",
            QString("Unable to save data in %1, the directory does not exist").arg(outputDirectory.dirName()),
            __FUNCTION__,
            {
                { "FilePath", filePath },
                { "OutputDirectory", outputDirectory.dirName() }
            }
        );

    // Create binary file
    QFile binaryFile(filePath);

    // And save it do disk
    if (!binaryFile.open(QIODevice::WriteOnly))
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to save raw data to binary file",
            "Unable to open file for writing",
            __FUNCTION__,
            {
                { "FilePath", filePath }
            }
        );

    binaryFile.write(QByteArray::fromRawData(bytes, numberOfBytes)); // usng QByteArray::fromRawData(const char *data, qsizetype size) so that the bytes are not copied.
    binaryFile.close();
}

void loadRawDataFromBinaryFile(char* bytes, const std::uint64_t& numberOfBytes, const QString& filePath)
{
    // Exit prematurely if the target file does not exist
    if (!QFileInfo(filePath).exists())
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to load raw data from binary file",
            QString("Unable to load binary file, %1 does not exist").arg(filePath),
            __FUNCTION__,
            {
                { "FilePath", filePath }
            }
        );

    // Create binary file
    QFile binaryFile(filePath);

    // Open the file read-only
    if (!binaryFile.open(QIODevice::ReadOnly))
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to load raw data from binary file",
            "Unable to open file for reading",
            __FUNCTION__,
            {
                { "FilePath", filePath }
            }
        );

    // Read the raw data from the file directly into bytes
    const auto numberOfBytesRead = binaryFile.read(bytes, numberOfBytes);

    // Except if the number of bytes in the file deviates from the number of requested bytes
    if (numberOfBytesRead != numberOfBytes)
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to load raw data from binary file",
            "Number of requested bytes is not the same as in the file",
            __FUNCTION__,
            {
                { "FilePath", filePath },
                { "RequestedBytes", QString::number(numberOfBytes) },
                { "ReadBytes", QString::number(numberOfBytesRead) }
            }
        );
}

QByteArray readBinaryFileToByteArray(const QString& filePath)
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to read binary file",
            QString("Unable to open file %1").arg(filePath),
            __FUNCTION__,
            {
                { "FilePath", filePath }
            }
        );

    return file.readAll();
}

static EncodeBlockResult encodeBlock(const EncodeBlockJob& job, const QString& saveDir, const std::function<std::shared_ptr<BlobCodec>()>& createCodec)
{
    EncodeBlockResult result;

    try {
        QVariantMap blockVariantMap;

        blockVariantMap["Offset"] = QVariant::fromValue(job._offset);
        blockVariantMap["Size"]   = QVariant::fromValue(job._size);

        auto codec = createCodec();

        const auto fileName     = QUuid::createUuid().toString(QUuid::WithoutBraces) + codec->getFileExtension();
        const auto filePath     = QDir::cleanPath(saveDir + QDir::separator() + fileName);

        std::uint64_t numberOfEncodedBytes = 0;

    	codec->encodeToFile(job._rawData, filePath, &numberOfEncodedBytes);

        blockVariantMap["CompressedSize"]   = QVariant::fromValue<std::uint64_t>(numberOfEncodedBytes);
        blockVariantMap["URI"]              = fileName;

        result._block = std::move(blockVariantMap);
    }
    catch (const ManiVaultException& maniVaultException)
    {
        throw maniVaultException.withAddedDetails({
            { "Offset", QString::number(job._offset) },
            { "Size", QString::number(job._size) }
		});
    }
    catch (const std::exception& exception) {
        throw ManiVaultException(SeverityLevel::Error,
            "Enable to encode block",
            exception.what(),
            __FUNCTION__,
            {
                { "Offset", QString::number(job._offset) },
				{ "Size", QString::number(job._size) },
                { "SaveDir", saveDir }
            });
    }

    return result;
}

QVariantMap rawDataToVariantMap(const char* bytes, const std::uint64_t& numberOfBytes, const BlobCodec* blobCodecOverride /*= nullptr*/)
{
    try {
        if (!mv::projects().hasProject())
            throw mv::ManiVaultException(
                SeverityLevel::Error,
                "Unable to save raw data",
                "No project is currently open",
                __FUNCTION__,
                {}
            );

        auto createCodec = [&]() -> std::shared_ptr<BlobCodec> {
            return mv::projects().getCurrentProject()->getCompressionAction().createCodec(nullptr);
		};

        const std::uint64_t maxBlockSizeInBytes = mv::projects().getCurrentProject()->getCompressionAction().getCodecSettingsAction()->getBlockSizeAction().getValue() << 20;

	    QVariantMap rawData;

	    rawData["Size"]     = QVariant::fromValue(numberOfBytes);
	    rawData["Codec"]    = QVariant::fromValue(createCodec()->getName());

	    const auto numberOfBlocks = (numberOfBytes + maxBlockSizeInBytes - 1) / maxBlockSizeInBytes;

        EncodeBlockJobs encodeBlockJobs;

	    encodeBlockJobs.reserve(static_cast<int>(numberOfBlocks));

	    std::uint64_t offset = 0;

	    while (offset < numberOfBytes)
	    {
	        const auto blockSize = std::min(maxBlockSizeInBytes, numberOfBytes - offset);

	        EncodeBlockJob job;

	        job._offset     = offset;
	        job._size       = blockSize;
	        job._rawData    = QByteArray(bytes + offset, static_cast<qsizetype>(blockSize));

	        encodeBlockJobs.push_back(std::move(job));

	        offset += blockSize;
	    }

	    const auto saveDir = QDir::cleanPath(projects().getTemporaryDirPath(AbstractProjectManager::TemporaryDirType::Save));

        WorkflowPlan encodeWorkflowPlan("Encode Blocks");
        WorkflowPlan::Jobs encodeJobs;

        std::int32_t encodeBlockJobIndex = 0;

        for (auto& encodeBlockJob : encodeBlockJobs) {
            auto* encodeBlockJobPtr = &encodeBlockJob;

            encodeJobs.emplace_back(QString("Encode Block %1").arg(QString::number(encodeBlockJobIndex)), [encodeBlockJobPtr, saveDir, createCodec](const WorkflowPlan::Job& job) {
                try {
                    encodeBlockJobPtr->_result = encodeBlock(*encodeBlockJobPtr, saveDir, createCodec);
                }
                catch (std::exception& e) {
                    Serializable::reportSerializationError("Encoder", "Failed to encode block: " + QString::fromStdString(e.what()));
                }
                catch (...) {
                    Serializable::reportSerializationError("Encoder", "Failed to encode block");
                }
            });

            ++encodeBlockJobIndex;
        }

        if (!encodeJobs.empty()) {
            encodeWorkflowPlan.addParallelStage("Encode Blocks", encodeJobs);
            encodeWorkflowPlan.executeBlocking(mv::projects().getWorkflowPlanExecutor(), WorkflowExecutionContext::current());
        }

        QVariantList blocks;

        blocks.reserve(encodeBlockJobs.size());

        for (const auto& encodeBlockJob : encodeBlockJobs) {
            if (!encodeBlockJob._result._error.isEmpty())
                throw ManiVaultException(
                    SeverityLevel::Error,
                    "Failed to encode block",
                    encodeBlockJob._result._error,
                    __FUNCTION__,
                    {
                        { "BlockIndex", QString::number(&encodeBlockJob - &encodeBlockJobs[0]) }
                    });

            blocks.push_back(encodeBlockJob._result._block);
        }

        rawData["NumberOfBlocks"]   = QVariant::fromValue(numberOfBlocks);
        rawData["BlockSize"]        = QVariant::fromValue(maxBlockSizeInBytes);
        rawData["Blocks"]           = QVariant::fromValue(blocks);

        return rawData;
    }
	catch (const ManiVaultException& maniVaultException) {
        throw maniVaultException.withAddedDetails({
            { "NumberOfBytes", QString::number(numberOfBytes) }
        });
    }
    catch (const std::exception& exception) {
        throw ManiVaultException(SeverityLevel::Error,
            "Failed to convert raw data to variant map",
            exception.what(),
            __FUNCTION__,
            {
                { "NumberOfBytes", QString::number(numberOfBytes) }
            });
    }
    catch (...) {
	    throw ManiVaultException(SeverityLevel::Error,
			"Failed to convert raw data to variant map",
			"An unknown error occurred",
			__FUNCTION__,
			{
				{ "NumberOfBytes", QString::number(numberOfBytes) }
			});
    }
}

DecodeBlockResult decodeBlockFromFileTo(const DecodeBlockJob& decodeBlockJob, const std::function<std::shared_ptr<BlobCodec>()>& createCodec, char* destination, std::uint64_t destinationSize)
{
    DecodeBlockResult result;

    try {
	    result._offset  = decodeBlockJob._offset;
	    result._size    = decodeBlockJob._size;

	    if (!destination)
	        throw ManiVaultException(SeverityLevel::Error,
                "Unable to decode block from file to buffer",
	            "Destination buffer is null",
	            __FUNCTION__,
	            {
	                { "URI", decodeBlockJob._uri }
	            });

	    if (decodeBlockJob._offset > static_cast<std::uint64_t>(std::numeric_limits<qsizetype>::max()) || decodeBlockJob._size > static_cast<std::uint64_t>(std::numeric_limits<qsizetype>::max())) {
	        throw ManiVaultException(SeverityLevel::Error,
                "Unable to decode block from file to buffer",
	            "Decode block offset or size exceeds std::uint64_t range",
	            __FUNCTION__,
	            {
	                { "URI", decodeBlockJob._uri }
	            });
	    }

	    const auto offset   = static_cast<std::uint64_t>(decodeBlockJob._offset);
	    const auto size     = static_cast<std::uint64_t>(decodeBlockJob._size);

	    if (offset < 0 || size < 0 || offset > destinationSize || size > destinationSize - offset) {
            throw ManiVaultException(SeverityLevel::Error,
                "Unable to decode block from file to buffer",
                QString("Decode block destination range is out of bounds. offset=%1, size=%2, destinationSize=%3, uri=%4")
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

	    auto codec = createCodec();

	    if (!codec)
	        throw ManiVaultException(SeverityLevel::Error,
                "Unable to decode block from file to buffer",
	            "Failed to create blob codec",
	            __FUNCTION__,
	            {
	                { "URI", decodeBlockJob._uri }
	            });

	    if (decodeBlockJob._uri.isEmpty())
	        throw ManiVaultException(SeverityLevel::Error,
                "Unable to decode block from file to buffer",
	            "Block URI is empty",
	            __FUNCTION__,
	            {
	                { "URI", decodeBlockJob._uri }
	            });

	    codec->decodeFromFileTo(decodeBlockJob._uri, destination + offset, size);
    }
    catch (const ManiVaultException&) {

    	// Rethrow ManiVaultExceptions as they are already properly constructed
    	throw;
    } 
    catch (const std::exception& exception) {

        // Upgrade to ManiVaultException with context
        throw ManiVaultException(
            SeverityLevel::Error,
            "Unable to decode block from file to buffer",
            exception.what(),
            __FUNCTION__,
            {
                { "Offset", QString::number(decodeBlockJob._offset) },
                { "Size", QString::number(decodeBlockJob._size) },
                { "DestinationSize", QString::number(destinationSize) },
                { "URI", decodeBlockJob._uri }
            });
    }
	catch(...) {

        // Handle any other types of exceptions
        throw ManiVaultException(
            SeverityLevel::Error,
            "Unable to decode block from file to buffer",
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

DecodeBlockResult decodeBlockFromBase64To(const DecodeBlockJob& decodeBlockJob, const std::function<std::shared_ptr<BlobCodec>()>& createCodec, char* destination, std::uint64_t destinationSize)
{
    DecodeBlockResult result;

    try {
	    result._offset  = decodeBlockJob._offset;
	    result._size    = decodeBlockJob._size;

	    if (!destination)
            throw ManiVaultException(SeverityLevel::Error,
                "Unable to decode block from base64 to buffer",
                "Destination buffer is null",
                __FUNCTION__,
                {
                    { "URI", decodeBlockJob._uri }
                });

	    const auto offset   = static_cast<std::uint64_t>(decodeBlockJob._offset);
	    const auto size     = static_cast<std::uint64_t>(decodeBlockJob._size);

	    if (offset < 0 || size < 0 || offset > destinationSize || size > destinationSize - offset) {
	        throw ManiVaultException(SeverityLevel::Error,
                "Unable to decode block from base64 to buffer",
	            "Inline decode block out of bounds",
	            __FUNCTION__,
	            {
	                { "Offset", QString::number(decodeBlockJob._offset) },
	                { "Size", QString::number(decodeBlockJob._size) },
	                { "DestinationSize", QString::number(destinationSize) },
	                { "URI", decodeBlockJob._uri }
	            });
	    }

	    auto codec = createCodec();

	    if (!codec)
	        throw ManiVaultException(SeverityLevel::Error,
                "Unable to decode block from base64 to buffer",
	            "Failed to create blob codec",
	            __FUNCTION__,
	            {
	                { "URI", decodeBlockJob._uri }
	            });

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

PopulateDataBufferResult populateDataBufferFromVariantMap(const QVariantMap& variantMap, WorkflowPlan::ConcurrencyMode concurrencyMode)
{
    if (variantMap.isEmpty())
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to populate data buffer from variant map",
            "Variant map is empty",
            __FUNCTION__,
            {
                { "VariantMap", variantMap }
            }
        );

    variantMapMustContain(variantMap, "BlockSize");
    variantMapMustContain(variantMap, "Blocks");
    variantMapMustContain(variantMap, "Size");

    const auto blocks       = variantMap.value("Blocks").toList();
    const bool hasCodec     = variantMap.contains("Codec");
    const auto codecName    = hasCodec ? variantMap.value("Codec").toString() : QStringLiteral("none");
    const auto totalSize    = static_cast<std::uint64_t>(variantMap.value("Size").toULongLong());

    if (totalSize == 0)
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to populate data buffer from variant map",
            "Decoded buffer size is zero",
            __FUNCTION__,
            {
                { "VariantMap", variantMap }
            }
        );

    auto sharedData = SharedDataBuffer::create();

    sharedData->resize(totalSize);

    if (hasCodec && !codecRegistry().isRegistered(codecName)) {
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to populate data buffer from variant map",
            QString("Unable to load raw data, codec %1 is not registered").arg(codecName),
            __FUNCTION__,
            {
                { "CodecName", codecName },
                { "VariantMap", variantMap }
            }
        );
    }

    auto createCodec = [hasCodec, codecName]() -> std::shared_ptr<BlobCodec> {
        if (hasCodec)
            return codecRegistry().createCodec(nullptr, codecName);

        return codecRegistry().createCodec(nullptr, BlobCodec::Type::None);
    };

    DecodeBlockJobs decodeBlockJobs;
    decodeBlockJobs.reserve(blocks.size());

    for (const auto& block : blocks) {
        const auto blockMap = block.toMap();

        variantMapMustContain(blockMap, "Offset");
        variantMapMustContain(blockMap, "Size");

        DecodeBlockJob job;

        job._offset         = blockMap.value("Offset").value<quint64>();
        job._size           = blockMap.value("Size").value<quint64>();
        job._compressedSize = blockMap.value("CompressedSize", 0).value<quint64>();
        
        if (blockMap.contains("URI"))
            job._uri = blockMap.value("URI").toString();

        if (blockMap.contains("Data"))
            job._encodedData = blockMap.value("Data").toString();

        decodeBlockJobs.push_back(std::move(job));
    }

    // Overlap check
    std::sort(decodeBlockJobs.begin(), decodeBlockJobs.end(),
        [](const DecodeBlockJob& a, const DecodeBlockJob& b) {
            return a._offset < b._offset;
    });

    quint64 expectedOffset = 0;

    for (int i = 0; i < decodeBlockJobs.size(); ++i) {
        const auto& job = decodeBlockJobs[i];
        const quint64 end = job._offset + job._size;

        if (job._offset < expectedOffset) {
            throw ManiVaultException(
                SeverityLevel::Error,
                "Failed to populate data buffer from variant map",
                QString("Raw-data block overlap detected at offset %1").arg(job._offset),
                __FUNCTION__,
                {
                    { "CodecName", codecName },
                    { "VariantMap", variantMap }
                }
            );
        }

        // Optional: require contiguous packing
        if (job._offset != expectedOffset) {
            throw ManiVaultException(
                SeverityLevel::Error,
                "Failed to populate data buffer from variant map",
                QString("Raw-data block gap detected: expected offset %1, got %2").arg(expectedOffset).arg(job._offset),
                __FUNCTION__,
                {
                    { "CodecName", codecName },
                    { "VariantMap", variantMap }
                }
            );
        }

        expectedOffset = end;
    }

    if (expectedOffset != totalSize) {
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to populate data buffer from variant map",
            QString("Raw-data blocks do not cover total size. Expected %1 bytes, got %2 bytes").arg(totalSize).arg(expectedOffset),
            __FUNCTION__,
            {
                { "CodecName", codecName },
                { "VariantMap", variantMap },
            }
        );
    }

    WorkflowPlan decodeWorkflowPlan("Decode Blocks");

    WorkflowPlan::Jobs decodeJobs;

    std::int32_t decodeBlockJobIndex = 0;

    for (auto& decodeBlockJob : decodeBlockJobs) {
        decodeJobs.emplace_back(QString("Decode Block %1").arg(QString::number(decodeBlockJobIndex)), [decodeBlockJob, sharedData, totalSize, createCodec](const WorkflowPlan::Job& job) {
            if (decodeBlockJob._uri.isEmpty()) {
                decodeBlockFromBase64To(decodeBlockJob, createCodec, sharedData->data(), totalSize);
            } else {
                decodeBlockFromFileTo(decodeBlockJob, createCodec, sharedData->data(), totalSize);
            }
        }, WorkflowPlan::JobThreadAffinity::CurrentWorkerThread);

        ++decodeBlockJobIndex;
    }

    decodeWorkflowPlan.addParallelStage("Decode blocks", decodeJobs);
    decodeWorkflowPlan.addSequentialStage("Finalize", [totalSize](WorkflowPlan::Job& job) {
        if (auto workflowExecutionContext = WorkflowExecutionContext::current()) {
            auto state = workflowExecutionContext->getState();

            if (!state)
                return;

            state->metrics().addInteger("project.data.bytes_loaded", totalSize);
        }
    });
   
    auto sharedExecutor = SharedWorkflowPlanExecutor(mv::projects().getWorkflowPlanExecutor());

    PopulateDataBufferResult result;

    result._data = sharedData;

    if (concurrencyMode == WorkflowPlan::ConcurrencyMode::Parallel) {
        result._async = true;
        result._future = decodeWorkflowPlan.executeAsync(sharedExecutor, WorkflowExecutionContext::current());
    }
    else {
        result._async = false;
        result._workflowResult = decodeWorkflowPlan.executeBlocking(sharedExecutor, WorkflowExecutionContext::current());
    }

    return result;
}

PopulateDataBufferResult populateDataBufferFromVariantMapAsync(const QVariantMap& variantMap, QObject* context, PopulateDataReadyCallback populated /*= {}*/)
{
    auto result = populateDataBufferFromVariantMap(variantMap, WorkflowPlan::ConcurrencyMode::Parallel);

    if (populated) {
        auto watcher = new QFutureWatcher<SharedWorkflowResult>(context);

        QObject::connect(
            watcher,
            &QFutureWatcher<SharedWorkflowResult>::finished,
            context ? context : watcher,
            [watcher, data = result._data, populated = std::move(populated)]() mutable {
                watcher->deleteLater();

                //const auto workflowResult = watcher->result();

                //if (!workflowResult || !workflowResult->isSuccess()) {
                //    qCritical() << "Failed to populate data buffer";
                //    return;
                //}

                populated(data);
            }
        );

        watcher->setFuture(result._future.getFuture());
    }

    return result;
}

QByteArray populateDataBufferFromVariantMapSync(const QVariantMap& variantMap)
{
    auto result = populateDataBufferFromVariantMap(variantMap, WorkflowPlan::ConcurrencyMode::Sequential);

    result._future.waitForFinished();

    return *result._data;
}

WorkflowResultFuture populateDataBufferFromVariantMapToRawBuffer(const QVariantMap& variantMap, char* destination, std::uint64_t destinationSize, WorkflowPlan::ConcurrencyMode concurrencyMode)
{
    if (variantMap.isEmpty())
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to populate data buffer from variant map",
            "Variant map is empty",
            __FUNCTION__,
            {
                { "VariantMap", variantMap },
                { "Destination", QString::number(reinterpret_cast<std::uintptr_t>(destination)) },
                { "DestinationSize", QString::number(destinationSize) }
            }
        );

    if (!destination)   
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to populate data buffer from variant map",
            "Destination buffer is null",
            __FUNCTION__,
            {
                { "VariantMap", variantMap },
                { "Destination", QString::number(reinterpret_cast<std::uintptr_t>(destination)) },
                { "DestinationSize", QString::number(destinationSize) }
            }
        );

    variantMapMustContain(variantMap, "BlockSize");
    variantMapMustContain(variantMap, "Blocks");
    variantMapMustContain(variantMap, "Size");

    const auto blocks = variantMap.value("Blocks").toList();
    const bool hasCodec = variantMap.contains("Codec");
    const auto codecName = hasCodec
        ? variantMap.value("Codec").toString()
        : QStringLiteral("none");

    const auto totalSize = variantMap.value("Size").toULongLong();

    if (totalSize == 0)
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to populate data buffer from variant map",
            "Decoded buffer size is zero",
            __FUNCTION__,
            {
                { "VariantMap", variantMap },
                { "Destination", QString::number(reinterpret_cast<std::uintptr_t>(destination)) },
                { "DestinationSize", QString::number(destinationSize) }
            }
        );

    if (totalSize != destinationSize) {
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to populate data buffer from variant map",
            "Destination buffer size mismatch",
            __FUNCTION__,
            {
                { "VariantMap", variantMap },
                { "Destination", QString::number(reinterpret_cast<std::uintptr_t>(destination)) },
                { "DestinationSize", QString::number(destinationSize) },
                { "ExpectedSize", QString::number(totalSize) }
            }
        );
    }

    if (hasCodec && !codecRegistry().isRegistered(codecName)) {
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to populate data buffer from variant map",
            QString("Unable to load raw data, codec %1 is not registered").arg(codecName),
            __FUNCTION__,
            {
                { "VariantMap", variantMap },
                { "Destination", QString::number(reinterpret_cast<std::uintptr_t>(destination)) },
                { "DestinationSize", QString::number(destinationSize) },
                { "Codec", codecName }
            }
        );
    }

    auto createCodec = [hasCodec, codecName]() -> std::shared_ptr<BlobCodec> {
        if (hasCodec)
            return codecRegistry().createCodec(nullptr, codecName);

        return codecRegistry().createCodec(nullptr, BlobCodec::Type::None);
    };

    DecodeBlockJobs decodeBlockJobs;
    decodeBlockJobs.reserve(blocks.size());

    for (const auto& block : blocks) {
        const auto blockMap = block.toMap();

        variantMapMustContain(blockMap, "Offset");
        variantMapMustContain(blockMap, "Size");

        DecodeBlockJob job;

        job._offset = blockMap.value("Offset").value<quint64>();
        job._size = blockMap.value("Size").value<quint64>();
        job._compressedSize = blockMap.value("CompressedSize", 0).value<quint64>();

        if (blockMap.contains("URI"))
            job._uri = blockMap.value("URI").toString();

        if (blockMap.contains("Data"))
            job._encodedData = blockMap.value("Data").toString();

        decodeBlockJobs.push_back(std::move(job));
    }

    std::sort(
        decodeBlockJobs.begin(),
        decodeBlockJobs.end(),
        [](const DecodeBlockJob& a, const DecodeBlockJob& b) {
            return a._offset < b._offset;
        }
    );

    quint64 expectedOffset = 0;

    for (const auto& job : decodeBlockJobs) {
        if (job._size > std::numeric_limits<quint64>::max() - job._offset)
            throw ManiVaultException(
                SeverityLevel::Error,
                "Failed to populate data buffer from variant map",
                "Raw-data block offset overflow detected",
                __FUNCTION__,
                {
                    { "VariantMap", variantMap },
                    { "Destination", QString::number(reinterpret_cast<std::uintptr_t>(destination)) },
                    { "DestinationSize", QString::number(destinationSize) },
                    { "BlockOffset", QString::number(job._offset) },
                    { "BlockSize", QString::number(job._size) }
                }
            );

        const quint64 end = job._offset + job._size;

        if (job._offset < expectedOffset) {
            throw ManiVaultException(
                SeverityLevel::Error,
                "Failed to populate data buffer from variant map",
                "Raw-data block overlap detected",
                __FUNCTION__,
                {
                    { "VariantMap", variantMap },
                    { "Destination", QString::number(reinterpret_cast<std::uintptr_t>(destination)) },
                    { "DestinationSize", QString::number(destinationSize) },
                    { "BlockOffset", QString::number(job._offset) },
                    { "ExpectedOffset", QString::number(expectedOffset) }
                }
            );
        }

        if (job._offset != expectedOffset) {
            throw ManiVaultException(
                SeverityLevel::Error,
                "Failed to populate data buffer from variant map",
                "Raw-data block gap detected",
                __FUNCTION__,
                {
                    { "VariantMap", variantMap },
                    { "Destination", QString::number(reinterpret_cast<std::uintptr_t>(destination)) },
                    { "DestinationSize", QString::number(destinationSize) },
                    { "BlockOffset", QString::number(job._offset) },
                    { "ExpectedOffset", QString::number(expectedOffset) }
                }
            );
        }

        if (end > destinationSize)
            throw ManiVaultException(
                SeverityLevel::Error,
                "Failed to populate data buffer from variant map",
                "Raw-data block exceeds destination buffer size",
                __FUNCTION__,
                {
                    { "VariantMap", variantMap },
                    { "Destination", QString::number(reinterpret_cast<std::uintptr_t>(destination)) },
                    { "DestinationSize", QString::number(destinationSize) },
                    { "BlockOffset", QString::number(job._offset) },
                    { "BlockSize", QString::number(job._size) }
                }
            );

        expectedOffset = end;
    }

    if (expectedOffset != totalSize) {
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to populate data buffer from variant map",
            "Raw-data blocks do not cover total size",
            __FUNCTION__,
            {
                { "VariantMap", variantMap },
                { "Destination", QString::number(reinterpret_cast<std::uintptr_t>(destination)) },
                { "DestinationSize", QString::number(destinationSize) },
                { "ExpectedOffset", QString::number(expectedOffset) },
                { "TotalSize", QString::number(totalSize) }
            }
        );
    }

    WorkflowPlan decodeWorkflowPlan("Decode Raw Buffer Blocks");
    WorkflowPlan::Jobs decodeJobs;

    std::int32_t decodeBlockJobIndex = 0;

    for (const auto& decodeBlockJob : decodeBlockJobs) {
        decodeJobs.emplace_back(
            QString("Decode Block %1").arg(decodeBlockJobIndex),
            [decodeBlockJob, destination, destinationSize, createCodec](const WorkflowPlan::Job&) {
                if (decodeBlockJob._uri.isEmpty()) {
                    decodeBlockFromBase64To(
                        decodeBlockJob,
                        createCodec,
                        destination,
                        destinationSize
                    );
                }
                else {
                    decodeBlockFromFileTo(
                        decodeBlockJob,
                        createCodec,
                        destination,
                        destinationSize
                    );
                }
            },
            WorkflowPlan::JobThreadAffinity::CurrentWorkerThread
        );

        ++decodeBlockJobIndex;
    }

    decodeWorkflowPlan.addParallelStage("Decode blocks", decodeJobs);

    decodeWorkflowPlan.addSequentialStage(
        "Finalize",
        [totalSize](WorkflowPlan::Job&) {
            if (auto workflowExecutionContext = WorkflowExecutionContext::current()) {
                auto state = workflowExecutionContext->getState();

                if (!state)
                    return;

                state->metrics().addInteger("project.data.bytes_loaded", totalSize);
            }
        }
    );

    auto options = WorkflowExecutionOptions{};

    if (auto context = WorkflowExecutionContext::current()) {
        if (auto state = context->getState())
            options = state->getExecutionOptions();
    }

    options._parallel = concurrencyMode == WorkflowPlan::ConcurrencyMode::Parallel;

    const auto sharedExecutor = SharedWorkflowPlanExecutor(mv::projects().getWorkflowPlanExecutor());

    if (concurrencyMode == WorkflowPlan::ConcurrencyMode::Parallel)
        return decodeWorkflowPlan.executeAsync(sharedExecutor, WorkflowExecutionContext::current() ? WorkflowExecutionContext::current() : nullptr, options);

    const auto blockingResult =
        decodeWorkflowPlan.executeBlocking(sharedExecutor, WorkflowExecutionContext::current() ? WorkflowExecutionContext::current() : nullptr, options);

    return WorkflowResultFuture::makeReady(blockingResult);
}

WorkflowResultFuture populateDataBufferFromVariantMapToRawBufferAsync(const QVariantMap& variantMap, char* destination, std::uint64_t destinationSize, QObject* context, PopulateDoneCallback onPopulated)
{
    auto future = populateDataBufferFromVariantMapToRawBuffer(
        variantMap,
        destination,
        destinationSize,
        WorkflowPlan::ConcurrencyMode::Parallel
    );

    if (onPopulated) {
        auto watcher = new QFutureWatcher<SharedWorkflowResult>(context);

        QObject::connect(
            watcher,
            &QFutureWatcher<SharedWorkflowResult>::finished,
            context ? context : watcher,
            [watcher, onPopulated = std::move(onPopulated)]() mutable {
                watcher->deleteLater();

                //const auto result = watcher->result();

                //if (!result || !result->isSuccess()) {
                //    qCritical() << "Failed to populate raw data buffer";
                //    return;
                //}

                onPopulated();
            }
        );

        watcher->setFuture(future.getFuture());
    }

    return future;
}

void populateDataBufferFromVariantMapToRawBufferSync(const QVariantMap& variantMap, char* destination, std::uint64_t destinationSize)
{
    if (!destination)
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to populate data buffer from variant map to raw buffer",
            "Destination buffer is null",
            __FUNCTION__,
            {
                { "VariantMap", variantMap },
                { "DestinationPointer", QString::number(reinterpret_cast<std::uintptr_t>(destination)) },
                { "DestinationSize", QString::number(destinationSize) }
            }
        );

    auto result = populateDataBufferFromVariantMap(variantMap, WorkflowPlan::ConcurrencyMode::Sequential);

    const auto decodedSize = static_cast<std::uint64_t>(result._data->size());

    if (decodedSize != destinationSize)
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to populate data buffer from variant map to raw buffer",
            "Decoded data size does not match destination size",
            __FUNCTION__,
            {
                { "VariantMap", variantMap },
                { "DestinationPointer", QString::number(reinterpret_cast<std::uintptr_t>(destination)) },
                { "DestinationSize", QString::number(destinationSize) },
                { "DecodedSize", QString::number(decodedSize) }
            }
        );
        
    try {
        std::memcpy(destination, result._data->constData(), static_cast<size_t>(decodedSize));
    }
    catch (const std::exception& exception) {
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to populate data buffer from variant map to raw buffer (memcpy)",
            exception.what(),
            __FUNCTION__,
            {
                { "VariantMap", variantMap },
                { "DestinationPointer", QString::number(reinterpret_cast<std::uintptr_t>(destination)) },
                { "DestinationSize", QString::number(destinationSize) },
            }
        );
    }
    catch (...) {
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to populate data buffer from variant map to raw buffer (memcpy)",
            "Unknown error",
            __FUNCTION__,
            {
                { "VariantMap", variantMap },
                { "DestinationPointer", QString::number(reinterpret_cast<std::uintptr_t>(destination)) },
                { "DestinationSize", QString::number(destinationSize) },
            }
        );
    }
}

void variantMapMustContain(const QVariantMap& variantMap, const QString& key)
{
    if (!variantMap.contains(key)) {
        throw ManiVaultException(
            SeverityLevel::Error,
            "Variant map is missing required key",
            QString("Variant map is missing required key '%1'").arg(key),
            __FUNCTION__,
            {
                { "VariantMap", variantMap },
                { "MissingKey", key }
            }
        );
    }
}

QVariant storeQVariant(const QVariant& variant)
{
    
    if (variant.isNull() || !variant.isValid())
        return variant;

    // check how many bytes it takes to store the variant
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    stream << variant;

    if (byteArray.size() < 10485760)
    {
        return variant;
    }
    // store large variant in one or more binary files similar to raw data
    const quint32 version = 1;
    QVariantMap result = rawDataToVariantMap(byteArray.data(), byteArray.size());
    Q_ASSERT(!result.contains("QVariantOnDiskVersion"));
    result["QVariantOnDiskVersion"] = version; // add an extra item to the result so we can check how the data was stored
    return result;
}

QVariant loadQVariant(const QVariant& variant)
{
    // check if variant contains a non-empty QVariantMap
    QVariantMap variantMap = variant.toMap();
    if (variantMap.empty())
        return variant;

    // check if it was stored on disk, if not just return it
    if (!variantMap.contains("QVariantOnDiskVersion"))
    {
        return variantMap;
    }

    // load the QVariant from disk
    quint32 version = variantMap["QVariantOnDiskVersion"].toUInt();
    Q_ASSERT(version == 1);


    // First check the size we need for a temporary buffer
    variantMapMustContain(variantMap, "Blocks");
    const auto blocks = variantMap["Blocks"].toList();
    uint64_t totalSize = 0;
    for (const auto& block : blocks)
    {
        // Get block variant map
        const auto map = block.toMap();
        variantMapMustContain(map, "Size");
        totalSize += map["Size"].value<uint64_t>();
    }
    // Next create a temporary buffer and load the data
    //std::vector<char> bytes(totalSize);
    //populateDataBufferFromVariantMap(variantMap, bytes.data());

    // Finally convert the data to a QVariant.
    QVariant result;;
    //QByteArray byteArray(bytes.data(), bytes.size());
    //QDataStream stream(&byteArray, QIODevice::ReadOnly);
    //stream >> result;
    return result;
}

QVariantMap storeOnDisk(const QStringList& list)
{
    QByteArray byteArray;
    QDataStream dataStream(&byteArray, QIODevice::WriteOnly);
    dataStream << list;
    QVariantMap variantMap = rawDataToVariantMap((const char*)byteArray.data(), byteArray.size());
    return variantMap;
}

QVariantMap storeOnDisk(const QVector<uint32_t>& vec)
{
    QByteArray byteArray;
    QDataStream dataStream(&byteArray, QIODevice::WriteOnly);
    dataStream << vec;
    QVariantMap variantMap = rawDataToVariantMap((const char*)byteArray.data(), byteArray.size());
    return variantMap;
}

void loadFromDisk(const QVariantMap& variantMap, QStringList& list)
{
    //std::vector<char> bytes(variantMap["Size"].value<uint64_t>());
    //populateDataBufferFromVariantMap(variantMap, (char*)bytes.data());
    //QByteArray byteArray(bytes.data(), bytes.size());
    //QDataStream dataStream(byteArray);
    //dataStream >> list;
}

void loadFromDisk(const QVariantMap& variantMap, QVector<uint32_t>& vec)
{
    //std::vector<char> bytes(variantMap["Size"].value<uint64_t>());
    //populateDataBufferFromVariantMap(variantMap, (char*)bytes.data());
    //QByteArray byteArray(bytes.data(), bytes.size());
    //QDataStream dataStream(byteArray);
    //dataStream >> vec;
}

QVariantMap loadJsonToVariantMap(const QString& filePath)
{
	QFile file(filePath);

	if (!file.open(QIODevice::ReadOnly)) {
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to open JSON file",
            QString("Unable to open file at path '%1'").arg(filePath),
            __FUNCTION__,
            {
                { "FilePath", filePath }
            }
        );
	}

	const auto data = file.readAll();

	QJsonParseError parseError;

	const auto doc = QJsonDocument::fromJson(data, &parseError);

	if (parseError.error != QJsonParseError::NoError) {
        throw ManiVaultException(
            SeverityLevel::Error,
            "JSON parse error",
            QString("JSON parse error: %1").arg(parseError.errorString()),
            __FUNCTION__,
            {
                { "FilePath", filePath }
            }
        );
	}

	if (!doc.isObject()) {
        throw ManiVaultException(
            SeverityLevel::Error,
            "JSON root is not an object",
            QString("JSON root is not an object in file '%1'").arg(filePath),
            __FUNCTION__,
            {
                { "FilePath", filePath }
            }
        );
	}

	return doc.object().toVariantMap();
}

bool isVariantMapRawBlockObject(const QVariantMap& map)
{
	return map.contains("BlockSize") &&
		map.contains("Blocks") &&
		map.contains("Codec") &&
		map.contains("NumberOfBlocks") &&
		map.contains("Size");
}

QVariantMap findRawBlockObject(const QVariant& value)
{
	if (value.canConvert<QVariantMap>()) {
		const QVariantMap map = value.toMap();

		if (isVariantMapRawBlockObject(map))
			return map;

		for (auto it = map.begin(); it != map.end(); ++it) {
			const QVariantMap found = findRawBlockObject(it.value());

			if (!found.isEmpty())
				return found;
		}
	}

	if (value.canConvert<QVariantList>()) {
		const QVariantList list = value.toList();

		for (const QVariant& item : list) {
			const QVariantMap found = findRawBlockObject(item);

			if (!found.isEmpty())
				return found;
		}
	}

	return {};
}

std::uint64_t getRawBlockObjectSize(const QVariantMap& map)
{
	bool       ok   = false;
	const auto size = map.value("Size").toULongLong(&ok);

	return ok ? size : 0;
}

std::uint64_t estimateRawBlockTotalSize(const QVariant& value)
{
	std::uint64_t totalSize = 0;

	if (value.canConvert<QVariantMap>()) {
		const QVariantMap map = value.toMap();

		if (isVariantMapRawBlockObject(map)) {
            totalSize += getRawBlockObjectSize(map);
		}

		for (auto it = map.begin(); it != map.end(); ++it) {
			totalSize += estimateRawBlockTotalSize(it.value());
		}
	}

	if (value.canConvert<QVariantList>()) {
		const QVariantList list = value.toList();

		for (const QVariant& item : list) {
			totalSize += estimateRawBlockTotalSize(item);
		}
	}

	return totalSize;
}

QByteArray serializeVariantMap(const QVariantMap& map)
{
	QByteArray  bytes;
	QDataStream out(&bytes, QIODevice::WriteOnly);

	out.setVersion(QDataStream::Qt_6_8); // match your Qt version if desired
	out << map;

	return bytes;
}

QVariantMap deserializeVariantMap(const QByteArray& bytes)
{
	QVariantMap map;

	QDataStream in(const_cast<QByteArray*>(&bytes), QIODevice::ReadOnly);

	in.setVersion(QDataStream::Qt_6_8);
	in >> map;

	return map;
}

QMetaType::Type getVariantListHomogenousType(const QVariantList& list)
{
    if (list.isEmpty())
        return QMetaType::UnknownType;

    auto classifyOne = [](const QVariant& value) -> QMetaType::Type {
        switch (value.metaType().id()) {
	        case QMetaType::Int:
	        case QMetaType::UInt:
	        case QMetaType::LongLong:
	        case QMetaType::ULongLong:
	        case QMetaType::Double:
	        case QMetaType::Bool:
	        case QMetaType::QString:
                return static_cast<QMetaType::Type>(value.metaType().id());
                
	        case QMetaType::QVariantMap:
                return QMetaType::QVariantMap;

            case QMetaType::QByteArray:
                return QMetaType::UnknownType;

	        default:
                return QMetaType::UnknownType;
        }
    };

    const auto firstKind = classifyOne(list.front());

    for (const QVariant& item : list) {
        if (classifyOne(item) != firstKind)
            return QMetaType::UnknownType;
    }

    return firstKind;
}

QVariantMap saveOptimizedVariantMap(const QVariantMap& source)
{
	QVariantMap target;
	//target.reserve(source.size());

	for (auto it = source.cbegin(); it != source.cend(); ++it)
		target.insert(it.key(), saveOptimizedVariant(it.value()));

	return target;
}

QVariant saveOptimizedVariant(const QVariant& source)
{
	if (source.metaType().id() == QMetaType::QVariantMap)
		return saveOptimizedVariantMap(source.toMap());

	if (source.metaType().id() == QMetaType::QVariantList)
		return saveOptimizedVariantList(source.toList());

	return source;
}

template<typename T>
QVariant saveTypedVector(const QVariantList& list, const QString& typeName)
{
    QVector<T> values;
    values.reserve(list.size());

    for (const QVariant& item : list)
        values.append(item.value<T>());

    QVariantMap raw;

    raw["__OptimizedVariantList"] = true;
    raw["Type"] = typeName;
    raw["Count"] = static_cast<qulonglong>(values.size());

    raw["Data"] = rawDataToVariantMap(
        reinterpret_cast<const char*>(values.constData()),
        static_cast<std::uint64_t>(values.size() * sizeof(T))
    );

    return raw;
}

QVariant saveBoolVector(const QVariantList& list)
{
    QByteArray bytes;
    bytes.resize(list.size());

    for (qsizetype i = 0; i < list.size(); ++i)
        bytes[i] = list[i].toBool() ? char(1) : char(0);

    QVariantMap raw;

    raw["__OptimizedVariantList"] = true;
    raw["Type"] = "BoolArray";
    raw["Count"] = static_cast<qulonglong>(list.size());

    raw["Data"] = rawDataToVariantMap(
        bytes.constData(),
        static_cast<std::uint64_t>(bytes.size())
    );

    return raw;
}

QVariant saveStringList(const QVariantList& list)
{
    QStringList strings;
    strings.reserve(list.size());

    for (const QVariant& item : list)
        strings.append(item.toString());

    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_6_8);
    stream << strings;

    QVariantMap block;
    block["__OptimizedVariantList"] = true;
    block["Type"] = "QStringList";
    block["Count"] = static_cast<qulonglong>(strings.size());
    block["Data"] = rawDataToVariantMap(
        bytes.constData(),
        static_cast<std::uint64_t>(bytes.size())
    );

    QDir dir(QDir::cleanPath(projects().getTemporaryDirPath(AbstractProjectManager::TemporaryDirType::Save)));

    QStringList entries = dir.entryList(
        QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot
    );

    return block;
}

QVariant saveOptimizedVariantList(const QVariantList& list)
{
	if (list.isEmpty())
		return list;

    constexpr qsizetype MinimumOptimizedListSize = 1024;

    if (list.size() < MinimumOptimizedListSize) {
        QVariantList target;
        target.reserve(list.size());

        for (const QVariant& item : list)
            target.append(saveOptimizedVariant(item));

        return target;
    }

	const auto homogeneousType = getVariantListHomogenousType(list);

	if (homogeneousType == QMetaType::UnknownType) {
		QVariantList target;
		target.reserve(list.size());

		for (const QVariant& item : list)
			target.append(saveOptimizedVariant(item));

		return target;
	}

	if (homogeneousType == QMetaType::QVariantMap) {
		QVariantList target;
		target.reserve(list.size());

		for (const QVariant& item : list)
			target.append(saveOptimizedVariantMap(item.toMap()));

		return target;
	}

	if (homogeneousType == QMetaType::QString)
        return saveStringList(list);

	if (homogeneousType == QMetaType::Int)
		return saveTypedVector<int>(list, "Int32Array");

	if (homogeneousType == QMetaType::UInt)
		return saveTypedVector<uint>(list, "UInt32Array");

	if (homogeneousType == QMetaType::LongLong)
		return saveTypedVector<qlonglong>(list, "Int64Array");

	if (homogeneousType == QMetaType::ULongLong)
		return saveTypedVector<qulonglong>(list, "UInt64Array");

	if (homogeneousType == QMetaType::Float)
		return saveTypedVector<float>(list, "Float32Array");

	if (homogeneousType == QMetaType::Double)
		return saveTypedVector<double>(list, "Float64Array");

	if (homogeneousType == QMetaType::Bool)
		return saveBoolVector(list);

	QVariantList target;
	target.reserve(list.size());

	for (const QVariant& item : list)
		target.append(saveOptimizedVariant(item));

	return target;
}

QVariantMap loadOptimizedVariantMap(const QVariantMap& source)
{
	QVariantMap target;

	for (auto it = source.cbegin(); it != source.cend(); ++it)
		target.insert(it.key(), loadOptimizedVariant(it.value()));

	return target;
}

QVariant loadOptimizedVariant(const QVariant& source)
{
	if (source.metaType().id() == QMetaType::QVariantMap) {
		const QVariantMap map = source.toMap();

		if (map.value("__OptimizedVariantList").toBool())
			return loadOptimizedVariantList(map);

		return loadOptimizedVariantMap(map);
	}

	if (source.metaType().id() == QMetaType::QVariantList) {
		const QVariantList list = source.toList();

		QVariantList target;
		target.reserve(list.size());

		for (const QVariant& item : list)
			target.append(loadOptimizedVariant(item));

		return target;
	}

	return source;
}

template<typename T>
QVariantList loadTypedList(const QByteArray& bytes, qsizetype count)
{
    const qsizetype expectedBytes = count * qsizetype(sizeof(T));

    if (bytes.size() != expectedBytes)
        throw ManiVaultException(
            SeverityLevel::Error,
            "Invalid optimized QVariantList byte size",
            QString("Invalid optimized QVariantList byte size: expected %1, got %2").arg(expectedBytes).arg(bytes.size()),
            __FUNCTION__,
            {
                { "ExpectedBytes", QString::number(expectedBytes) },
                { "ActualBytes", QString::number(bytes.size()) }
            }
        );

    const auto* src = reinterpret_cast<const T*>(bytes.constData());

    QVariantList list;
    list.reserve(count);

    for (qsizetype i = 0; i < count; ++i)
        list.append(QVariant::fromValue(src[i]));

    return list;
}

QVariantList loadBoolList(const QByteArray& bytes, qsizetype count)
{
    if (bytes.size() != count)
        throw ManiVaultException(
            SeverityLevel::Error,
            "Invalid optimized bool QVariantList byte size",
            QString("Invalid optimized bool QVariantList byte size: expected %1, got %2").arg(count).arg(bytes.size()),
            __FUNCTION__,
            {
                { "ExpectedCount", QString::number(count) },
                { "ActualCount", QString::number(bytes.size()) }
            }
        );

    QVariantList list;
    list.reserve(count);

    for (qsizetype i = 0; i < count; ++i)
        list.append(bytes[i] != char(0));

    return list;
}

QVariant loadStringList(const QVariantMap& block)
{
    if (!block.value("__OptimizedVariantList").toBool())
        return block;

    if (block.value("Type").toString() != "QStringList")
        return block;

    const auto expectedCount =
        static_cast<qsizetype>(block.value("Count").toULongLong());

    const QVariantMap dataMap = block.value("Data").toMap();
    QByteArray bytes = populateDataBufferFromVariantMapSync(dataMap);

	QDataStream stream(bytes);
    stream.setVersion(QDataStream::Qt_6_8);

    QStringList strings;
    stream >> strings;

    if (stream.status() != QDataStream::Ok)
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to load optimized QStringList block",
            "Failed to load optimized QStringList block",
            __FUNCTION__,
            {
                { "Block", block }
            }
        );

    if (strings.size() != expectedCount)
        throw ManiVaultException(
            SeverityLevel::Error,
            "Optimized QStringList count mismatch",
            QString("Optimized QStringList count mismatch: expected %1, got %2").arg(expectedCount).arg(strings.size()),
            __FUNCTION__,
            {
                { "Block", block }
            }
        );

    QVariantList list;
    list.reserve(strings.size());

    for (const QString& string : strings)
        list.append(string);

    return list;
}

QVariant loadOptimizedVariantList(const QVariantMap& map)
{
    const QString type = map.value("Type").toString();

    if (type == "QStringList")
        return loadStringList(map);

    const auto count =
        static_cast<qsizetype>(map.value("Count").toULongLong());

    const QVariantMap dataMap = map.value("Data").toMap();

    QByteArray bytes = populateDataBufferFromVariantMapSync(dataMap);

    if (type == "BoolArray")
        return loadBoolList(bytes, count);

    if (type == "Int32Array")
        return loadTypedList<int>(bytes, count);

    if (type == "UInt32Array")
        return loadTypedList<uint>(bytes, count);

    if (type == "Int64Array")
        return loadTypedList<qlonglong>(bytes, count);

    if (type == "UInt64Array")
        return loadTypedList<qulonglong>(bytes, count);

    if (type == "Float32Array")
        return loadTypedList<float>(bytes, count);

    if (type == "Float64Array")
        return loadTypedList<double>(bytes, count);

    return map;
}

}
