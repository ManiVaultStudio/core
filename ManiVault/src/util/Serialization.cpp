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

QVariantMap rawDataToVariantMap(const char* bytes,
    const std::uint64_t& numberOfBytes,
    bool saveToDisk /*= false*/,
    std::uint64_t maxBlockSize /*= DEFAULT_MAX_BLOCK_SIZE*/,
    const BlobCodec* blobCodecOverride /*= nullptr*/)
{
    const auto* blobCodec = blobCodecOverride ? blobCodecOverride : &mv::projects().getDefaultBlobCodec();

    Q_ASSERT(blobCodec != nullptr);
    Q_ASSERT(maxBlockSize != 0);

    if (maxBlockSize == static_cast<std::uint64_t>(-1))
        maxBlockSize = DEFAULT_MAX_BLOCK_SIZE;

    QVariantMap rawData;

    rawData["Size"] = QVariant::fromValue(numberOfBytes);
    rawData["Codec"] = QVariant::fromValue(blobCodec->getName());

    const auto numberOfBlocks = static_cast<std::uint64_t>(
        ceilf(numberOfBytes / static_cast<float>(maxBlockSize)));

    QVector<EncodeBlockJob> jobs;
    jobs.reserve(static_cast<int>(numberOfBlocks));

    std::uint64_t offset = 0;

    while (offset < numberOfBytes)
    {
        const auto blockSize = std::min(maxBlockSize, numberOfBytes - offset);

        EncodeBlockJob job;
        job._offset = offset;
        job._size = blockSize;
        job._rawData = QByteArray(bytes + offset, static_cast<qsizetype>(blockSize));

        jobs.push_back(std::move(job));
        offset += blockSize;
    }

    const auto results = QtConcurrent::blockingMapped<QVector<EncodeBlockResult>>(
        jobs,
        [blobCodec](const EncodeBlockJob& job) -> EncodeBlockResult
        {
            EncodeBlockResult result;
            result._offset = job._offset;
            result._size = job._size;

            const auto encodedResult = blobCodec->encode(job._rawData);

            if (!encodedResult._success) {
                result._error = encodedResult._error;
                return result;
            }

            result._encodedData = encodedResult._data;
            return result;
        });

    QVariantList blocks;
    const auto fileExtension = blobCodec->getFileExtension();

    for (const auto& result : results)
    {
        if (!result._error.isEmpty())
            throw std::runtime_error(result._error.toStdString());

        QVariantMap block;
        block["Offset"] = QVariant::fromValue(result._offset);
        block["Size"] = QVariant::fromValue(result._size);
        block["StoredSize"] = QVariant::fromValue<std::uint64_t>(
            static_cast<std::uint64_t>(result._encodedData.size()));

        if (saveToDisk) {
            const auto fileName = QUuid::createUuid().toString(QUuid::WithoutBraces) + fileExtension;
            const auto filePath = QDir::cleanPath(
                projects().getTemporaryDirPath(AbstractProjectManager::TemporaryDirType::Save)
                + QDir::separator()
                + fileName
            );

            // Save the raw data to binary file
            //saveRawDataToBinaryFile(&bytes[offset], blockSize, filePath);

            if (!blobCodec->encodeToFile(QByteArray::fromRawData(&bytes[offset], blockSize), filePath).isSuccess())
                throw std::runtime_error(QString("Unable to encode block to file: %1").arg(filePath).toLatin1());

            block["URI"] = fileName;
        }
        else {
            block["Data"] = QString::fromUtf8(result._encodedData.toBase64());
        }

        blocks.push_back(block);
    }

    rawData["NumberOfBlocks"] = QVariant::fromValue(numberOfBlocks);
    rawData["BlockSize"] = QVariant::fromValue(maxBlockSize);
    rawData["Blocks"] = QVariant::fromValue(blocks);

    return rawData;
}

void populateDataBufferFromVariantMap(const QVariantMap& variantMap, char* bytes)
{
    std::unique_ptr<BlobCodec> blobCodec;

    if (variantMap.contains("Codec")) {
        const auto codecName = variantMap["Codec"].toString();

        if (!BlobCodec::isRegistered(codecName))
            throw std::runtime_error(
                QString("Unable to load raw data, blob codec %1 is not registered")
                .arg(codecName)
                .toStdString()
            );

        blobCodec = BlobCodec::create(codecName);
    }
    else {
        blobCodec = BlobCodec::create(BlobCodec::Type::None);
    }

    variantMapMustContain(variantMap, "Blocks");

    const auto blocks = variantMap["Blocks"].toList();

    QVector<DecodeBlockJob> jobs;
    jobs.reserve(blocks.size());

    for (const auto& block : blocks) {
        const auto map = block.toMap();

        variantMapMustContain(map, "Offset");
        variantMapMustContain(map, "Size");

        const auto offset = map["Offset"].value<std::uint64_t>();
        const auto size = map["Size"].value<std::uint64_t>();

        DecodeBlockJob job;
        job._offset = offset;
        job._size = size;

        if (map.contains("URI")) {
            const auto filePath = QDir::cleanPath(
                projects().getTemporaryDirPath(AbstractProjectManager::TemporaryDirType::Open)
                + QDir::separator()
                + map["URI"].toString()
            );

            job._encodedData = readBinaryFileToByteArray(filePath);
        }
        else if (map.contains("Data")) {
            job._encodedData = QByteArray::fromBase64(map["Data"].toString().toUtf8());
        }
        else {
            throw std::runtime_error("Raw data block contains neither URI nor Data");
        }

        jobs.push_back(std::move(job));
    }

    const auto results = QtConcurrent::blockingMapped<QVector<DecodeBlockResult>>(
        jobs,
        [codecName = variantMap.contains("Codec") ? variantMap["Codec"].toString() : QString()](const DecodeBlockJob& job) -> DecodeBlockResult
        {
            DecodeBlockResult result;
            result._offset = job._offset;
            result._size = job._size;

            std::unique_ptr<BlobCodec> localCodec;

            if (!codecName.isEmpty())
                localCodec = BlobCodec::create(codecName);
            else
                localCodec = BlobCodec::create(BlobCodec::Type::None);

            const auto decodedResult = localCodec->decode(job._encodedData, static_cast<qsizetype>(job._size));

            if (!decodedResult._success) {
                result._error = decodedResult._error;
                return result;
            }

            result._decodedData = decodedResult._data;
            return result;
        });

    for (const auto& result : results)
    {
        if (!result._error.isEmpty())
            throw std::runtime_error(result._error.toStdString());

        if (result._decodedData.size() != static_cast<qsizetype>(result._size))
            throw std::runtime_error("Decoded block size mismatch");

        std::memcpy(bytes + result._offset,
            result._decodedData.constData(),
            static_cast<size_t>(result._size));
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
