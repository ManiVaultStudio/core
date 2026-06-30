// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include "BlobCodec.h"

#include <QString>

namespace mv::gui
{
	class CodecSettingsAction;
}

namespace mv::util {

class BlobCodecSettings;

/**
 * @brief Factory interface for creating codec instances.
 * 
 * This factory is expected to live in the main thread
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT BlobCodecFactory : public QObject
{
public:

    BlobCodecFactory(QObject* parent);
    ~BlobCodecFactory() override = default;

    virtual BlobCodec::Type type() const = 0;
    virtual QString key() const = 0;
    virtual QString displayName() const = 0;

    /**
     * Get default codec settings action for this codec (returns nullptr if no settings are needed)
     * @return Default codec settings action for this codec (returns nullptr if no settings are needed)
     */
    virtual const gui::CodecSettingsAction* getDefaultCodecSettingsAction() const = 0;

    virtual gui::CodecSettingsAction* createCodecSettingsAction(QObject* parent) const = 0;

protected:

    /**
     * Create a new codec instance with the provided codec settings action.
     * If no codec settings action is provided, the default codec settings action is used (if available).
     *
     * @param parent Pointer to parent object for the created codec instance
     * @param codecSettingsAction Codec settings action for the created codec instance (optional)
     * @return A new codec instance
     */
    virtual std::shared_ptr<BlobCodec> createCodec(QObject* parent, gui::CodecSettingsAction* codecSettingsAction = nullptr) const = 0;

    friend class CodecRegistry;
};

}
