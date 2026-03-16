// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/GroupAction.h"
#include "actions/ToggleAction.h"
#include "actions/OptionAction.h"
#include "actions/CodecSettingsAction.h"

namespace mv {

/**
 * Project compression class 
 *
 * Group action class for compression parameters
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT ProjectCompressionAction final : public gui::GroupAction {
public:

    /**
     * Constructs from \p parent object
     * @param parent Pointer to parent object
     */
    ProjectCompressionAction(QObject* parent = nullptr);

    std::unique_ptr<util::BlobCodec> createCodec() const;

public: // Serialization

    /**
     * Load compression action from variant
     * @param variantMap Variant representation of the compression action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save compression action to variant
     * @return Variant representation of the compression action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    gui::ToggleAction& getEnabledAction() { return _enabledAction; }
    gui::OptionAction& getCodecTypeAction() { return _codecTypeAction; }


    //gui::HorizontalGroupAction* getCodecEditAction();

private:
    gui::ToggleAction           _enabledAction;         /** Action to enable/disable project file compression */
    gui::OptionAction           _codecTypeAction;       /** Blob codec type action for project serialization */
    gui::CodecSettingsAction*   _codecSettingsAction;   /** Codec settings action for project serialization */

public:
    static constexpr bool       DEFAULT_ENABLE_COMPRESSION  = false;    /** No compression by default */
};

}
