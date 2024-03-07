// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Serialization.h"
#include "CoreInterface.h"
#include "Application.h"

#include <QUuid>

#include <exception>

#include <math.h>

namespace mv::util {

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

QVariantMap rawDataToVariantMap(const char* bytes, const std::uint64_t& numberOfBytes, bool saveToDisk /*= false*/, std::uint64_t maxBlockSize /*= DEFAULT_MAX_BLOCK_SIZE*/)
{
    Q_ASSERT(maxBlockSize != 0);

    if (maxBlockSize == -1)
        maxBlockSize = DEFAULT_MAX_BLOCK_SIZE;

    QVariantMap rawData;

    // Save the number of bytes
    rawData["Size"] = QVariant::fromValue(numberOfBytes);

    // Compute the number of blocks
    const auto numberOfBlocks = static_cast<std::uint64_t>(ceilf(numberOfBytes / static_cast<float>(maxBlockSize)));

    // Offset in number of bytes
    std::uint64_t offset = 0;

    QVariantList blocks;

    while (offset < numberOfBytes)
    {
        QVariantMap block;

        // Determine the size of the block
        const auto blockSize = std::min(maxBlockSize, numberOfBytes - offset);

        block["Offset"] = QVariant::fromValue(offset);
        block["Size"]   = QVariant::fromValue(blockSize);

        if (saveToDisk) {

            // File name and path of the external binary file in the temporary directory
            const auto fileName = QUuid::createUuid().toString(QUuid::WithoutBraces) + ".bin";
            const auto filePath = QDir::cleanPath(projects().getTemporaryDirPath(AbstractProjectManager::TemporaryDirType::Save) + QDir::separator() + fileName);

            // Save the raw data to binary file
            saveRawDataToBinaryFile(&bytes[offset], blockSize, filePath);

            // Set the raw data URL
            block["URI"] = fileName;
        }
        else {

            // Create data block
            block["Data"] = QString(qCompress(QByteArray::fromRawData(&bytes[offset], blockSize)).toBase64());
        }

        // Append block to the blocks
        blocks.push_back(block);

        // Advance to next block 
        offset += maxBlockSize;
    }

    rawData["NumberOfBlocks"]   = QVariant::fromValue(numberOfBlocks);
    rawData["BlockSize"]        = QVariant::fromValue(maxBlockSize);
    rawData["Blocks"]           = QVariant::fromValue(blocks);

    return rawData;
}

void populateDataBufferFromVariantMap(const QVariantMap& variantMap, char* bytes)
{
    variantMapMustContain(variantMap, "BlockSize");
    variantMapMustContain(variantMap, "Blocks");

    const auto blockSize    = variantMap["BlockSize"].toInt();
    const auto blocks       = variantMap["Blocks"].toList();

    // Go over all blocks in the blocks map and copy the raw data to the output bytes
    for (const auto& block : blocks) {

        // Get block variant map
        const auto map = block.toMap();

        variantMapMustContain(map, "Offset");
        variantMapMustContain(map, "Size");

        const auto offset   = map["Offset"].value<uint64_t>();
        const auto size     = map["Size"].value<uint64_t>();

        if (map.contains("URI"))
            loadRawDataFromBinaryFile(&bytes[offset], size, QDir::cleanPath(projects().getTemporaryDirPath(AbstractProjectManager::TemporaryDirType::Open) + QDir::separator() + map["URI"].toString()));

        if (map.contains("Data")) {
            const auto data         = map["Data"].toString();
            const auto blockData    = qUncompress(QByteArray::fromBase64(data.toUtf8()));

            // Copy the block to the output bytes
            memcpy((void*)&bytes[offset], blockData.data(), size);
        }
    }
}

void variantMapMustContain(const QVariantMap& variantMap, const QString& key)
{
    if (!variantMap.contains(key))
        throw std::runtime_error(QString("%1 not found in map: %2").arg(key, variantMap.keys().join(", ")).toLatin1());
}

}
