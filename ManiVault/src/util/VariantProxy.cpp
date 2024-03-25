// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "VariantProxy.h"
#include "VariantMap.h"

#ifdef _DEBUG
    #define VARIANT_PROXY_VERBOSE
#endif

namespace mv {

VariantProxy::VariantProxy(const QString& key, VariantMap* variantMap) :
    _key(key),
    _variantMap(variantMap)
{
}

VariantProxy::VariantProxy(VariantMap* variantMap) :
    _key(),
    _variantMap(variantMap)
{
}

VariantProxy::VariantProxy(const VariantProxy& other) :
    _key(),
    _variantMap(other._variantMap)
{
    *this = other;
}

void VariantProxy::setKey(const QString& key)
{
    _key = key;
}

VariantProxy& VariantProxy::operator=(const VariantProxy& rhs)
{
    _key        = rhs._key;
    _variantMap = rhs._variantMap;

    return *this;
}

VariantProxy VariantProxy::operator=(const VariantMap& rhs)
{
    Q_ASSERT(!_variantMap->get().contains(_key));

    _variantMap->get()[_key] = rhs.toVariant();

    return *this;
}

VariantProxy VariantProxy::operator=(const QVariant& rhs)
{
    Q_ASSERT(!_variantMap->get().contains(_key));

#ifdef VARIANT_PROXY_VERBOSE
    qDebug() << __FUNCTION__ << _key;
#endif

    _variantMap->get()[_key] = rhs;

    return *this;
}

VariantProxy VariantProxy::operator=(const QVariantList& rhs)
{
    Q_ASSERT(!_variantMap->get().contains(_key));

#ifdef VARIANT_PROXY_VERBOSE
    qDebug() << __FUNCTION__ << _key;
#endif

    _variantMap->get()[_key] = rhs;

    return *this;
}

VariantProxy VariantProxy::operator=(const QVariantMap& rhs)
{
    Q_ASSERT(!_variantMap->get().contains(_key));

#ifdef VARIANT_PROXY_VERBOSE
    qDebug() << __FUNCTION__ << _key;
#endif

    _variantMap->get()[_key] = rhs;

    return *this;
}

QVariant VariantProxy::get() const
{
    return _variantMap->get()[_key];
}

}
