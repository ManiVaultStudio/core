// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "PassthroughBlobCodec.h"
#include "PassthroughCodecSettingsAction.h"

#include "Archiver.h"

#ifdef _DEBUG
	#define PASSTHROUGH_CODEC_VERBOSE
#endif

#define PASSTHROUGH_CODEC_VERBOSE

PassthroughBlobCodec::PassthroughBlobCodec(QObject* parent, mv::gui::CodecSettingsAction* codecSettingsAction) :
    BlobCodec(parent, codecSettingsAction)
{
#ifdef PASSTHROUGH_CODEC_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}

PassthroughBlobCodec::~PassthroughBlobCodec()
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

mv::util::BlobCodec::Result PassthroughBlobCodec::encode(const QByteArray& input) const
{
#ifdef PASSTHROUGH_CODEC_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    return { true, input, {} };
}

mv::util::BlobCodec::Result PassthroughBlobCodec::decode(const QByteArray& input, qsizetype expectedSize) const
{
#ifdef PASSTHROUGH_CODEC_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (expectedSize >= 0 && input.size() != expectedSize)
        return { false, {}, QStringLiteral("Decoded size mismatch") };

    return { true, input, {} };
}

mv::util::BlobCodec::Result PassthroughBlobCodec::decodeFromFile(const QString& filePath, qsizetype expectedSize) const
{
#ifdef PASSTHROUGH_CODEC_VERBOSE
    qDebug() << __FUNCTION__ << filePath;
#endif

    const QByteArray encodedData = mv::util::Archiver::readZipEntryToMemory(mv::projects().getCurrentProject()->getFilePath(), filePath);

    return { true, encodedData, {} };
}

mv::util::BlobCodec::Result PassthroughBlobCodec::decodeFromFileTo(const QString& filePath, char* destination, std::uint64_t destinationSize) const
{
#ifdef PASSTHROUGH_CODEC_VERBOSE
    qDebug() << __FUNCTION__ << filePath;
#endif

    const QByteArray encodedData = mv::util::Archiver::readZipEntryToMemory(mv::projects().getCurrentProject()->getFilePath(), filePath);

    if (encodedData.isEmpty())
        return { false, {}, QStringLiteral("Failed to read encoded data from file") };

    if (encodedData.size() > static_cast<qsizetype>(destinationSize))
        return { false, {}, QStringLiteral("Destination buffer is too small for decoded data") };

    memcpy(destination, encodedData.constData(), encodedData.size());

	return { true, {}, {} };
}

QString PassthroughBlobCodec::getFileExtension() const
{
    return QStringLiteral(".bin");
}

