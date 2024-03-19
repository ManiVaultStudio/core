// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

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
    
    QVariantMap::iterator insert(QVariantMap::const_iterator pos, const QString& key, const QVariant& value) {
        return _variantMap.insert(pos, key, value);
    }
    
    void insert(const QVariantMap& map) {
        _variantMap.insert(map);
    }
    
    void insert(QVariantMap&& map) {
        _variantMap.insert(map);
    }

    QList<QString> keys() const {
        return _variantMap.keys();
        //auto allKeysExceptSerializationVersion = _variantMap.keys();

        //allKeysExceptSerializationVersion.takeFirst();

        //return allKeysExceptSerializationVersion;
    }

    QList<QVariant> values() const {
        return _variantMap.values();
        //auto allValuesExceptSerializationVersion = _variantMap.values();

        //allValuesExceptSerializationVersion.takeFirst();

        //return allValuesExceptSerializationVersion;
    }

    QVariant& operator [](const QString& key) {
        Q_ASSERT(key != serializationVersionKey);

        return _variantMap[key];
    }

    QVariant operator [](const QString& key) const {
        Q_ASSERT(key != serializationVersionKey);

        return _variantMap[key];
    }

    VariantMap operator =(const VariantMap& other) {
        _variantMap = other.getVariantMap();

        return *this;
    }

    VariantMap operator =(const QVariantMap& variantMap) {
        _variantMap = variantMap;

        return *this;
    }

    operator QVariant () const {
        return _variantMap;
    }

    operator QVariantMap () const {
        return _variantMap;
    }

protected:

    const QVariantMap& getVariantMap() const;

private:

    /** Add serialization version etc. */
    void initialize();

private:
    const std::uint32_t _version{1};    /** Variant map serialization version */
    QVariantMap         _variantMap;    /** The actual variant map container */

    static const QString serializationVersionKey;

    friend class mv::util::Serializable;
};

}
