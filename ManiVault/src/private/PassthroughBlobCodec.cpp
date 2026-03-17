// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "PassthroughBlobCodec.h"
#include "PassthroughCodecSettingsAction.h"

#include "Archiver.h"

PassthroughBlobCodec::PassthroughBlobCodec(QObject* parent, mv::gui::CodecSettingsAction* codecSettingsAction) :
    BlobCodec(parent, codecSettingsAction ? codecSettingsAction : new PassthroughCodecSettingsAction(this, "Settings"))
{
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
    return { true, input, {} };
}

mv::util::BlobCodec::Result PassthroughBlobCodec::decode(const QByteArray& input, qsizetype expectedSize) const
{
    if (expectedSize >= 0 && input.size() != expectedSize)
        return { false, {}, QStringLiteral("Decoded size mismatch") };

    return { true, input, {} };
}

mv::util::BlobCodec::Result PassthroughBlobCodec::decodeFromFile(const QString& filePath, qsizetype expectedSize) const
{
    const QByteArray encodedData = mv::util::Archiver::readZipEntryToMemory(mv::projects().getCurrentProject()->getFilePath(), filePath);

    return { true, encodedData, {} };
}

mv::util::BlobCodec::Result PassthroughBlobCodec::decodeFromFileTo(const QString& filePath, char* destination, std::uint64_t destinationSize) const
{
    const QByteArray encodedData = mv::util::Archiver::readZipEntryToMemory(mv::projects().getCurrentProject()->getFilePath(), filePath);

    memcpy(destination, encodedData.constData(), encodedData.size());

	return { true, {}, {} };
}

QString PassthroughBlobCodec::getFileExtension() const
{
    return QStringLiteral(".bin");
}

