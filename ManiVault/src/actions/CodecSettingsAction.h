// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "VerticalGroupAction.h"
#include "StringAction.h"
#include "IntegralAction.h"

namespace mv::gui {

/**
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT CodecSettingsAction : public VerticalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent and \p project
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE CodecSettingsAction(QObject* parent, const QString& title);

public: // Action getters

    const StringAction& getTypeAction() const { return _typeAction; }

    StringAction& getTypeAction() { return _typeAction; }
    IntegralAction& getBlockSizeAction() { return _blockSizeAction; }

private:
    StringAction    _typeAction;        /** Type of compression codec */
    IntegralAction  _blockSizeAction;   /** Block size for compression (MiB) */
};

using CodecSettingsActionPtr = QPointer<CodecSettingsAction>;

}

Q_DECLARE_METATYPE(mv::gui::CodecSettingsAction)

inline const auto codecSettingsActionMetaTypeId = qRegisterMetaType<mv::gui::CodecSettingsAction*>("mv::gui::CodecSettingsAction");