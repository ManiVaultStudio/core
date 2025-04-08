// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "Serialization.h"

#include <QString>
#include <QJsonDocument>

namespace mv::util {

/**
 * Serializable class
 * 
 * Serialize objects using Qt variants.
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT Serializable
{
public:

    /** Determines the state of the serializable object */
    enum class State {
        Idle,       /** The serializable object is not being read or written */
        Reading,    /** The serializable object is being read */
        Writing     /** The serializable object is being written */
    };

public:

    /**
     * Construct with serialization name
     * @param serializationName Serialization name
     */
    Serializable(const QString& serializationName = "");

    /** Destructor */
    virtual ~Serializable() { }

    /**
     * Get globally unique identifier, possibly \p truncated
     * @param truncated Whether to only return the first six characters of the ID
     * @return Globally unique identifier of the serializable object
     */
    QString getId(bool truncated = false) const;

    /**
     * Set globally unique identifier (only use this function when strictly necessary and when the ramifications are understood, undefined behaviour might happen otherwise)
     * @param id Globally unique identifier of the serializable object
     */
    void setId(const QString& id);

    /**
     * Get serialization name
     * @return Serialization name
     */
    QString getSerializationName() const;

    /**
     * Set serialization name to \p name
     * @param serializationName Serialization name
     */
    void setSerializationName(const QString& serializationName);

    /**
     * Load from variant map
     * @param variantMap Variant map
     */
    virtual void fromVariantMap(const QVariantMap& variantMap);

    /**
     * Load from variant map located in \p parentVariantMap at the serialization name
     * @param parentVariantMap Parent variant map
     */
    virtual void fromParentVariantMap(const QVariantMap& parentVariantMap);

    /**
     * Save to variant map
     * @return Variant map
     */
    virtual QVariantMap toVariantMap() const;

    /**
     * Save into \p variantMap
     * @param variantMap Variant map
     */
    void insertIntoVariantMap(QVariantMap& variantMap) const;

    /**
     * Load widget action from JSON document
     * @param jsonDocument The JSON document
     */
    void fromJsonDocument(const QJsonDocument& jsonDocument);

    /**
     * Save widget action to JSON document
     * @return JSON document
     */
    QJsonDocument toJsonDocument() const;

    /**
     * Load from JSON file
     * @param filePath Path to the JSON file (if none/invalid a file open dialog is automatically opened)
     */
    void fromJsonFile(const QString& filePath = "");

    /**
     * Save to JSON file
     * @param filePath Path to the JSON file (if none/invalid a file save dialog is automatically opened)
     */
    void toJsonFile(const QString& filePath = "") const;

    /** Assigns a fresh new identifier to the serializable object */
    void makeUnique();

    /**
     * Creates a new globally unique identifier for a serializable object
     * @return Globally unique identifier
     */
    static QString createId();

protected: // Serialization

    /**
     * Load serializable object from variant map
     * @param serializable Pointer to serializable object
     * @param variantMap The Variant map
     */
    static void fromVariantMap(Serializable* serializable, const QVariantMap& variantMap);

    /**
     * Load from variant map
     * @param serializable Reference to serializable object
     * @param variantMap Variant map
     * @param key Variant map key
     */
    static void fromVariantMap(Serializable& serializable, const QVariantMap& variantMap, const QString& key);

    /**
     * Save serializable object to variant map
     * @param serializable Pointer to serializable object
     * @return Variant map
     */
    static QVariantMap toVariantMap(const Serializable* serializable);

    /**
     * Save \p serializable object in \p variantMap with \p key
     * @param serializable Reference to serializable object
     * @param variantMap Variant map
     * @param key Variant map key
     */
    static void insertIntoVariantMap(const Serializable& serializable, QVariantMap& variantMap, const QString& key);

    /**
     * Save \p serializable object in \p variantMap
     * @param serializable Reference to serializable object
     * @param variantMap Variant map
     */
    static void insertIntoVariantMap(const Serializable& serializable, QVariantMap& variantMap);

public: // Operators

    /**
     * Equality operator
     * @param rhs Right-hand-side operator
     */
    bool operator == (const Serializable& rhs) const {
        return rhs.getId() == getId();
    }
    
    /**
     * Inequality operator
     * @param rhs Right-hand-side operator
     */
    bool operator != (const Serializable& rhs) const {
        return rhs.getId() != getId();
    }

private:
    QString     _id;                    /** Globally unique identifier of the serializable object */
    QString     _serializationName;     /** Serialization name */
};

}

