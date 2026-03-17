// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/HorizontalGroupAction.h"
#include "actions/ToggleAction.h"
#include "actions/OptionAction.h"

namespace mv {

/**
 * Project compression class 
 *
 * Group action class for compression parameters
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT ProjectCompressionAction final : public gui::HorizontalGroupAction {
protected:

    /** Maps codec display names to codec instances */
    using CodecInstanceMap = std::unordered_map<QString,std::shared_ptr<util::BlobCodec>>;

public:

    /**
     * Constructs from \p parent object
     * @param parent Pointer to parent object
     */
    ProjectCompressionAction(QObject* parent = nullptr);

    std::shared_ptr<util::BlobCodec> getCodec() const;

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

private:
    gui::ToggleAction   _enabledAction;         /** Action to enable/disable project file compression */
    gui::OptionAction   _codecTypeAction;       /** Blob codec type action for project serialization */
    CodecInstanceMap    _codecInstanceMap;      /** Maps codec display names to codec instances, used for serialization */

public:
    static constexpr bool   DEFAULT_ENABLE_COMPRESSION  = false;    /** No compression by default */
};

}
