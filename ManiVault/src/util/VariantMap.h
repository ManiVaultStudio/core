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

//class _VariantMap : public QMap<QString, Variant> {
//public:
//    _VariantMap();
//    _VariantMap(std::initializer_list<std::pair<QString, QVariant>> list);
//    _VariantMap(const std::map<QString, QVariant>& other);
//    _VariantMap(std::map<QString, QVariant>&& other);
//    _VariantMap(const QVariantMap& other);
//    _VariantMap(QVariantMap&& other);
//};

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

    bool contains(const QString& key) const {
        return _variantMap.contains(key);
    }

    QVariantMap::size_type count(const QString& key) const {
        return _variantMap.count(key);
    }
    
    QVariantMap::size_type count() const {
        return _variantMap.count();
    }

    QVariantMap::iterator insert(const QString& key, const QVariant& value) {
        return _variantMap.insert(key, value);
    }

    //QVariantMap::iterator insert(QVariantMap::const_iterator pos, const QString& key, const QVariant& value) {
    //    return _variantMap.insert(pos, key, value);
    //}

    void insert(const QVariantMap& map) {
        _variantMap.insert(map);
    }

    void insert(QVariantMap&& map) {
        _variantMap.insert(map);
    }

    QList<QString> keys() const {
        return _variantMap.keys();
    }

    QList<QVariant> values() const {
        QList<QVariant> variantValues;

        for (const auto& value : _variantMap.values())
            variantValues << value;

        return variantValues;
    }

    VariantProxy& operator[](const QString& key) {
        _variantProxy.setKey(key);

        return _variantProxy;
    }

    QVariant operator[](const QString& key) const {
        return VariantProxy(key, const_cast<QVariantMap&>(_variantMap)).get();
    }

    VariantMap operator=(const VariantMap& other) {
        _variantMap = other._variantMap;

        return *this;
    }

    VariantMap operator=(const QVariantMap& variantMap) {
        for (const auto& key : variantMap.keys())
            _variantMap[key] = variantMap[key];

        return *this;
    }

    operator QVariant () const {
        return converted();
    }

    operator QVariantMap () const {
        return getVariantMap();
    }

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
