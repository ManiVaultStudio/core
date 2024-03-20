// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "VariantMap.h"

#ifdef _DEBUG
    #define VARIANT_MAP_VERBOSE
#endif

namespace mv {

const QString VariantMap::serializationVersionKey = "SerializationVersion";

VariantMap::VariantMap()
{
    initialize();
}

VariantMap::VariantMap(std::initializer_list<std::pair<QString, QVariant>> list) :
    _variantMap(list)
{
    initialize();
}

VariantMap::VariantMap(const std::map<QString, QVariant>& other) :
    _variantMap(other)
{
    initialize();
}

VariantMap::VariantMap(std::map<QString, QVariant>&& other) :
    _variantMap(other)
{
    initialize();
}

VariantMap::VariantMap(const QVariantMap& other) :
    _variantMap(other)
{
    initialize();
}

VariantMap::VariantMap(QVariantMap&& other) :
    _variantMap(other)
{
    initialize();
}

std::uint32_t VariantMap::getVersion() const
{
    return _version;
}

const QVariantMap& VariantMap::getVariantMap() const
{
    return _variantMap;
}

void VariantMap::initialize()
{
    //_variantMap["SerializationVersion"] = QVariant::fromValue(_version);
}

QVariant VariantMap::converted() const
{
    for (const auto& variant : _variantMap.values()) {
        qDebug() << variant.typeName();
        if (variant.typeName() == "QStringList")
            qDebug() << "Found a string list!";
    }

    return _variantMap;
}

}
