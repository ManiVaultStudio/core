// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/GroupAction.h"
#include "actions/ToggleAction.h"
#include "actions/IntegralAction.h"

namespace mv {

/**
 * Project compression class 
 *
 * Group action class for compression parameters
 * 
 * @author Thomas Kroes
 */
class ProjectCompressionAction final : public gui::GroupAction {
public:

    /**
     * Constructs from \p parent object
     * @param parent Pointer to parent object
     */
    ProjectCompressionAction(QObject* parent = nullptr);

public: // Serialization

    /**
     * Load compression action from variant
     * @param Variant representation of the compression action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save compression action to variant
     * @return Variant representation of the compression action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    gui::ToggleAction& getEnabledAction() { return _enabledAction; }
    gui::IntegralAction& getLevelAction() { return _levelAction; }

private:
    gui::ToggleAction       _enabledAction;     /** Action to enable/disable project file compression */
    gui::IntegralAction     _levelAction;       /** Action to control the amount of project file compression */

public:
    static constexpr bool           DEFAULT_ENABLE_COMPRESSION  = false;    /** No compression by default */
    static constexpr std::uint32_t  DEFAULT_COMPRESSION_LEVEL   = 2;        /** Default compression level*/
};

}
