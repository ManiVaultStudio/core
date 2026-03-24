// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "BlobCodec.h"

#include "actions/CodecSettingsAction.h"

#include <stdexcept>

#ifdef _DEBUG
	#define CODEC_VERBOSE
#endif

namespace mv::util {

BlobCodec::BlobCodec(QObject* parent, gui::CodecSettingsAction* codecSettingsAction) :
    QObject(parent),
    _codecSettingsAction(codecSettingsAction)
{
    //Q_ASSERT(_codecSettingsAction);

    //if (!_codecSettingsAction)
    //    throw std::invalid_argument("Codec settings action must be a valid pointer");
}

BlobCodec::Result BlobCodec::decodeTo(const QByteArray& encodedData, char* destination, std::uint64_t destinationSize) const
{
#ifdef CODEC_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    Result result;

    if (destination == nullptr) {
        result._error = "Destination buffer is null";
        return result;
    }

    const auto decodeResult = decode(encodedData);

    if (!decodeResult._success) {
        result._error = decodeResult._error;
        return result;
    }

    const auto decodedSize = static_cast<std::uint64_t>(decodeResult._data.size());

    if (decodedSize > destinationSize) {
        result._error = QString("Decoded data size (%1) exceeds destination buffer size (%2)").arg(decodedSize).arg(destinationSize);
        return result;
    }

    memcpy(destination, decodeResult._data.constData(), static_cast<size_t>(decodedSize));

    result._success = true;
    // result._decodedSize = decodedSize;
    return result;
}

BlobCodec::Result BlobCodec::encodeToFile(const QByteArray& input, const QString& filePath) const
{
#ifdef CODEC_VERBOSE
    qDebug() << __FUNCTION__ << filePath;
#endif

    auto encoded = encode(input);

    if (!encoded.isSuccess())
        return encoded;

    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly))
        return { false, {}, QString("Unable to open file for writing: %1").arg(filePath) };

    if (file.write(encoded._data) != encoded._data.size())
        return { false, {}, QString("Unable to write file: %1").arg(filePath) };

    return encoded;
}

BlobCodec::Result BlobCodec::decodeFromFile(const QString& filePath, qsizetype expectedSize) const
{
#ifdef CODEC_VERBOSE
    qDebug() << __FUNCTION__ << filePath;
#endif

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
        return { false, {}, QString("Unable to open file for reading: %1").arg(filePath) };

    return decode(file.readAll(), expectedSize);
}

BlobCodec::Result BlobCodec::decodeFromFileTo(const QString& filePath, char* destination, std::uint64_t destinationSize) const
{
#ifdef CODEC_VERBOSE
    qDebug() << __FUNCTION__ << filePath;
#endif

    Result result;

    if (destination == nullptr) {
        result._error = "Destination buffer is null";
        return result;
    }

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        result._error = QString("Unable to open file for reading: %1").arg(filePath);
        return result;
    }

    const QByteArray encodedData = file.readAll();

    if (file.error() != QFileDevice::NoError) {
        result._error = QString("Failed to read file: %1").arg(filePath);
        return result;
    }

    return decodeTo(encodedData, destination, destinationSize);
}

QString BlobCodec::typeToString(Type type)
{
    switch (type) {
	    case Type::None:        return QStringLiteral("none");
	    case Type::QtCompress:  return QStringLiteral("qcompress");
	    case Type::Zstd:        return QStringLiteral("zstd");
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

    throw std::runtime_error(QString("Unknown blob codec type: %1").arg(typeString).toStdString());
}

gui::CodecSettingsAction* BlobCodec::getSettingsAction() const
{
	return _codecSettingsAction;
}

}
