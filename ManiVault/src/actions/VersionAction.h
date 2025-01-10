// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "HorizontalGroupAction.h"
#include "IntegralAction.h"
#include "StringAction.h"

#include "util/Version.h"

#include <QCompleter>

namespace mv::gui {

/**
 * Version action class
 *
 * For interaction with a semantic version
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT VersionAction final : public HorizontalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent and \p project
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE VersionAction(QObject* parent, const QString& title);

    /**
     * Get major version number
     * @return Major version number
     */
    std::int32_t getMajor() const;

    /**
     * Get minor version number
     * @return Minor version number
     */
    std::int32_t getMinor() const;

    /**
     * Get patch version number
     * @return Patch version number
     */
    std::int32_t getPatch() const;

    /**
     * Get version suffix string
     * @return Version suffix string
     */
    QString getSuffix() const;

    /**
     * Get version
     * @return Version
     */
    util::Version getVersion() const;

    /**
     * Set version to \p version
     * @param version Version
     */
    void setVersion(const util::Version& version);

public: // Serialization

    /**
     * Load version action from variant
     * @param variantMap Variant representation of the project
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save project to variant
     * @return Variant representation of the project
     */
    QVariantMap toVariantMap() const override;

signals:

    /**
     * Signals that the version changed to \p version
     * @param version Changed version
     */
    void versionChanged(const util::Version& version);

public: // Action getters

    const IntegralAction& getMajorAction() const { return _majorAction; }
    const IntegralAction& getMinorAction() const { return _minorAction; }
    const IntegralAction& getPatchAction() const { return _patchAction; }
    const StringAction& getSuffixAction() const { return _suffixAction; }
    const StringAction& getVersionStringAction() const { return _versionStringAction; }

    IntegralAction& getMajorAction() { return _majorAction; }
    IntegralAction& getMinorAction() { return _minorAction; }
    IntegralAction& getPatchAction() { return _patchAction; }
    StringAction& getSuffixAction() { return _suffixAction; }
    StringAction& getVersionStringAction() { return _versionStringAction; }

private:
    IntegralAction  _majorAction;           /** Action for storing the major version */
    IntegralAction  _minorAction;           /** Action for storing the minor version */
    IntegralAction  _patchAction;           /** Action for storing the patch version */
    StringAction    _suffixAction;          /** Action which stores the version suffix (e.g. alpha or beta) */
    StringAction    _versionStringAction;   /** Action which stores the string representation of the version */
    QCompleter      _suffixCompleter;       /** Completer for version suffix */
};

}

Q_DECLARE_METATYPE(mv::gui::VersionAction)

inline const auto versionActionMetaTypeId = qRegisterMetaType<mv::gui::VersionAction*>("mv::gui::VersionAction");