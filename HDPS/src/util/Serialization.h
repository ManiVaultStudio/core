#pragma once

#include <QVariantMap>

#define DEFAULT_MAX_BLOCK_SIZE 1000000LL

namespace hdps {

namespace util {

/**
 * Convert raw data buffer to variant map
 * @param bytes Pointer to input buffer
 * @param numberOfBytes Number of input bytes 
 * @param maxBlockSize Maximum size per block (DEFAULT_MAX_BLOCK_SIZE when maxBlockSize == -1)
 */
QVariantMap rawDataToVariantMap(const char* bytes, const std::int64_t& numberOfBytes, std::int64_t maxBlockSize = -1);

/**
 * Convert variant map to raw data
 * @param variantMap Variant map containing the data blocks
 * @param bytes Output buffer to which the data is copied
 */
void variantMapToRawData(const QVariantMap& variantMap, const char* bytes);

}
}
