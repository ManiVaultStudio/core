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
 * @author Thomas Kroes
 */
class VariantMap final
{
public:

    

public:
    VariantMap();
    VariantMap(std::initializer_list<std::pair<QString, QVariant>> list);
    VariantMap(const std::map<QString, QVariant>& other);
    VariantMap(std::map<QString, QVariant>&& other);
    VariantMap(const QVariantMap& other);
    VariantMap(QVariantMap&& other);

    std::uint32_t getVersion() const;

    bool contains(const QString& key) const;

    QVariantMap::size_type count(const QString& key) const;
    
    QVariantMap::size_type count() const;

    QVariantMap::iterator insert(const QString& key, const QVariant& value);

    //QVariantMap::iterator insert(QVariantMap::const_iterator pos, const QString& key, const QVariant& value) {
    //    return _variantMap.insert(pos, key, value);
    //}

    void insert(const QVariantMap& map);

    void insert(QVariantMap&& map);

    QList<QString> keys() const;

    QList<QVariant> values() const;

    VariantProxy& operator[](const QString& key);

    QVariant operator[](const QString& key) const;

    VariantMap operator=(const VariantMap& other);

    VariantMap operator=(const QVariantMap& variantMap);

    operator QVariant () const;

    operator QVariantMap () const;

    QVariant toVariant() const;

protected:

    QVariantMap getVariantMap() const;

private:

    QVariant converted() const;

    /** Add serialization version etc. */
    void initialize();

private:
    const std::uint32_t     _version{1};    /** Variant map serialization version */
    QVariantMap             _variantMap;    /** The actual variant map container */
    VariantProxy            _variantProxy;  /**  */

    static const QString serializationVersionKey;

    friend class mv::util::Serializable;
};

}
