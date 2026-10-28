// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "PointsStorageBackendTypes.h"

#include <QMap>

namespace mv::pointdata::storage
{

namespace
{

const QMap<StorageBackendType, QString> storageBackendTypeNames = {
    { StorageBackendType::Eager,  QStringLiteral("Eager") },
    { StorageBackendType::Tiled,  QStringLiteral("Tiled") },
    { StorageBackendType::Sparse, QStringLiteral("Sparse") }
};

}

QString getStorageBackendTypeName(const StorageBackendType type)
{
    return storageBackendTypeNames.value(type);
}

std::optional<StorageBackendType> getStorageBackendType(const QString& typeName)
{
    for (auto iterator = storageBackendTypeNames.cbegin(); iterator != storageBackendTypeNames.cend(); ++iterator)
    {
        if (iterator.value() == typeName)
            return iterator.key();
    }

    return std::nullopt;
}

}
