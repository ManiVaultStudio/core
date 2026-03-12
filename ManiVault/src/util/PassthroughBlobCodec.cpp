// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "PassthroughBlobCodec.h"

namespace mv::util {

BlobCodec::Type PassthroughBlobCodec::getType() const
{
    return Type::None;
}

QString PassthroughBlobCodec::getName() const
{
    return QStringLiteral("none");
}

BlobCodec::Result PassthroughBlobCodec::encode(const QByteArray& input) const
{
    return { true, input, {} };
}

BlobCodec::Result PassthroughBlobCodec::decode(const QByteArray& input, qsizetype expectedSize) const
{
    if (expectedSize >= 0 && input.size() != expectedSize)
        return { false, {}, QStringLiteral("Decoded size mismatch") };

    return { true, input, {} };
}

QString PassthroughBlobCodec::getFileExtension() const
{
    return QStringLiteral(".bin");
}

}
