// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "CodecSettingsAction.h"

namespace mv::gui {

CodecSettingsAction::CodecSettingsAction(QObject* parent, const QString& title) :
    VerticalGroupAction(parent, title),
    _typeAction(this, "Type"),
    _blockSizeAction(this, "Block size (MiB)", 1, 1024, 32)
{
    setIconByName("gear");
    setConfigurationFlag(ConfigurationFlag::ForceCollapsedInGroup);
    setPopupSizeHint(QSize(400, 0));

    _typeAction.setToolTip("Type of compression codec");
    _blockSizeAction.setToolTip("Block size (MiB) for compression");

    addAction(&_blockSizeAction);
}

}
