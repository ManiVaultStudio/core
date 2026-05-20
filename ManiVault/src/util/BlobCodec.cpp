// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "BlobCodec.h"

#include "actions/CodecSettingsAction.h"

#include <stdexcept>

#ifdef _DEBUG
	#define CODEC_VERBOSE
#endif

using namespace mv::gui;

namespace mv::util {

BlobCodec::BlobCodec(QObject* parent, const CodecSettingsActionPtr& codecSettingsAction) :
    QObject(parent),
    _codecSettingsAction(codecSettingsAction)
{
}

void BlobCodec::decodeTo(const QByteArray& encodedData, char* destination, std::uint64_t destinationSize) const
{
#ifdef CODEC_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (destination == nullptr)
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to decode to buffer",
            "Destination buffer is null",
            __FUNCTION__,
            {
                { "DestinationPointer", QString::number(reinterpret_cast<std::uintptr_t>(destination), 16) },
                { "DestinationSize", QString::number(destinationSize) }
            }
        );

    const auto decodedBytes = decode(encodedData);
    const auto decodedSize  = static_cast<std::uint64_t>(decodedBytes.size());

    if (decodedSize > destinationSize) {
        throw ManiVaultException(
            SeverityLevel::Error,
            QString("Decoded data size (%1) exceeds destination buffer size (%2)").arg(decodedSize).arg(destinationSize),
            "DecodedSize > destinationSize",
            __FUNCTION__,
            {
				{ "DestinationPointer", QString::number(reinterpret_cast<std::uintptr_t>(destination), 16) },
                { "DestinationSize", QString::number(destinationSize) },
                { "EncodedDataSize", QString::number(encodedData.size()) }
            }
        );
    }

    try {
        memcpy(destination, decodedBytes.constData(), decodedSize);
    }
    catch (const std::exception& e) {
        throw ManiVaultException(
            SeverityLevel::Error,
            QString("Failed to copy decoded data to destination buffer: %1").arg(e.what()),
            "DecodedSize > destinationSize",
            __FUNCTION__,
            {
                { "DestinationPointer", QString::number(reinterpret_cast<std::uintptr_t>(destination), 16) },
                { "DestinationSize", QString::number(destinationSize) },
                { "EncodedDataSize", QString::number(encodedData.size()) }
            }
        );
    }
    catch (...) {
        throw ManiVaultException(
            SeverityLevel::Error,
            "Failed to copy decoded data to destination buffer: unknown error",
            "DecodedSize > destinationSize",
            __FUNCTION__,
            {
                { "DestinationPointer", QString::number(reinterpret_cast<std::uintptr_t>(destination), 16) },
                { "DestinationSize", QString::number(destinationSize) },
                { "EncodedDataSize", QString::number(encodedData.size()) }
            }
        );
    }
}
void BlobCodec::encodeToFile(const QByteArray& input, const QString& filePath, std::uint64_t* numberOfEncodedBytes /*= nullptr*/) const
{
#ifdef CODEC_VERBOSE
    qDebug() << __FUNCTION__ << filePath;
#endif

    auto encodedBytes = encode(input);

    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly))
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to open file for writing",
            QString("Unable to open file for writing: %1").arg(filePath),
            __FUNCTION__,
            {
                { "FilePath", filePath },
                { "InputSize", QString::number(input.size()) },
                { "EncodedDataSize", QString::number(encodedBytes.size()) }
            }
        );

    if (file.write(encodedBytes) != encodedBytes.size())
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to write file",
            QString("Unable to write file: %1").arg(filePath),
            __FUNCTION__,
            {
                { "FilePath", filePath },
                { "InputSize", QString::number(input.size()) },
                { "EncodedDataSize", QString::number(encodedBytes.size()) }
            }
        );

    if (numberOfEncodedBytes != nullptr)
        *numberOfEncodedBytes = static_cast<std::uint64_t>(encodedBytes.size());
}

QByteArray BlobCodec::decodeFromFile(const QString& filePath, qsizetype expectedSize) const
{
#ifdef CODEC_VERBOSE
    qDebug() << __FUNCTION__ << filePath;
#endif

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to open file for reading",
            QString("Unable to open file for reading: %1").arg(filePath),
            __FUNCTION__,
            {
                { "FilePath", filePath },
                { "ExpectedSize", QString::number(expectedSize) }
            }
        );

    return decode(file.readAll(), expectedSize);
}

void BlobCodec::decodeFromFileTo(const QString& filePath, char* destination, std::uint64_t destinationSize) const
{
#ifdef CODEC_VERBOSE
    qDebug() << __FUNCTION__ << filePath;
#endif

    if (destination == nullptr)
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to decode to destination buffer",
            "Destination buffer is null",
            __FUNCTION__,
			{
                { "DestinationPointer", QString::number(reinterpret_cast<std::uintptr_t>(destination), 16) },
                { "DestinationSize", QString::number(destinationSize) },
                { "FilePath", filePath }
            }
        );

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to open file for reading",
            QString("Unable to open file for reading: %1").arg(filePath),
            __FUNCTION__,
            {
                { "FilePath", filePath },
                { "DestinationPointer", QString::number(reinterpret_cast<std::uintptr_t>(destination), 16) },
                { "DestinationSize", QString::number(destinationSize) }
            }
        );

    if (file.error() != QFileDevice::NoError)
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to read file",
            QString("Failed to read file: %1").arg(filePath),
            __FUNCTION__,
            {
                { "FilePath", filePath },
                { "DestinationPointer", QString::number(reinterpret_cast<std::uintptr_t>(destination), 16) },
                { "DestinationSize", QString::number(destinationSize) }
            }
        );

    decodeTo(file.readAll(), destination, destinationSize);
}

QString BlobCodec::typeToString(Type type)
{
    switch (type) {
	    case Type::None:        return QStringLiteral("none");
	    case Type::QtCompress:  return QStringLiteral("qcompress");
	    case Type::Zstd:        return QStringLiteral("zstd");

        case Type::Count:
            break; // not a valid type, just a count of the number of types
    }

    return {};
}

BlobCodec::Type BlobCodec::typeFromString(const QString& typeString)
{
    if (typeString.compare(QStringLiteral("none"), Qt::CaseInsensitive) == 0)
        return Type::None;

    if (typeString.compare(QStringLiteral("qcompress"), Qt::CaseInsensitive) == 0)
        return Type::QtCompress;

    if (typeString.compare(QStringLiteral("zstd"), Qt::CaseInsensitive) == 0)
        return Type::Zstd;

    throw mv::ManiVaultException(
        SeverityLevel::Error,
        "Unknown blob codec type",
        QString("Unknown blob codec type: %1").arg(typeString),
        __FUNCTION__,
        {
            { "TypeString", typeString }
        }
    );
}

gui::CodecSettingsAction* BlobCodec::getSettingsAction() const
{
	return _codecSettingsAction;
}

}
