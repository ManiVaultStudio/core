// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "ZstdBlobCodec.h"
#include "ZstdCodecSettingsAction.h"

#include "Archiver.h"

#include <zstd.h>

#ifdef _DEBUG
	#define ZSTD_CODEC_VERBOSE
#endif

namespace {

QString getZstdErrorString(const char* prefix, size_t code) {
    return QStringLiteral("%1: %2").arg(QString::fromUtf8(prefix), QString::fromUtf8(ZSTD_getErrorName(code)));
}

}

ZstdBlobCodec::ZstdBlobCodec(QObject* parent, mv::gui::CodecSettingsAction* codecSettingsAction) :
	BlobCodec(parent, codecSettingsAction)
{
#ifdef ZSTD_CODEC_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}

ZstdBlobCodec::~ZstdBlobCodec()
{
#ifdef ZSTD_CODEC_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}

mv::util::BlobCodec::Type ZstdBlobCodec::getType() const
{
    return Type::Zstd;
}

QString ZstdBlobCodec::getName() const
{
    return QStringLiteral("zstd");
}

mv::util::BlobCodec::Result ZstdBlobCodec::encode(const QByteArray& input) const
{
#ifdef ZSTD_CODEC_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (input.isEmpty())
        return { true, {}, {} };

    if (!getSettingsAction())
        return { false, {}, "Codec settings action is not set" };

    auto settings = dynamic_cast<ZstdCodecSettingsAction*>(getSettingsAction());

    if (!settings)
        return { false, {}, "Invalid codec settings action" };

    const auto bound = ZSTD_compressBound(static_cast<size_t>(input.size()));

    QByteArray output;
    output.resize(static_cast<qsizetype>(bound));

    const auto compressedSize = ZSTD_compress(output.data(), bound, input.constData(), static_cast<size_t>(input.size()),settings->getLevelAction().getValue());

    if (ZSTD_isError(compressedSize))
        return { false, {}, getZstdErrorString("ZSTD_compress failed", compressedSize) };

    output.resize(static_cast<qsizetype>(compressedSize));

    return { true, output, {} };
}

mv::util::BlobCodec::Result ZstdBlobCodec::decode(const QByteArray& input, qsizetype expectedSize) const
{
#ifdef ZSTD_CODEC_VERBOSE
    qDebug() << __FUNCTION__;
#endif

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

mv::util::BlobCodec::Result ZstdBlobCodec::decodeTo(const QByteArray& encodedData, char* destination, std::uint64_t destinationSize) const
{
#ifdef ZSTD_CODEC_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    Result result;

    if (destination == nullptr) {
        result._error = "Destination buffer is null";
        return result;
    }

    const size_t decodedSize = ZSTD_decompress(
        destination,
        static_cast<size_t>(destinationSize),
        encodedData.constData(),
        static_cast<size_t>(encodedData.size()));

    if (ZSTD_isError(decodedSize)) {
        result._error = QString("ZSTD decompression failed: %1")
            .arg(ZSTD_getErrorName(decodedSize));
        return result;
    }

    result._success = true;
    return result;
}

mv::util::BlobCodec::Result ZstdBlobCodec::decodeFromFile(const QString& filePath, qsizetype expectedSize) const
{
#ifdef ZSTD_CODEC_VERBOSE
    qDebug() << __FUNCTION__ << filePath;
#endif

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
        return { false, {}, QString("Unable to open file for reading: %1").arg(filePath) };

    const auto zstdBytes = mv::util::Archiver::readZipEntryToMemory(mv::projects().getCurrentProject()->getFilePath(), filePath);

    return decode(zstdBytes, expectedSize);
}

mv::util::BlobCodec::Result ZstdBlobCodec::decodeFromFileTo(const QString& filePath, char* destination, std::uint64_t destinationSize) const
{
#ifdef ZSTD_CODEC_VERBOSE
    qDebug() << __FUNCTION__ << filePath;
#endif

    if (destination == nullptr) {
        Result result;

    	result._error = "Destination buffer is null";
        return result;
    }

    const QByteArray encodedData = mv::util::Archiver::readZipEntryToMemory(mv::projects().getCurrentProject()->getFilePath(), filePath);

    return decodeTo(encodedData, destination, destinationSize);
}

QString ZstdBlobCodec::getFileExtension() const
{
    return QStringLiteral(".bin.zst");
}


