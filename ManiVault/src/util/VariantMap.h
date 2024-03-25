// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "VariantProxy.h"

#include <QVariantMap>

namespace mv::util {
    class Serializable;
}

namespace mv {

/**
 * TODO
 * 
 * @author Thomas Kroes & Jeroen Eggermont
 */
class VariantMap final
{
public:
    VariantMap();
    VariantMap(const QVariantMap& variantMap);
    VariantMap(std::initializer_list<std::pair<QString, QVariant>> list);

    std::uint32_t getVersion() const;

    bool contains(const QString& key) const;

    QVariantMap::size_type count(const QString& key) const;
    QVariantMap::size_type count() const;

public: // Insertion

    QVariantMap::iterator insert(const QString& key, const QVariant& value);
    void insert(const QVariantMap& map);

public: // keys, values

    QList<QString> keys() const;
    QList<QVariant> values() const;

public:

    VariantProxy& operator[](const QString& key);

    QVariant operator[](const QString& key) const;


    VariantMap operator=(const VariantMap& other);
    VariantMap operator=(const QVariantMap& variantMap);

public: // Conversion operators

    operator QVariant () const;
    operator QVariantMap () const;

    QVariant toVariant() const;

protected:

    QVariantMap& get();
    const QVariantMap& get() const;

private:

    /** Add serialization version etc. */
    void initialize();

private:
    const std::uint32_t     _version{1};    /** Variant map serialization version */
    QVariantMap             _variantMap;    /** The actual variant map container */
    VariantProxy            _variantProxy;  /**  */

    static const QString serializationVersionKey;

    friend class VariantProxy;
    friend class mv::util::Serializable;
};

}
