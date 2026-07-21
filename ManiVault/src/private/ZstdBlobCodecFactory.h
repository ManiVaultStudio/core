// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ZstdCodecSettingsAction.h"


#include <util/BlobCodec.h>
#include <util/BlobCodecFactory.h>

/**
 * @brief Factory interface for creating Zstandard codec instances.
 *
 * This factory is expected to live in the main thread
 *
 * @author Thomas Kroes
 */
class ZstdBlobCodecFactory final : public mv::util::BlobCodecFactory
{
public:

    ZstdBlobCodecFactory(QObject* parent = nullptr);

    ~ZstdBlobCodecFactory();

	mv::util::BlobCodec::Type type() const override;

	QString key() const override;

	QString displayName() const override;

    /**
     * Get default codec settings action for this codec (returns nullptr if no settings are needed)
     * @return Default codec settings action for this codec (returns nullptr if no settings are needed)
     */
    const mv::gui::CodecSettingsAction* getDefaultCodecSettingsAction() const override;

    mv::gui::CodecSettingsAction* createCodecSettingsAction(QObject* parent) const override;

protected:

    
    std::shared_ptr<mv::util::BlobCodec> createCodec(QObject* parent, mv::gui::CodecSettingsAction* codecSettingsAction = nullptr) const override;

private:
    ZstdCodecSettingsAction     _defaultSettingsAction;     /** Default codec settings action for this codec */

    friend class mv::util::CodecRegistry;
};
