// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ZstdCodecSettingsAction.h"

#ifdef _DEBUG
	#define ZSTD_CODEC_SETTINGS_ACTION_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

ZstdCodecSettingsAction::ZstdCodecSettingsAction(QObject* parent, const QString& title) :
    CodecSettingsAction(parent, title),
    _levelAction(this, "Level", 1, 22, 3)
{
#ifdef ZSTD_CODEC_SETTINGS_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    addAction(&_levelAction);
}

ZstdCodecSettingsAction::~ZstdCodecSettingsAction()
{
#ifdef ZSTD_CODEC_SETTINGS_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}
