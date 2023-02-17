#pragma once

#include "InlineGroupAction.h"
#include "IntegralAction.h"

namespace hdps::gui {

/**
 * Version action class
 *
 * Action class for storing a major and minor version
 *
 * @author Thomas Kroes
 */
class VersionAction final : public InlineGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent and \p project
     * @param parent Pointer to parent object
     */
    VersionAction(QObject* parent);

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;

public: // Serialization

    /**
     * Load project from variant
     * @param Variant representation of the project
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save project to variant
     * @return Variant representation of the project
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    const gui::IntegralAction& getMajorVersionAction() const { return _majorVersionAction; }
    const gui::IntegralAction& getMinorVersionAction() const { return _minorVersionAction; }

    gui::IntegralAction& getMajorVersionAction() { return _majorVersionAction; }
    gui::IntegralAction& getMinorVersionAction() { return _minorVersionAction; }

private:
    IntegralAction  _majorVersionAction;            /** Action for storing the major version */
    IntegralAction  _minorVersionAction;            /** Action for storing the minor version */
};

}
