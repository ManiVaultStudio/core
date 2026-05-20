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

static ZSTD_DCtx* getThreadLocalDCtx()
{
    thread_local ZSTD_DCtx* dctx = ZSTD_createDCtx();

    if (!dctx)
        throw std::runtime_error("ZSTD_createDCtx failed");

    return dctx;
}

}

ZstdBlobCodec::ZstdBlobCodec(QObject* parent, mv::gui::CodecSettingsAction* codecSettingsAction) :
	BlobCodec(parent, codecSettingsAction)
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

QByteArray ZstdBlobCodec::encode(const QByteArray& input) const
{
#ifdef ZSTD_CODEC_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (input.isEmpty())
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to encode input data",
            "Input data is empty",
            __FUNCTION__,
            {
                { "InputSize", QString::number(input.size()) }
            }
        );

    auto settings = dynamic_cast<ZstdCodecSettingsAction*>(getSettingsAction());

    if (!settings)
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to encode input data",
            "Invalid codec settings action",
            __FUNCTION__,
            {
                { "InputSize", QString::number(input.size()) }
            }
        );

    const auto bound = ZSTD_compressBound(static_cast<size_t>(input.size()));

    QByteArray output;

    output.resize(static_cast<qsizetype>(bound));

    const auto compressedSize = ZSTD_compress(output.data(), bound, input.constData(), static_cast<size_t>(input.size()),settings->getLevelAction().getValue());

    if (ZSTD_isError(compressedSize))
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to encode input data",
            getZstdErrorString("ZSTD_compress failed", compressedSize),
            __FUNCTION__,
            {
                { "InputSize", QString::number(input.size()) }
            }
        );

    output.resize(static_cast<qsizetype>(compressedSize));

    return output;
}

QByteArray ZstdBlobCodec::decode(const QByteArray& input, qsizetype expectedSize) const
{
#ifdef ZSTD_CODEC_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (input.isEmpty()) {
        if (expectedSize > 0)
            throw mv::ManiVaultException(
                SeverityLevel::Error,
                "Failed to decode input data",
                "Input is empty but expected decoded data",
                __FUNCTION__,
                {
                    { "ExpectedSize", QString::number(expectedSize) }
                }
            );
    }

    if (expectedSize < -1)
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to decode input data",
            "Expected decoded size cannot be negative",
            __FUNCTION__,
            {
                { "ExpectedSize", QString::number(expectedSize) }
            }
        );

    unsigned long long frameContentSize = ZSTD_getFrameContentSize(input.constData(), static_cast<size_t>(input.size()));

    if (frameContentSize == ZSTD_CONTENTSIZE_ERROR)
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to decode input data",
            "Input is not a valid zstd frame",
            __FUNCTION__,
            {
                { "InputSize", QString::number(input.size()) },
                { "ExpectedSize", QString::number(expectedSize) }
            }
        );

    if (frameContentSize == ZSTD_CONTENTSIZE_UNKNOWN) {
        if (expectedSize < 0)
            throw mv::ManiVaultException(
                SeverityLevel::Error,
                "Failed to decode input data",
                "Expected decoded size is required for this zstd frame",
                __FUNCTION__,
                {
                    { "ExpectedSize", QString::number(expectedSize) }
                }
            );

        frameContentSize = static_cast<unsigned long long>(expectedSize);
    }

    if (frameContentSize > static_cast<unsigned long long>(std::numeric_limits<qsizetype>::max()))
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to decode input data",
            "Decoded size exceeds maximum QByteArray size",
            __FUNCTION__,
            {
                { "FrameContentSize", QString::number(frameContentSize) }
            }
        );

    if (expectedSize >= 0 && frameContentSize != static_cast<unsigned long long>(expectedSize)) {
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to decode input data",
            QStringLiteral("Zstd frame decoded size mismatch. Frame reports %1 bytes, expected %2 bytes")
                .arg(frameContentSize)
                .arg(expectedSize),
            __FUNCTION__,
            {
                { "FrameContentSize", QString::number(frameContentSize) },
                { "ExpectedSize", QString::number(expectedSize) }
            }
        );
    }

    QByteArray output;

    output.resize(static_cast<qsizetype>(frameContentSize));

    if (output.size() != static_cast<qsizetype>(frameContentSize))
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to decode input data",
            "Failed to allocate decoded output buffer of the required size",
            __FUNCTION__,
            {
                { "FrameContentSize", QString::number(frameContentSize) }
            }
        );

    const auto decompressedSize = ZSTD_decompress(output.data(), static_cast<size_t>(output.size()), input.constData(), static_cast<size_t>(input.size()));

    if (ZSTD_isError(decompressedSize))
        throw std::runtime_error(getZstdErrorString("ZSTD_decompress failed", decompressedSize).toStdString());

    if (decompressedSize > static_cast<size_t>(output.size()))
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to decode input data",
            "ZSTD_decompress returned more bytes than output buffer size",
            __FUNCTION__,
            {
                { "DecompressedSize", QString::number(decompressedSize) },
                { "OutputSize", QString::number(output.size()) }
            }
        );

    output.resize(static_cast<qsizetype>(decompressedSize));

    if (expectedSize >= 0 && output.size() != expectedSize)
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to decode input data",
            QStringLiteral("Decoded size mismatch. Decoded %1 bytes, expected %2 bytes")
                .arg(output.size())
                .arg(expectedSize),
            __FUNCTION__,
            {
                { "DecodedSize", QString::number(output.size()) },
                { "ExpectedSize", QString::number(expectedSize) }
            }
        );

    return output;
}

void ZstdBlobCodec::decodeTo(const QByteArray& encodedData, char* destination, std::uint64_t destinationSize) const
{
#ifdef ZSTD_CODEC_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (encodedData.isEmpty())
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to decode input data",
            "Encoded data is empty",
            __FUNCTION__
        );

    if (destinationSize == 0)
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to decode input data",
            "Destination buffer size is zero",
            __FUNCTION__
        );

    auto dctx = getThreadLocalDCtx();

    const auto decodedSize = ZSTD_decompressDCtx(dctx, destination, destinationSize, encodedData.constData(), static_cast<size_t>(encodedData.size()));

    if (ZSTD_isError(decodedSize))
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to decode input data",
            QString("ZSTD decompression failed: %1").arg(ZSTD_getErrorName(decodedSize)),
            __FUNCTION__
        );

    if (decodedSize != destinationSize)
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to decode input data",
            QString("Decoded size mismatch: expected %1 bytes, got %2 bytes").arg(destinationSize).arg(static_cast<std::uint64_t>(decodedSize)),
            __FUNCTION__
        );
}

QByteArray ZstdBlobCodec::decodeFromFile(const QString& filePath, qsizetype expectedSize) const
{
#ifdef ZSTD_CODEC_VERBOSE
    qDebug() << __FUNCTION__ << filePath;
#endif

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to open file for reading",
            QString("Unable to open file for reading: %1").arg(filePath),
            __FUNCTION__
        );

    const auto zstdBytes = mv::util::Archiver::readZipEntryToMemory(mv::projects().getCurrentProject()->getFilePath(), filePath);

    if (zstdBytes.isEmpty()) {
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to decode input data",
            QString("ZIP entry '%1' is empty").arg(filePath),
            __FUNCTION__
        );
    }

    return decode(zstdBytes, expectedSize);
}

void ZstdBlobCodec::decodeFromFileTo(const QString& filePath, char* destination, std::uint64_t destinationSize) const
{
#ifdef ZSTD_CODEC_VERBOSE
    qDebug() << __FUNCTION__ << filePath;
#endif

    if (destination == nullptr)
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to decode input data",
            "Destination buffer is null",
            __FUNCTION__
        );

    const auto encodedData = Archiver::readZipEntryToMemory(mv::projects().getCurrentProject()->getFilePath(), filePath);

    return decodeTo(encodedData, destination, destinationSize);
}

QString ZstdBlobCodec::getFileExtension() const
{
    return QStringLiteral(".bin.zst");
}


