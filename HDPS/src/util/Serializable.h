#pragma once

#include "Serialization.h"

#include <QString>
#include <QJsonDocument>

namespace hdps::util {

/**
 * Serializable class
 * 
 * Serialize objects using Qt variants.
 * 
 * @author Thomas Kroes
 */
class Serializable
{
public:

    /**
     * Construct with serialization name
     * @param serializationName Serialization name
     */
    Serializable(const QString& serializationName = "");

    /**
     * Get id
     * @return Globally unique identifier of the serializable object
     */
    virtual QString getId() const;

    /**
     * Get serialization name
     * @return Serialization name
     */
    virtual QString getSerializationName() const final;

    /**
     * Set serialization name to \p name
     * @param serializationName Serialization name
     */
    virtual void setSerializationName(const QString& serializationName) final;

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
    virtual void insertIntoVariantMap(QVariantMap& variantMap) const final;

    /**
     * Load widget action from JSON document
     * @param JSON document
     */
    virtual void fromJsonDocument(const QJsonDocument& jsonDocument) final;

    /**
     * Save widget action to JSON document
     * @return JSON document
     */
    virtual QJsonDocument toJsonDocument() const final;

    /**
     * Load from JSON file
     * @param filePath Path to the JSON file (if none/invalid a file open dialog is automatically opened)
     */
    virtual void fromJsonFile(const QString& filePath = "") final;

    /**
     * Save to JSON file
     * @param filePath Path to the JSON file (if none/invalid a file save dialog is automatically opened)
     */
    virtual void toJsonFile(const QString& filePath = "") final;

    /** Assigns a fresh new identifier to the serializable object */
    virtual void makeUnique() final;

    /**
     * Creates a new globally unique identifier for a serializable object
     * @return Globally unique identifier
     */
    static QString createId();

protected:

    /**
     * Load serializable object from variant map
     * @param serializable Pointer to serializable object
     * @param Variant map
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

private:
    QString     _id;                    /** Globally unique identifier of the serializable object */
    QString     _serializationName;     /** Serialization name */
};

}

