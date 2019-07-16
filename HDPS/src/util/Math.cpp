#include "Math.h"

namespace hdps
{
namespace util
{

    float lerp(float v0, float v1, float t)
    {
        return (1 - t) * v0 + t * v1;
    }

} // namespace util

} // namespace hdps
