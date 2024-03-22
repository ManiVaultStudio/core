// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "VariantProxy.h"

#ifdef _DEBUG
    #define VARIANT_PROXY_VERBOSE
#endif

namespace mv {

VariantProxy::VariantProxy(const QString& key, QVariantMap& variantMap) :
    _key(key),
    _variantMap(variantMap)
{
}

VariantProxy::VariantProxy(QVariantMap& variantMap) :
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

}
