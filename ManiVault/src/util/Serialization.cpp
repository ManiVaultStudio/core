// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Serialization.h"
#include "CoreInterface.h"
#include "CodecRegistry.h"

#include "workflow/WorkflowRuntimeScoped.h"

#include "Exception/SerializationException.h"

#include <QUuid>

#include <exception>

#include <math.h>
#include <limits>

using namespace mv::workflow;

namespace mv::util {

namespace
{

    EncodeBlockResult encodeBlock(const EncodeBlockJob& job, const QString& saveDir)
    {
        EncodeBlockResult result;

        try {
            if (job._data == nullptr)
                throw std::invalid_argument("EncodeBlockJob data pointer is null");

            if (job._size > static_cast<std::uint64_t>(std::numeric_limits<qsizetype>::max()))
                throw std::overflow_error("EncodeBlockJob block size exceeds qsizetype range");

            QVariantMap blockVariantMap;

            blockVariantMap["Offset"] = QVariant::fromValue(job._offset);
            blockVariantMap["Size"] = QVariant::fromValue(job._size);

            const auto fileName = QUuid::createUuid().toString(QUuid::WithoutBraces) + job._codec->getFileExtension();
            const auto filePath = QDir::cleanPath(saveDir + QDir::separator() + fileName);

            std::uint64_t numberOfEncodedBytes = 0;

            job._codec->encodeToFile(job._data + job._offset, static_cast<qsizetype>(job._size), filePath, &numberOfEncodedBytes);

            blockVariantMap["CompressedSize"] = QVariant::fromValue<std::uint64_t>(numberOfEncodedBytes);
            blockVariantMap["URI"] = fileName;

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
                "Unable to encode block",
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

    struct ActiveDecodeKey
    {
        QString        _uri;
        const char* _destinationBase = nullptr;
        std::uint64_t  _offset = 0;
        std::uint64_t  _size = 0;

        bool operator==(const ActiveDecodeKey& other) const
        {
            return _uri == other._uri &&
                _destinationBase == other._destinationBase &&
                _offset == other._offset &&
                _size == other._size;
        }
    };

    QString makeActiveDecodeKey(
        const QString& uri,
        const char* destinationBase,
        std::uint64_t offset,
        std::uint64_t size)
    {
        return QString("%1|%2|%3|%4")
            .arg(uri)
            .arg(reinterpret_cast<quintptr>(destinationBase))
            .arg(offset)
            .arg(size);
    }

    QMutex g_activeDecodeMutex;
    QSet<QString> g_activeDecodes;

    class ActiveDecodeGuard
    {
    public:
        ActiveDecodeGuard(
            const QString& uri,
            const char* destinationBase,
            std::uint64_t offset,
            std::uint64_t size) :
            _uri(uri),
            _destinationBase(destinationBase),
            _offset(offset),
            _size(size),
            _key(makeActiveDecodeKey(uri, destinationBase, offset, size))
        {
            QMutexLocker lock(&g_activeDecodeMutex);

            if (g_activeDecodes.contains(_key)) {
                qWarning()
                    << "Duplicate concurrent decode detected:"
                    << "uri =" << _uri
                    << "destination =" << static_cast<const void*>(_destinationBase)
                    << "offset =" << _offset
                    << "size =" << _size;
            }

            g_activeDecodes.insert(_key);
        }

        ~ActiveDecodeGuard()
        {
            QMutexLocker lock(&g_activeDecodeMutex);
            g_activeDecodes.remove(_key);
        }

    private:
        QString       _uri;
        const char* _destinationBase = nullptr;
        std::uint64_t _offset = 0;
        std::uint64_t _size = 0;
        QString       _key;
    };


    DecodeBlockJobs makeDecodeBlockJobs(const QVariantMap& variantMap)
    {
        variantMapMustContain(variantMap, "BlockSize");
        variantMapMustContain(variantMap, "Blocks");
        variantMapMustContain(variantMap, "Size");

        const auto blocks = variantMap.value("Blocks").toList();
        const bool hasCodec = variantMap.contains("Codec");
        const auto codecName = hasCodec ? variantMap.value("Codec").toString() : QStringLiteral("none");
        const auto totalSize = variantMap.value("Size").toULongLong();

        if (totalSize == 0)
            throw ManiVaultException(SeverityLevel::Error, "Failed to populate data buffer from variant map", "Decoded buffer size is zero", __FUNCTION__);

        if (hasCodec && !codecRegistry().isRegistered(codecName))
            throw ManiVaultException(SeverityLevel::Error, "Failed to populate data buffer from variant map", QString("Unable to load raw data, codec %1 is not registered").arg(codecName), __FUNCTION__);

        auto createCodec = [hasCodec, codecName]() -> std::shared_ptr<BlobCodec> {
            return hasCodec
                ? codecRegistry().createCodec(nullptr, codecName)
                : codecRegistry().createCodec(nullptr, BlobCodec::Type::None);
            };

        DecodeBlockJobs jobs;
        jobs.reserve(blocks.size());

        for (const auto& block : blocks) {
            const auto blockMap = block.toMap();

            DecodeBlockJob job;
            job._offset = blockMap.value("Offset").value<quint64>();
            job._size = blockMap.value("Size").value<quint64>();
            job._compressedSize = blockMap.value("CompressedSize", 0).value<quint64>();
            job._codec = createCodec();
            job._uri = blockMap.value("URI").toString();
            job._encodedData = blockMap.value("Data").toString();

            jobs.push_back(std::move(job));
        }

        // Overlap check
        std::sort(jobs.begin(), jobs.end(),
            [](const DecodeBlockJob& a, const DecodeBlockJob& b) {
                return a._offset < b._offset;
            });

        quint64 expectedOffset = 0;

        for (int i = 0; i < jobs.size(); ++i) {
            const auto& job = jobs[i];
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

        return jobs;
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
        bool       ok = false;
        const auto size = map.value("Size").toULongLong(&ok);

        return ok ? size : 0;
    }
}

QVariantMap bytesToBlobVariantMap(const char* bytes, const std::uint64_t& numberOfBytes, const SharedWorkflowExecutionContext& executionContext /*= nullptr*/)
{
    try {
        WorkflowExecutionOptions resolvedOptions = executionContext ? executionContext->getExecutionOptions() : WorkflowExecutionOptions();

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

        rawData["Size"] = QVariant::fromValue(numberOfBytes);
        rawData["Codec"] = QVariant::fromValue(createCodec()->getName());

        const auto numberOfBlocks = (numberOfBytes + maxBlockSizeInBytes - 1) / maxBlockSizeInBytes;

        EncodeBlockJobs encodeBlockJobs;

        encodeBlockJobs.reserve(static_cast<int>(numberOfBlocks));

        std::uint64_t offset = 0;

        while (offset < numberOfBytes)
        {
            const auto blockSize = std::min(maxBlockSizeInBytes, numberOfBytes - offset);

            EncodeBlockJob job;

            job._data = bytes;
            job._offset = offset;
            job._size = blockSize;
            job._codec = createCodec();

            encodeBlockJobs.push_back(std::move(job));

            offset += blockSize;
        }

        const auto saveDir = QDir::cleanPath(projects().getTemporaryDirPath(AbstractProjectManager::TemporaryDirType::Save));

        UniqueWorkflowPlan encodeWorkflowPlan = std::make_unique<WorkflowPlan>("Encode Blocks");
        WorkflowPlan::Jobs encodeJobs;

        std::int32_t encodeBlockJobIndex = 0;

        for (auto& encodeBlockJob : encodeBlockJobs) {
            auto* encodeBlockJobPtr = &encodeBlockJob;

            encodeJobs.emplace_back(QString("Encode Block %1").arg(QString::number(encodeBlockJobIndex)), [encodeBlockJobPtr, saveDir](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& context) {
                encodeBlockJobPtr->_result = encodeBlock(*encodeBlockJobPtr, saveDir);
                });

            ++encodeBlockJobIndex;
        }

        if (!encodeJobs.empty()) {
            encodeWorkflowPlan->addBatchedParallelStage("Encode Blocks", encodeJobs, resolvedOptions._workflowBatchingOptions._dataBlockEncodingBatchSize);

            auto future = WorkflowRuntimeScoped::executeBlocking(std::move(encodeWorkflowPlan), executionContext);

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

            rawData["NumberOfBlocks"] = QVariant::fromValue(numberOfBlocks);
            rawData["BlockSize"] = QVariant::fromValue(maxBlockSizeInBytes);
            rawData["Blocks"] = QVariant::fromValue(blocks);
        }

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

DecodeBlockResult decodeBlockFromFileTo(const DecodeBlockJob& decodeBlockJob, char* destination, std::uint64_t destinationSize)
{
    DecodeBlockResult result;

    try {
        result._offset = decodeBlockJob._offset;
        result._size = decodeBlockJob._size;

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

        const auto offset = static_cast<std::uint64_t>(decodeBlockJob._offset);
        const auto size = static_cast<std::uint64_t>(decodeBlockJob._size);

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

        auto codec = decodeBlockJob._codec;

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

        ActiveDecodeGuard activeDecodeGuard({ decodeBlockJob._uri, destination, offset, size });

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
    catch (...) {

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

DecodeBlockResult decodeBlockFromBase64To(const DecodeBlockJob& decodeBlockJob, char* destination, std::uint64_t destinationSize)
{
    DecodeBlockResult result;

    try {
        result._offset = decodeBlockJob._offset;
        result._size = decodeBlockJob._size;

        if (!destination)
            throw ManiVaultException(SeverityLevel::Error,
                "Unable to decode block from base64 to buffer",
                "Destination buffer is null",
                __FUNCTION__,
                {
                    { "URI", decodeBlockJob._uri }
                });

        const auto offset = static_cast<std::uint64_t>(decodeBlockJob._offset);
        const auto size = static_cast<std::uint64_t>(decodeBlockJob._size);

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

        auto codec = decodeBlockJob._codec;

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

UniqueWorkflowPlan populateBytesFromBlobMapWorkflow(QVariantMap variantMap, char* destination, std::uint64_t destinationSize, const SharedWorkflowExecutionContext& executionContext /*= nullptr*/)
{
    if (variantMap.isEmpty()) {
        executionContext->warning("Failed to populate data buffer from variant map, variant map is empty");

        return std::make_unique<WorkflowPlan>("Decode Blocks (Empty Variant Map)");
    }

    auto resolvedOptions = executionContext ? executionContext->getExecutionOptions() : WorkflowExecutionOptions{};

    auto decodeBlockJobs = makeDecodeBlockJobs(variantMap);

    auto plan = std::make_unique<WorkflowPlan>("Decode Blocks");

    WorkflowPlan::Jobs jobs;
    std::int32_t jobIndex = 0;

    for (const auto& decodeBlockJob : decodeBlockJobs) {
        jobs.emplace_back(
            QString("Decode Block %1").arg(jobIndex++),
            [decodeBlockJob, destination, destinationSize](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
                if (decodeBlockJob._uri.isEmpty())
                    decodeBlockFromBase64To(decodeBlockJob, destination, destinationSize);
                else
                    decodeBlockFromFileTo(decodeBlockJob, destination, destinationSize);
            });
    }

    plan->addBatchedParallelStage(
        "Decode Blocks",
        std::move(jobs),
        resolvedOptions._workflowBatchingOptions._dataBlockDecodingBatchSize);

    return plan;
}

void populateBytesFromBlobMap(QVariantMap variantMap, char* destination, std::uint64_t destinationSize, const SharedWorkflowExecutionContext& executionContext)
{
    if (variantMap.isEmpty()) {
        executionContext->warning("Failed to populate data buffer from variant map, variant map is empty");
        return;
    }

    if (!destination)
        throw std::invalid_argument("populateBytesFromBlobMap destination is null");

    if (destinationSize == 0)
        return;

    auto jobs = makeDecodeBlockJobs(variantMap);

    for (const auto& job : jobs) {
        if (job._offset + job._size > destinationSize)
            throw std::runtime_error("Decode block range exceeds destination buffer");

        if (job._uri.isEmpty())
            decodeBlockFromBase64To(job, destination, destinationSize);
        else
            decodeBlockFromFileTo(job, destination, destinationSize);
    }
}

QByteArray bytesFromBlobVariantMap(QVariantMap variantMap, const SharedWorkflowExecutionContext& executionContext /*= nullptr*/)
{
    if (!executionContext)
        throw ManiVaultException(SeverityLevel::Warning, "Parent context is null", "Please provide a valid parent context to avoid detached parallel work", __FUNCTION__, variantMap);

    if (variantMap.isEmpty()) {
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to populate data buffer from variant map",
            "Variant map is empty",
            __FUNCTION__,
            {
                { "VariantMap", variantMap },
            }
            );
    }

    variantMapMustContain(variantMap, "BlockSize");
    variantMapMustContain(variantMap, "Blocks");
    variantMapMustContain(variantMap, "Size");

    const auto blocks = variantMap.value("Blocks").toList();
    const bool hasCodec = variantMap.contains("Codec");
    const auto codecName = hasCodec ? variantMap.value("Codec").toString() : QStringLiteral("none");
    const auto totalSize = variantMap.value("Size").toULongLong();

    if (totalSize == 0) {
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to populate data buffer from variant map",
            "Decoded buffer size is zero",
            __FUNCTION__,
            {
                { "VariantMap", variantMap },
            }
            );
    }

    QByteArray bytes;

    bytes.resize(static_cast<qsizetype>(totalSize));

    populateBytesFromBlobMap(variantMap, bytes.data(), static_cast<std::uint64_t>(bytes.size()), executionContext);

    return bytes;
}

void variantMapMustContain(const QVariantMap& variantMap, const QString& key)
{
    if (variantMap.contains(key))
        return;

    if (settings().getMiscellaneousSettings().getIgnoreLoadingErrorsAction().isChecked()) {
        throw ManiVaultException(SeverityLevel::Warning, "Missing key in QVariantMap", QString("Variant map is missing key '%1', but loading errors are set to be ignored").arg(key), __FUNCTION__, {
            { "Key", key },
            { "VariantMap", describeVariantMapKeys(variantMap) }
            });
    }

    throw ManiVaultException(SeverityLevel::Error, "Missing key in QVariantMap", QString("Variant map is missing required key '%1'").arg(key),
        __FUNCTION__,
        {
            { "Key", key },
            { "VariantMap", describeVariantMapKeys(variantMap) }
        }
    );
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

}
