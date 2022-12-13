#pragma once

#include <util/Serializable.h>

/**
 * Project class
 *
 * TODO: Write description
 *
 * @author Thomas Kroes
 */
class Project final : public QObject, public hdps::util::Serializable
{
public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;
};
