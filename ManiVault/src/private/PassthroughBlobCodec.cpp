// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "PassthroughBlobCodec.h"
#include "PassthroughCodecSettingsAction.h"

#include "Archiver.h"

#ifdef _DEBUG
	#define PASSTHROUGH_CODEC_VERBOSE
#endif

//#define PASSTHROUGH_CODEC_VERBOSE

PassthroughBlobCodec::PassthroughBlobCodec(QObject* parent, mv::gui::CodecSettingsAction* codecSettingsAction) :
    BlobCodec(parent, codecSettingsAction)
{
#ifdef PASSTHROUGH_CODEC_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}

mv::util::BlobCodec::Type PassthroughBlobCodec::getType() const
{
    return Type::None;
}

QString PassthroughBlobCodec::getName() const
{
    return QStringLiteral("none");
}

QByteArray PassthroughBlobCodec::encode(const QByteArray& input) const
{
#ifdef PASSTHROUGH_CODEC_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    return input;
}

QByteArray PassthroughBlobCodec::decode(const QByteArray& input, qsizetype expectedSize) const
{
#ifdef PASSTHROUGH_CODEC_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (expectedSize >= 0 && input.size() != expectedSize)
        throw std::runtime_error("Decoded size mismatch");

    return input;
}

QByteArray PassthroughBlobCodec::decodeFromFile(const QString& filePath, qsizetype expectedSize) const
{
#ifdef PASSTHROUGH_CODEC_VERBOSE
    qDebug() << __FUNCTION__ << filePath;
#endif

    const auto encodedData = Archiver::readZipEntryToMemory(mv::projects().getCurrentProject()->getFilePath(), filePath);

    return encodedData;
}

void PassthroughBlobCodec::decodeFromFileTo(const QString& filePath, char* destination, std::uint64_t destinationSize) const
{
#ifdef PASSTHROUGH_CODEC_VERBOSE
    qDebug() << __FUNCTION__ << filePath;
#endif

    if (destination == nullptr)
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to decode from file to buffer",
            "Destination buffer is null",
            __FUNCTION__,
            {
                { "DestinationPointer", QString::number(reinterpret_cast<std::uintptr_t>(destination), 16) },
                { "DestinationSize", QString::number(destinationSize) }
            }
        );

    const auto encodedData = mv::util::Archiver::readZipEntryToMemory(mv::projects().getCurrentProject()->getFilePath(), filePath);

    if (encodedData.isEmpty())
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to decode from file to buffer",
            "Encoded data is empty",
            __FUNCTION__,
            {
                { "FilePath", filePath }
            }
        );

    if (encodedData.size() != static_cast<qsizetype>(destinationSize))
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            QString("Encoded data size does not match destination buffer size. Encoded data size: %1 bytes, destination buffer size: %2 bytes").arg(encodedData.size()).arg(destinationSize),
            "Encoded data size != destinationSize",
            __FUNCTION__,
            {
                { "DestinationPointer", QString::number(reinterpret_cast<std::uintptr_t>(destination), 16) },
                { "DestinationSize", QString::number(destinationSize) },
                { "EncodedDataSize", QString::number(encodedData.size()) }
            }
        );

    try {
        //QMutex mutex;

        //QMutexLocker<QMutex> locker(&mutex);

        //{
            //memcpy(destination, encodedData.constData(), encodedData.size());
        //}
    }
    catch (const std::exception& exception) {
	    throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to copy decoded data to destination buffer",
            exception.what(),
            __FUNCTION__,
            {
                { "DestinationPointer", QString::number(reinterpret_cast<std::uintptr_t>(destination), 16) },
                { "DestinationSize", QString::number(destinationSize) },
                { "EncodedDataSize", QString::number(encodedData.size()) }
            }
        );
    }
    catch (...)
    {
        throw mv::ManiVaultException(
            SeverityLevel::Error,
            "Failed to copy decoded data to destination buffer: unknown error",
            "Unknown error",
            __FUNCTION__,
            {
                { "DestinationPointer", QString::number(reinterpret_cast<std::uintptr_t>(destination), 16) },
                { "DestinationSize", QString::number(destinationSize) },
                { "EncodedDataSize", QString::number(encodedData.size()) }
            }
        );
    }
}

QString PassthroughBlobCodec::getFileExtension() const
{
    return QStringLiteral(".bin");
}

