#pragma once

#include "actions/WidgetAction.h"

namespace hdps {

/**
 * Project class
 *
 * TODO: Write description
 *
 * @author Thomas Kroes
 */
class Project : public gui::WidgetAction
{
    Q_OBJECT

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

}
