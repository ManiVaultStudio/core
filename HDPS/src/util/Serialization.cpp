#include "Serialization.h"

namespace hdps {

namespace util {

QVariantMap rawDataToVariant(const char* bytes, const std::int64_t& numberOfBytes, std::int64_t maxBlockSize /*= -1*/)
{
    if (maxBlockSize == -1)
        maxBlockSize = DEFAULT_MAX_BLOCK_SIZE;

    QVariantMap rawData;

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
        block["Data"] = QString(qCompress(QByteArray::fromRawData(&bytes[offset], blockSize)).toBase64());
        block["Offset"] = offset;
        block["Size"] = blockSize;

        //Q_ASSERT(blockSize > offset);

        // Append block
        blocks.push_back(block);

        // Advance to next block
        offset += maxBlockSize;
    }

    rawData["NumberOfBlocks"] = numberOfBlocks;
    rawData["BlockSize"] = maxBlockSize;
    rawData["Blocks"] = blocks;

    return rawData;
}

}
}
