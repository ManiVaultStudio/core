// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PassthroughCodecSettingsAction.h"

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

PassthroughCodecSettingsAction::PassthroughCodecSettingsAction(QObject* parent, const QString& title) :
    CodecSettingsAction(parent, title)
{
}
