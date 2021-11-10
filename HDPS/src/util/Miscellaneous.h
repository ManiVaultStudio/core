#pragma once

#include <QString>

namespace hdps
{
namespace util
{

/**
 * Returns a human readable string of an integer count
 * @param count Integer count
 * @return Human readable string of an integer count
 */
QString getIntegerCountHumanReadable(const float& count);

/**
 * Returns a human readable string of a byte count
 * @param noBytes Number of bytes
 * @return Human readable string of a byte count
 */
QString getNoBytesHumanReadable(float noBytes);

}
}
