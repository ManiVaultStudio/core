#pragma once

#include "Serialization.h"

#include <QString>
#include <QJsonDocument>

namespace hdps {

/**
 * @author Thomas Kroes
 */
class Serializable
{
public:

    /**
     * Constructor
     * @param name Serialization key
     */
    Serializable(const QString& name = "");

    /**
     * Get serialization name
     * @return Serialization name
     */
    virtual QString getSerializationName() const;

    /**
     * Load from variant map
     * @param variantMap Variant map
     */
    virtual void fromVariantMap(const QVariantMap& variantMap);

    /**
     * Save to variant map
     * @return Variant map
     */
    virtual QVariantMap toVariantMap() const;

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

protected:

    /**
     * Load serializable object from variant map
     * @param serializable Pointer to serializable object
     * @param Variant map
     */
    static void fromVariantMap(Serializable* serializable, const QVariantMap& variantMap);

    /**
     * Save serializable object to variant map
     * @param serializable Pointer to serializable object
     * @return Variant map
     */
    static QVariantMap toVariantMap(const Serializable* serializable);

private:
    QString     _name;      /** Serialization name */
};

}
