// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/CodecSettingsAction.h>

/**
 * TODO
 *
 * @author Thomas Kroes
 */
class ZstdCodecSettingsAction : public mv::gui::CodecSettingsAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    ZstdCodecSettingsAction(QObject* parent, const QString& title);
};
