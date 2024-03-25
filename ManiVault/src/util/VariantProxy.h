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
 * a reference to a ManiVault \p VariantMap to modify it.
 * 
 * @author Thomas Kroes & Jeroen Eggermont
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
     * Initialize with current \p key and pointer to owning \p variantMap
     * @param key Key of the \p QVariant to modify in VariantProxy#_variantMap
     * @param variantMap Pointer to the owning ManiVault VariantMap
     */
    VariantProxy(const QString& key, VariantMap* variantMap);

    /**
     * Initialize with pointer to owning ManiVault \p variantMap
     * @param variantMap Pointer to the owning ManiVault VariantMap
     */
    VariantProxy(VariantMap* variantMap);

    /**
     * Sets current key to \p key
     * @param key Key of the \p QVariant to modify in VariantMap#_variantMap
     */
    void setKey(const QString& key);

public:

    /**
     * Assign \p rhs ManiVault variant proxy
     * @param rhs Other ManiVault variant proxy
     */
    VariantProxy& operator=(const VariantProxy& rhs);

    /**
     * Assign \p rhs ManiVault variant map
     * @param rhs Other ManiVault variant map
     */
    VariantProxy operator=(const VariantMap& rhs);

    /**
     * Assign \p rhs QVariant
     * @param rhs Other QVariant
     */
    VariantProxy operator=(const QVariant& rhs);

    /**
     * Assign \p rhs QVariantList
     * @param rhs Other QVariantList
     */
    VariantProxy operator=(const QVariantList& rhs);

    /**
     * Assign \p rhs QVariantMap
     * @param rhs Other QVariantMap
     */
    VariantProxy operator=(const QVariantMap& rhs);

protected:

    /**
     * Get the proxy as a QVariant
     * @return Proxy as QVariant
     */
    QVariant get() const;

private:
    QString         _key;           /** Key of the \p QVariant to modify in VariantMap#_variantMap */
    VariantMap*     _variantMap;    /** Pointer to owning ManiVault variant map */

    friend class VariantMap;
};

}
