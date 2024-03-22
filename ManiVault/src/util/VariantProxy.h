// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QVariantMap>

#ifdef _DEBUG
    #define VARIANT_PROXY_VERBOSE
#endif

namespace mv {

// Forward declare the ManiVault variant map because it is a friend of the VariantProxy class
class VariantMap;

/**
 * Variant proxy class
 * 
 * This class acts a proxy for modifying QVariant data in a \p QVariantMap located
 * elsewhere. It does not contain the actual QVariant data, it merely uses a key and 
 * a reference to a \p QVariantMap to modify it.
 * 
 * @author Thomas Kroes
 */
class VariantProxy
{
protected:

    /**
     * Copy constructor
     * @param other Other variant proxy to copy
     */
    VariantProxy(const VariantProxy& other);

    /**
     * Initialize with current \p key and reference to \p variantMap
     * @param key Key of the \p QVariant to modify in VariantProxy#_variantMap
     * @param variantMap Reference to the external \p QVariantMap that we want to modify
     */
    VariantProxy(const QString& key, QVariantMap& variantMap);

    /**
     * Initialize with reference to \p variantMap
     * @param variantMap Reference to the external \p QVariantMap that we want to modify
     */
    VariantProxy(QVariantMap& variantMap);

    /**
     * Sets current key to \p key
     * @param key Key of the \p QVariant to modify in VariantProxy#_variantMap
     */
    void setKey(const QString& key);

public:

    /**
     * Assign \p rhs ManiVault variant proxy
     * @param rhs Other variant proxy
     */
    VariantProxy operator=(const VariantProxy& rhs) {
        _key        = rhs._key;
        _variantMap = rhs._variantMap;

        return *this;
    }

    /**
     * Assign \p rhs ManiVault variant map
     * @param rhs Other variant map
     */
    VariantProxy operator=(const VariantMap& rhs) {
        Q_ASSERT(!_variantMap.contains(_key));

        _variantMap[_key] = rhs.toVariant();

        return *this;
    }

    /**
     * Assign \p rhs QVariant
     * @param rhs Other QVariant
     */
    VariantProxy operator=(const QVariant& rhs) {
        Q_ASSERT(!_variantMap.contains(_key));

#ifdef VARIANT_PROXY_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        _variantMap[_key] = rhs;

        return *this;
    }

    /**
     * Assign \p rhs QVariantList
     * @param rhs Other QVariantList
     */
    VariantProxy operator=(const QVariantList& rhs) {
        Q_ASSERT(!_variantMap.contains(_key));

        _variantMap[_key] = rhs;

        return *this;
    }

    /**
     * Assign \p rhs QVariantMap
     * @param rhs Other QVariantMap
     */
    VariantProxy operator=(const QVariantMap& rhs) {
        Q_ASSERT(!_variantMap.contains(_key));

        _variantMap[_key] = rhs;

        return *this;
    }

    /**
     * Assign \p rhs float
     * @param rhs Other float
     */
    //VariantProxy operator =(const float& rhs) {
    //    Q_ASSERT(!_variantMap.contains(_key));

    //    _variantMap[_key] = rhs;

    //    return *this;
    //}

protected:

    QVariant get() const {
        return _variantMap[_key];
    }

private:
    QString         _key;           /** Key of the \p QVariant to modify in VariantProxy#_variantMap */
    QVariantMap&    _variantMap;    /** Reference to the external \p QVariantMap that we want to modify */

    friend class mv::VariantMap;
};

}
