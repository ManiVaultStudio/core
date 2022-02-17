#include "Serialization.h"

#include <QUuid>

#include <exception>

namespace hdps {

namespace util {

void saveRawDataToBinaryFile(const char* bytes, const std::int64_t& numberOfBytes, const QString& filePath)
{
    // Output directory
    const auto outputDirectory = QFileInfo(filePath).dir();

    // Exit prematurely if the target directory does not exist
    if (!QFileInfo(filePath).dir().exists())
        throw std::runtime_error(QString("Unable to save data in %1, the directory does not exist").arg(outputDirectory.dirName()).toLatin1());

    // Raw bytes
    QByteArray rawData(bytes, numberOfBytes);

    // Copy the block to the raw data byte array
    memcpy((void*)rawData.data(), bytes, numberOfBytes);

    // Create binary file
    QFile binaryFile(filePath);

    // And save it do disk
    binaryFile.open(QIODevice::WriteOnly);
    binaryFile.write(rawData);
    binaryFile.close();
}

void loadRawDataFromBinaryFile(const char* bytes, const std::int64_t& numberOfBytes, const QString& filePath)
{
    // Exit prematurely if the target file does not exist
    if (!QFileInfo(filePath).exists())
        throw std::runtime_error(QString("Unable to load binary file, %1 does not exist").arg(filePath).toLatin1());

    // Create binary file
    QFile binaryFile(filePath);

    // Open the file read-only
    if (!binaryFile.open(QIODevice::ReadOnly))
        throw std::runtime_error("Unable to load binary file, cannot open file");

    // Read the raw data from the file
    QByteArray rawData = binaryFile.readAll();

    // Determine number of bytes in file
    const auto numberOfBytesInFile = rawData.size();

    // Except if the number of bytes in the file deviates from the number of requested bytes
    if (numberOfBytesInFile != numberOfBytes)
        throw std::runtime_error("Unable to load binary file, number of requested bytes is not the same as in the file");

    // Copy the block to the raw data byte array
    memcpy((void*)bytes, (void*)rawData.data(), numberOfBytes);
}

QVariantMap rawDataToVariantMap(const char* bytes, const std::int64_t& numberOfBytes, bool saveToDisk /*= false*/, std::int64_t maxBlockSize /*= -1*/)
{
    if (maxBlockSize == -1)
        maxBlockSize = DEFAULT_MAX_BLOCK_SIZE;

    QVariantMap rawData;

    // Save the number of bytes
    rawData["Size"] = numberOfBytes;

    if (saveToDisk) {

        // File name of the external binary file
        const auto fileName = QUuid::createUuid().toString(QUuid::WithoutBraces) + ".bin";

        // Save the raw data to binary file
        saveRawDataToBinaryFile(bytes, numberOfBytes, fileName);

        // Set the raw data URL
        rawData["URL"] = fileName;
    }
    else {

        // Compute the number of blocks
        const auto numberOfBlocks = static_cast<std::uint64_t>(ceilf(numberOfBytes / static_cast<float>(maxBlockSize)));

        // Offset in number of bytes
        std::int64_t offset = 0;

        QVariantList blocks;

        while (offset < numberOfBytes)
        {
            QVariantMap block;

            // Determine the size of the block
            const auto blockSize = std::min(maxBlockSize, numberOfBytes - offset);

            // Create data block
            block["Data"]   = QString(qCompress(QByteArray::fromRawData(&bytes[offset], blockSize)).toBase64());
            block["Offset"] = offset;
            block["Size"]   = blockSize;

            // Append block to the blocks
            blocks.push_back(block);

            // Advance to next block 
            offset += maxBlockSize;
        }

        rawData["NumberOfBlocks"]   = numberOfBlocks;
        rawData["BlockSize"]        = maxBlockSize;
        rawData["Blocks"]           = blocks;
    }

    return rawData;
}

void populateDataBufferFromVariantMap (const QVariantMap& variantMap, const char* bytes)
{
    if (variantMap.contains("URL")) {
        loadRawDataFromBinaryFile(bytes, variantMap["Size"].toInt(), variantMap["URL"].toString());
    }
    else {
        variantMapMustContain(variantMap, "BlockSize");
        variantMapMustContain(variantMap, "Blocks");

        const auto blockSize = variantMap["BlockSize"].toInt();
        const auto blocks = variantMap["Blocks"].toList();

        // Go over all blocks in the blocks map and copy the raw data to the output bytes
        for (const auto& block : blocks) {
            const auto map = block.toMap();

            variantMapMustContain(map, "Data");
            variantMapMustContain(map, "Offset");
            variantMapMustContain(map, "Size");

            const auto data         = map["Data"].toString();
            const auto offset       = map["Offset"].toInt();
            const auto size         = map["Size"].toInt();
            const auto blockData    = qUncompress(QByteArray::fromBase64(data.toUtf8()));

            // Copy the block to the output bytes
            memcpy((void*)&bytes[offset], blockData.data(), size);
        }
    }
    
}

void variantMapMustContain(const QVariantMap& variantMap, const QString& key)
{
    if (!variantMap.contains(key))
        throw std::runtime_error(QString("%1 not found in map").arg(key).toLatin1());
}

}
}
