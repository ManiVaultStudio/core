#include "util/BlobCodec.h"

#include <QDebug>

namespace {
bool require(bool condition, const char* message)
{
    if (!condition)
        qCritical().noquote() << message;
    return condition;
}
}

bool mapsBlobCodecTypes()
{
    return require(mv::util::BlobCodec::typeToString(mv::util::BlobCodec::Type::None) == "none", "None type was mapped incorrectly")
        && require(mv::util::BlobCodec::typeToString(mv::util::BlobCodec::Type::QtCompress) == "qcompress", "QtCompress type was mapped incorrectly")
        && require(mv::util::BlobCodec::typeToString(mv::util::BlobCodec::Type::Zstd) == "zstd", "Zstd type was mapped incorrectly")
        && require(mv::util::BlobCodec::typeToString(mv::util::BlobCodec::Type::Count).isEmpty(), "Count type produced a serialized value");
}

bool parsesBlobCodecTypes()
{
    return require(mv::util::BlobCodec::typeFromString("none") == mv::util::BlobCodec::Type::None, "None string was parsed incorrectly")
        && require(mv::util::BlobCodec::typeFromString("QCOMPRESS") == mv::util::BlobCodec::Type::QtCompress, "QtCompress string was parsed case-sensitively")
        && require(mv::util::BlobCodec::typeFromString("Zstd") == mv::util::BlobCodec::Type::Zstd, "Zstd string was parsed case-sensitively");
}

int main()
{
    return mapsBlobCodecTypes()
        && parsesBlobCodecTypes() ? 0 : 1;
}
