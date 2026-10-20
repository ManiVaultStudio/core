// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ZstdCodecSettingsAction.h"

#include <util/BlobCodec.h>
#include <util/BlobCodecFactory.h>

/**
 * @brief Creates Zstandard blob codec instances.
 *
 * This factory owns the default settings action and creates Zstandard-compressed
 * blob codec instances.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class ZstdBlobCodecFactory final : public mv::util::BlobCodecFactory
{
public:

    /**
     * @brief Constructs a Zstandard codec factory.
     * @param parent Optional parent object.
     */
    ZstdBlobCodecFactory(QObject* parent = nullptr);

    /**
     * @brief Destroys the codec factory.
     */
    ~ZstdBlobCodecFactory();

    /**
     * @brief Returns the codec type.
     * @return Blob codec type.
     */
    [[nodiscard]] mv::util::BlobCodec::Type type() const override;

    /**
     * @brief Returns the codec registry key.
     * @return Unique codec key.
     */
    [[nodiscard]] QString key() const override;

    /**
     * @brief Returns the user-facing codec name.
     * @return Display name.
     */
    [[nodiscard]] QString displayName() const override;

    /**
     * @brief Returns the default codec settings action.
     * @return Default settings action for this codec.
     */
    [[nodiscard]] const mv::gui::CodecSettingsAction* getDefaultCodecSettingsAction() const override;

    /**
     * @brief Creates a codec settings action.
     * @param parent Optional parent object.
     * @return Newly created settings action.
     */
    [[nodiscard]] mv::gui::CodecSettingsAction* createCodecSettingsAction(QObject* parent) const override;

protected:

    /**
     * @brief Creates a Zstandard blob codec.
     * @param parent Optional parent object.
     * @param codecSettingsAction Codec settings action to use.
     * @return Shared codec instance.
     */
    [[nodiscard]] std::shared_ptr<mv::util::BlobCodec> createCodec(QObject* parent, mv::gui::CodecSettingsAction* codecSettingsAction = nullptr) const override;

private:

    ZstdCodecSettingsAction _defaultSettingsAction;  /**< Default codec settings action for this codec. */

    friend class mv::util::CodecRegistry;
};
