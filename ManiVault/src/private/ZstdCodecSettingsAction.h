// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <actions/CodecSettingsAction.h>
#include <actions/IntegralAction.h>

/**
 * @brief Settings action for the Zstandard blob codec.
 *
 * The action exposes the compression level used when creating Zstandard codec
 * instances.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class ZstdCodecSettingsAction : public mv::gui::CodecSettingsAction
{
    Q_OBJECT

public:

    /**
     * @brief Constructs a Zstandard codec settings action.
     * @param parent Optional parent object.
     * @param title Action title.
     */
    ZstdCodecSettingsAction(QObject* parent, const QString& title);

public:

    /**
     * @brief Returns the compression level action.
     * @return Compression level action.
     */
    [[nodiscard]] mv::gui::IntegralAction& getLevelAction() { return _levelAction; }

private:

    mv::gui::IntegralAction  _levelAction;   /**< Compression level action in the range [1, 22]. */
};
