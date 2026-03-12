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
