// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::gui
{
    class CodecSettingsAction;
}

namespace mv::util {

/**
 * @brief Binds a BlobCodec instance to its corresponding GUI CodecSettingsAction.
 *
 * CodecActionBinding provides a safe bridge between the runtime codec layer and
 * the GUI layer. It ensures that a gui::CodecSettingsAction is properly cleaned
 * up when the associated BlobCodec is destroyed, without introducing direct
 * ownership or parent-child relationships between objects that may live in
 * different threads.
 *
 * This class exists to avoid common pitfalls:
 * - Prevents dangling pointers to GUI actions when codecs are destroyed
 * - Avoids illegal QObject parenting across threads
 * - Decouples GUI lifetime management from codec runtime logic
 *
 * The binding listens to the destruction of the BlobCodec and schedules the
 * deletion of the associated CodecSettingsAction using deleteLater(), ensuring
 * that deletion occurs safely in the GUI thread.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT CodecActionBinding : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructs a CodecActionBinding that binds a BlobCodec to a CodecSettingsAction.
     * @param codec The BlobCodec instance to bind
     * @param action The CodecSettingsAction instance to bind
     */
    CodecActionBinding(BlobCodec* codec, gui::CodecSettingsAction* action);

    ~CodecActionBinding();

private:
    QPointer<BlobCodec>                 _codec;     /** The associated BlobCodec instance */
    QPointer<gui::CodecSettingsAction>  _action;    /** The associated CodecSettingsAction instance */
};

}