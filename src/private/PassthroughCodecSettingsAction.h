// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <actions/CodecSettingsAction.h>

/**
 * @brief Settings action for the pass-through blob codec.
 *
 * The pass-through codec does not expose user-configurable settings, but it
 * still provides a concrete CodecSettingsAction for consistency with the codec
 * factory API.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class PassthroughCodecSettingsAction : public mv::gui::CodecSettingsAction
{
    Q_OBJECT

public:

    /**
     * @brief Constructs a pass-through codec settings action.
     * @param parent Optional parent object.
     * @param title Action title.
     */
    PassthroughCodecSettingsAction(QObject* parent, const QString& title);

    /**
     * @brief Destroys the settings action.
     */
    ~PassthroughCodecSettingsAction();
};
