// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "BlobCodec.h"

#include "PassthroughBlobCodec.h"

#include <stdexcept>

namespace mv::util {

QString BlobCodec::typeToString(Type type)
{
    switch (type) {
	    case Type::None:        return QStringLiteral("none");
	    case Type::QtCompress:  return QStringLiteral("qcompress");
	    case Type::Zstd:        return QStringLiteral("zstd");
    }

    Q_UNREACHABLE();
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

BlobCodec* BlobCodec::create(Type type)
{
    switch (type) {
        case Type::None:
            return new PassthroughBlobCodec();

        case Type::QtCompress:
        case Type::Zstd:
            break;
    }

    Q_UNREACHABLE();

    return nullptr;
}

}