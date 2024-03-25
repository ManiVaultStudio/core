// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "VariantMap.h"

#ifdef _DEBUG
    #define VARIANT_MAP_VERBOSE
#endif

namespace mv {

const QString VariantMap::serializationVersionKey = "SerializationVersion";

VariantMap::VariantMap() :
    _version(),
    _variantMap(),
    _variantProxy(this)
{
    initialize();
}

VariantMap::VariantMap(const QVariantMap& variantMap) :
    _version(),
    _variantMap(variantMap),
    _variantProxy(this)
{
}

VariantMap::VariantMap(std::initializer_list<std::pair<QString, QVariant>> list) :
    _version(),
    _variantMap(),
    _variantProxy(this)
{
    for (const auto& [key, value] : list)
        (*this)[key] = value;

    initialize();
}

std::uint32_t VariantMap::getVersion() const
{
    return _version;
}

bool VariantMap::contains(const QString& key) const
{
    return _variantMap.contains(key);
}

QVariantMap::size_type VariantMap::count() const
{
    return _variantMap.count();
}

QVariantMap::size_type VariantMap::count(const QString& key) const
{
    return _variantMap.count(key);
}

QVariantMap::iterator VariantMap::insert(const QString& key, const QVariant& value)
{
    return _variantMap.insert(key, value);
}

void VariantMap::insert(const QVariantMap& map)
{
    _variantMap.insert(map);
}

QList<QString> VariantMap::keys() const
{
    return _variantMap.keys();
}

QList<QVariant> VariantMap::values() const
{
    return _variantMap.values();
}

VariantProxy& VariantMap::operator[](const QString& key)
{
    _variantProxy.setKey(key);

    return _variantProxy;
}

QVariant VariantMap::operator[](const QString& key) const
{
    return VariantProxy(key, const_cast<VariantMap*>(this)).get();
}

VariantMap VariantMap::operator=(const VariantMap& other)
{
    _variantMap     = other._variantMap;
    _variantProxy   = other._variantProxy;

    return *this;
}

VariantMap VariantMap::operator=(const QVariantMap& variantMap)
{
    for (const auto& key : variantMap.keys())
        _variantMap[key] = variantMap[key];

    return *this;
}

QVariantMap& VariantMap::get()
{
    return _variantMap;
}

const QVariantMap& VariantMap::get() const
{
    return _variantMap;
}

void VariantMap::initialize()
{
    //_variantMap["SerializationVersion"] = QVariant::fromValue(_version);
}

QVariant VariantMap::toVariant() const
{
    return _variantMap;
}

VariantMap::operator QVariant() const
{
    return get();
}

VariantMap::operator QVariantMap() const
{
    return get();
}

}
