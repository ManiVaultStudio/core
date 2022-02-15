#pragma once

#include <QVariantMap>

#define DEFAULT_MAX_BLOCK_SIZE 1000000LL

namespace hdps {

namespace util {

QVariantMap rawDataToVariant(const char* bytes, const std::int64_t& numberOfBytes, std::int64_t maxBlockSize = -1);

}
}
