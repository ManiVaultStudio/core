// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "BlobCodec.h"

#include <stdexcept>

namespace mv::util {

namespace
{
    std::map<BlobCodec::Type, BlobCodec::FactoryFunction> factories;
    std::mutex factoriesMutex;
}

BlobCodec::Result BlobCodec::decodeTo(const QByteArray& encodedData, char* destination, std::uint64_t destinationSize) const
{
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
    const auto encoded = encode(input);

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
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
        return { false, {}, QString("Unable to open file for reading: %1").arg(filePath) };

    return decode(file.readAll(), expectedSize);
}

BlobCodec::Result BlobCodec::decodeFromFileTo(const QString& filePath, char* destination, std::uint64_t destinationSize) const
{
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

void BlobCodec::registerCodec(Type type, FactoryFunction factoryFunction)
{
    std::scoped_lock lock(factoriesMutex);

	factories[type] = std::move(factoryFunction);
}

bool BlobCodec::isRegistered(Type type)
{
	std::scoped_lock lock(factoriesMutex);
    return factories.find(type) != factories.end();
}

bool BlobCodec::isRegistered(const QString& typeString)
{
    return isRegistered(typeFromString(typeString));
}

std::unique_ptr<BlobCodec> BlobCodec::create(Type type)
{
    std::scoped_lock lock(factoriesMutex);

    const auto it = factories.find(type);

    if (it == factories.end())
        throw std::runtime_error("No factory registered for requested blob codec type");

    return it->second();
}

std::unique_ptr<BlobCodec> BlobCodec::create(const QString& typeString)
{
    return create(typeFromString(typeString));
}

}
