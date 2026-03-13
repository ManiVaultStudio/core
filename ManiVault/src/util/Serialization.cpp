// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Serialization.h"
#include "CoreInterface.h"
#include "Application.h"

#include <QUuid>
#include <QtConcurrent>

#include <exception>

#include <math.h>

#include "BlobCodec.h"

namespace mv::util {

struct EncodeBlockJob
{
    std::uint64_t _offset = 0;
    std::uint64_t _size = 0;
    QByteArray _rawData;
};

struct EncodeBlockResult
{
    std::uint64_t _offset = 0;
    std::uint64_t _size = 0;
    QByteArray _encodedData;
    QString _error;
};

struct DecodeBlockJob
{
    std::uint64_t _offset = 0;
    std::uint64_t _size = 0;
    QByteArray _encodedData;
};

struct DecodeBlockResult
{
    std::uint64_t _offset = 0;
    std::uint64_t _size = 0;
    QByteArray _decodedData;
    QString _error;
};

void saveRawDataToBinaryFile(const char* bytes, const std::uint64_t& numberOfBytes, const QString& filePath)
{
    // Exit prematurely if the serialization process was aborted
    if (Application::isSerializationAborted())
        return;

    // Output directory
    const auto outputDirectory = QFileInfo(filePath).dir();

    // Exit prematurely if the target directory does not exist
    if (!QFileInfo(filePath).dir().exists())
        throw std::runtime_error(QString("Unable to save data in %1, the directory does not exist").arg(outputDirectory.dirName()).toLatin1());

    // Create binary file
    QFile binaryFile(filePath);

    // And save it do disk
    binaryFile.open(QIODevice::WriteOnly);
    binaryFile.write(QByteArray::fromRawData(bytes, numberOfBytes)); // usng QByteArray::fromRawData(const char *data, qsizetype size) so that the bytes are not copied.
    binaryFile.close();
}

void loadRawDataFromBinaryFile(char* bytes, const std::uint64_t& numberOfBytes, const QString& filePath)
{
    // Exit prematurely if the serialization process was aborted
    if (Application::isSerializationAborted())
        return;

    // Exit prematurely if the target file does not exist
    if (!QFileInfo(filePath).exists())
        throw std::runtime_error(QString("Unable to load binary file, %1 does not exist").arg(filePath).toLatin1());

    // Create binary file
    QFile binaryFile(filePath);

    // Open the file read-only
    if (!binaryFile.open(QIODevice::ReadOnly))
        throw std::runtime_error("Unable to load binary file, cannot open file");

    // Read the raw data from the file directly into bytes
    const auto numberOfBytesRead = binaryFile.read(bytes, numberOfBytes);

    // Except if the number of bytes in the file deviates from the number of requested bytes
    if (numberOfBytesRead != numberOfBytes)
        throw std::runtime_error("Unable to load binary file, number of requested bytes is not the same as in the file");
}

QByteArray readBinaryFileToByteArray(const QString& filePath)
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
        throw std::runtime_error(QString("Unable to open file %1").arg(filePath).toStdString());

    return file.readAll();
}

QVariantMap rawDataToVariantMap(const char* bytes, const std::uint64_t& numberOfBytes, bool saveToDisk /*= false*/, std::uint64_t maxBlockSize /*= DEFAULT_MAX_BLOCK_SIZE*/, const BlobCodec* blobCodecOverride /*= nullptr*/)
{
    struct EncodeBlockJob
    {
        std::uint64_t _offset = 0;
        std::uint64_t _size = 0;
        QByteArray    _rawData;
    };

    struct EncodeBlockResult
    {
        QVariantMap _block;
        QString     _error;
    };

    const auto* selectedBlobCodec = blobCodecOverride ? blobCodecOverride : &mv::projects().getDefaultBlobCodec();

    Q_ASSERT(selectedBlobCodec != nullptr);
    Q_ASSERT(maxBlockSize != 0);

    if (maxBlockSize == static_cast<std::uint64_t>(-1))
        maxBlockSize = DEFAULT_MAX_BLOCK_SIZE;

    const QString codecName = selectedBlobCodec->getName();

    if (!BlobCodec::isRegistered(codecName)) {
        throw std::runtime_error(QString("Unable to save raw data, blob codec %1 is not registered").arg(codecName).toStdString());
    }

    auto createCodec = [codecName]() -> std::unique_ptr<BlobCodec> {
        return BlobCodec::create(codecName);
    };

    QVariantMap rawData;

    rawData["Size"]     = QVariant::fromValue(numberOfBytes);
    rawData["Codec"]    = QVariant::fromValue(codecName);

    const auto numberOfBlocks = static_cast<std::uint64_t>((numberOfBytes + maxBlockSize - 1) / maxBlockSize);

    QVector<EncodeBlockJob> jobs;
    jobs.reserve(static_cast<int>(numberOfBlocks));

    std::uint64_t offset = 0;

    while (offset < numberOfBytes)
    {
        const auto blockSize = std::min(maxBlockSize, numberOfBytes - offset);

        EncodeBlockJob job;
        job._offset     = offset;
        job._size       = blockSize;
        job._rawData    = QByteArray(bytes + offset, static_cast<qsizetype>(blockSize));

        jobs.push_back(std::move(job));

        offset += blockSize;
    }

    const auto saveDir = QDir::cleanPath(projects().getTemporaryDirPath(AbstractProjectManager::TemporaryDirType::Save));

    const auto results = QtConcurrent::blockingMapped<QVector<EncodeBlockResult>>(jobs, [createCodec, saveToDisk, saveDir](const EncodeBlockJob& job) -> EncodeBlockResult {
        EncodeBlockResult result;

        try {
            auto blobCodec = createCodec();

            QVariantMap block;

            block["Offset"]         = QVariant::fromValue(job._offset);
            block["Size"]           = QVariant::fromValue(job._size);

            if (saveToDisk) {
                const auto fileName     = QUuid::createUuid().toString(QUuid::WithoutBraces) + blobCodec->getFileExtension();
                const auto filePath     = QDir::cleanPath(saveDir + QDir::separator() + fileName);
                const auto encodeResult = blobCodec->encodeToFile(job._rawData, filePath);

                if (encodeResult.isSuccess()) {
                    block["CompressedSize"] = QVariant::fromValue<std::uint64_t>(static_cast<std::uint64_t>(encodeResult._data.size()));
                }
                else {
                    throw std::runtime_error(QString("Failed to encode block to file: %1").arg(filePath).toStdString());
                }

                block["URI"] = fileName;
            }
            else {
                block["Data"] = QString::fromUtf8(job._rawData.toBase64());
            }

            result._block = std::move(block);
        }
        catch (const std::exception& e) {
            result._error = QString::fromUtf8(e.what());
        }

        return result;
    });

    QVariantList blocks;
    blocks.reserve(results.size());

    for (const auto& result : results)
    {
        if (!result._error.isEmpty())
            throw std::runtime_error(result._error.toStdString());

        blocks.push_back(result._block);
    }

    rawData["NumberOfBlocks"]   = QVariant::fromValue(numberOfBlocks);
    rawData["BlockSize"]        = QVariant::fromValue(maxBlockSize);
    rawData["Blocks"]           = QVariant::fromValue(blocks);

    return rawData;
}

void populateDataBufferFromVariantMap(const QVariantMap& variantMap, char* bytes)
{
    struct BlockJob
    {
        quint64 offset = 0;
        quint64 size = 0;
        QString uri;
        QString data;
    };

    try {
        const auto blocks = variantMap["Blocks"].toList();

        // Keep enough information so each worker can create its own codec.
        const auto hasCodec     = variantMap.contains("Codec");
        const auto codecName    = hasCodec ? variantMap["Codec"].toString() : "none";

        if (hasCodec && !BlobCodec::isRegistered(codecName)) {
            throw std::runtime_error(QString("Unable to load raw data, blob codec %1 is not registered").arg(codecName).toStdString());
        }

        auto createCodec = [&]() -> std::unique_ptr<BlobCodec> {
            if (hasCodec)
                return BlobCodec::create(codecName);

            return BlobCodec::create(BlobCodec::Type::None);
        };

        variantMapMustContain(variantMap, "BlockSize");
        variantMapMustContain(variantMap, "Blocks");

        const auto blockSize = variantMap["BlockSize"].toInt();
        Q_UNUSED(blockSize);

        QVector<BlockJob> jobs;
        jobs.reserve(blocks.size());

        // Build and validate jobs on the main thread
        for (const auto& block : blocks) {
            const auto blockMap = block.toMap();

            variantMapMustContain(blockMap, "Offset");
            variantMapMustContain(blockMap, "Size");

            BlockJob job;

            job.offset  = blockMap["Offset"].value<quint64>();
            job.size    = blockMap["Size"].value<quint64>();

            if (blockMap.contains("URI"))
                job.uri = QDir::cleanPath(projects().getTemporaryDirPath(AbstractProjectManager::TemporaryDirType::Open) + QDir::separator() + blockMap["URI"].toString());

            if (blockMap.contains("Data"))
                job.data = blockMap["Data"].toString();

            jobs.push_back(std::move(job));
        }

        QMutex errorMutex;
        QString firstError;

        QtConcurrent::blockingMap(jobs, [&](BlockJob& job) {
            // Stop doing more work once an error was seen
            {
                QMutexLocker locker(&errorMutex);
                if (!firstError.isEmpty())
                    return;
            }

            try {
                auto blobCodec = createCodec();

                if (job.uri.isEmpty()) {
	                qDebug() << "Loading raw data from binary file for block with offset" << job.offset << "and size" << job.size;
                }

                const auto decodeResult = blobCodec->decodeFromFile(job.uri);

                if (decodeResult.isSuccess()) {
                    const auto& blockData = decodeResult._data;

                    if (static_cast<quint64>(blockData.size()) < job.size) {
                        throw std::runtime_error(QString("Decoded block is smaller than expected: got %1 bytes, expected %2 bytes").arg(blockData.size()).arg(job.size).toStdString());
                    }

                    memcpy(bytes + job.offset, blockData.constData(), static_cast<size_t>(job.size));
                } else {
                    throw std::runtime_error(QString("Failed to decode block from file: %1").arg(job.uri).toStdString());
                }
            }
            catch (const std::exception& e) {
                QMutexLocker locker(&errorMutex);

                if (firstError.isEmpty())
                    firstError = QString::fromUtf8(e.what());
            }
            });

        if (!firstError.isEmpty())
            throw std::runtime_error(firstError.toStdString());
    }
    catch (const std::exception& e) {
        qWarning() << "Unable to populate data from buffer:" << e.what();
    }
}

void variantMapMustContain(const QVariantMap& variantMap, const QString& key)
{
    if (!variantMap.contains(key))
        throw std::runtime_error(QString("%1 not found in map: %2").arg(key, variantMap.keys().join(", ")).toLatin1());
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
    QVariantMap result = rawDataToVariantMap(byteArray.data(), byteArray.size(), true);
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
    std::vector<char> bytes(totalSize);
    populateDataBufferFromVariantMap(variantMap, bytes.data());

    // Finally convert the data to a QVariant.
    QVariant result;;
    QByteArray byteArray(bytes.data(), bytes.size());
    QDataStream stream(&byteArray, QIODevice::ReadOnly);
    stream >> result;
    return result;
}

QVariantMap storeOnDisk(const QStringList& list)
{
    QByteArray byteArray;
    QDataStream dataStream(&byteArray, QIODevice::WriteOnly);
    dataStream << list;
    QVariantMap variantMap = rawDataToVariantMap((const char*)byteArray.data(), byteArray.size(), true);
    return variantMap;
}

QVariantMap storeOnDisk(const QVector<uint32_t>& vec)
{
    QByteArray byteArray;
    QDataStream dataStream(&byteArray, QIODevice::WriteOnly);
    dataStream << vec;
    QVariantMap variantMap = rawDataToVariantMap((const char*)byteArray.data(), byteArray.size(), true);
    return variantMap;
}

void loadFromDisk(const QVariantMap& variantMap, QStringList& list)
{
    std::vector<char> bytes(variantMap["Size"].value<uint64_t>());
    populateDataBufferFromVariantMap(variantMap, (char*)bytes.data());
    QByteArray byteArray(bytes.data(), bytes.size());
    QDataStream dataStream(byteArray);
    dataStream >> list;
}

void loadFromDisk(const QVariantMap& variantMap, QVector<uint32_t>& vec)
{
    std::vector<char> bytes(variantMap["Size"].value<uint64_t>());
    populateDataBufferFromVariantMap(variantMap, (char*)bytes.data());
    QByteArray byteArray(bytes.data(), bytes.size());
    QDataStream dataStream(byteArray);
    dataStream >> vec;
}
}
