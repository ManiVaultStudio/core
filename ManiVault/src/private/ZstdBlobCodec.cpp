// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "ZstdBlobCodec.h"

#include <zstd.h>

namespace mv::util {

namespace {

    QString getZstdErrorString(const char* prefix, size_t code)
    {
        return QStringLiteral("%1: %2")
            .arg(QString::fromUtf8(prefix), QString::fromUtf8(ZSTD_getErrorName(code)));
    }

}

ZstdBlobCodec::ZstdBlobCodec(int compressionLevel) :
    _compressionLevel(compressionLevel)
{
}

BlobCodec::Type ZstdBlobCodec::getType() const
{
    return Type::Zstd;
}

QString ZstdBlobCodec::getName() const
{
    return QStringLiteral("zstd");
}

BlobCodec::Result ZstdBlobCodec::encode(const QByteArray& input) const
{
    if (input.isEmpty())
        return { true, {}, {} };

    const auto bound = ZSTD_compressBound(static_cast<size_t>(input.size()));

    QByteArray output;
    output.resize(static_cast<qsizetype>(bound));

    const auto compressedSize = ZSTD_compress(output.data(),
        bound,
        input.constData(),
        static_cast<size_t>(input.size()),
        _compressionLevel);

    if (ZSTD_isError(compressedSize))
        return { false, {}, getZstdErrorString("ZSTD_compress failed", compressedSize) };

    output.resize(static_cast<qsizetype>(compressedSize));

    return { true, output, {} };
}

BlobCodec::Result ZstdBlobCodec::decode(const QByteArray& input, qsizetype expectedSize) const
{
    if (input.isEmpty()) {
        if (expectedSize > 0)
            return { false, {}, QStringLiteral("Input is empty but expected decoded data") };

        return { true, {}, {} };
    }

    unsigned long long frameContentSize = ZSTD_getFrameContentSize(input.constData(),
        static_cast<size_t>(input.size()));

    if (frameContentSize == ZSTD_CONTENTSIZE_ERROR)
        return { false, {}, QStringLiteral("Input is not a valid zstd frame") };

    if (frameContentSize == ZSTD_CONTENTSIZE_UNKNOWN) {
        if (expectedSize < 0)
            return { false, {}, QStringLiteral("Expected decoded size is required for this zstd frame") };

        frameContentSize = static_cast<unsigned long long>(expectedSize);
    }

    QByteArray output;
    output.resize(static_cast<qsizetype>(frameContentSize));

    const auto decompressedSize = ZSTD_decompress(output.data(),
        static_cast<size_t>(output.size()),
        input.constData(),
        static_cast<size_t>(input.size()));

    if (ZSTD_isError(decompressedSize))
        return { false, {}, getZstdErrorString("ZSTD_decompress failed", decompressedSize) };

    output.resize(static_cast<qsizetype>(decompressedSize));

    if (expectedSize >= 0 && output.size() != expectedSize)
        return { false, {}, QStringLiteral("Decoded size mismatch") };

    return { true, output, {} };
}

void ZstdBlobCodec::setCompressionLevel(int compressionLevel)
{
    _compressionLevel = compressionLevel;
}

int ZstdBlobCodec::getCompressionLevel() const
{
    return _compressionLevel;
}

}