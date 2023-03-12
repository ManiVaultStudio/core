#pragma once

#include "HorizontalGroupAction.h"
#include "IntegralAction.h"
#include "StringAction.h"

#include <QCompleter>

namespace hdps::gui {

/**
 * Version action class
 *
 * Action class for storing a major and minor version
 *
 * @author Thomas Kroes
 */
class VersionAction final : public HorizontalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent and \p project
     * @param parent Pointer to parent object
     */
    VersionAction(QObject* parent);

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

    const gui::IntegralAction& getMajorAction() const { return _majorAction; }
    const gui::IntegralAction& getMinorAction() const { return _minorAction; }
    const gui::StringAction& getSuffixAction() const { return _suffixAction; }
    const gui::StringAction& getVersionStringAction() const { return _versionStringAction; }

    gui::IntegralAction& getMajorAction() { return _majorAction; }
    gui::IntegralAction& getMinorAction() { return _minorAction; }
    gui::StringAction& getSuffixAction() { return _suffixAction; }
    gui::StringAction& getVersionStringAction() { return _versionStringAction; }

private:
    IntegralAction  _majorAction;           /** Action for storing the major version */
    IntegralAction  _minorAction;           /** Action for storing the minor version */
    StringAction    _suffixAction;          /** Action which stores the version suffix (e.g. alpha or beta) */
    StringAction    _versionStringAction;   /** Action which stores the string representation of the version */
    QCompleter      _suffixCompleter;       /** Completer for version suffix */
};

}
