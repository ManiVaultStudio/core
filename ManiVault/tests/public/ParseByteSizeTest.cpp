#include "util/Miscellaneous.h"

#include <QDebug>

#include <cstdint>
#include <functional>
#include <stdexcept>

namespace {
bool require(bool condition, const char* message)
{
    if (!condition)
        qCritical().noquote() << message;
    return condition;
}

bool rejectsInvalidInput(const QString& input)
{
    try {
        [[maybe_unused]] const auto result = mv::util::parseByteSize(input);
    }
    catch (const std::invalid_argument&) {
        return true;
    }
    catch (...) {
        return false;
    }

    return false;
}
}

bool parsesNormalizedAndFractionalSizes()
{
    return require(mv::util::parseByteSize(" 1.5 mib ") == 1'572'864ULL, "fractional MiB was parsed incorrectly")
        && require(mv::util::parseByteSize("2GB") == 2'147'483'648ULL, "GB value was parsed incorrectly")
        && require(mv::util::parseByteSize("512 B") == 512ULL, "byte value was parsed incorrectly");
}

bool rejectsMalformedSizes()
{
    return require(rejectsInvalidInput(""), "empty byte size was accepted")
        && require(rejectsInvalidInput("12"), "unitless byte size was accepted")
        && require(rejectsInvalidInput("1.5 XB"), "unknown byte unit was accepted")
        && require(rejectsInvalidInput("-1 MB"), "negative byte size was accepted");
}

bool formatsIECAndSISizes()
{
    return require(mv::util::getNoBytesHumanReadable(1023) == "1023.00 B", "IEC byte boundary was formatted incorrectly")
        && require(mv::util::getNoBytesHumanReadable(1024) == "1.00 KiB", "IEC kilobyte value was formatted incorrectly")
        && require(mv::util::getNoBytesHumanReadable(1'000'000, false) == "1.00 MB", "SI megabyte value was formatted incorrectly");
}

int main()
{
    return parsesNormalizedAndFractionalSizes()
        && rejectsMalformedSizes()
        && formatsIECAndSISizes() ? 0 : 1;
}
