// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "CodecActionBinding.h"
#include "actions/CodecSettingsAction.h"

#ifdef _DEBUG
	#define CODEC_ACTION_BINDING_VERBOSE
#endif

namespace mv::util {

CodecActionBinding::CodecActionBinding(BlobCodec* codec, gui::CodecSettingsAction* action):
	QObject(nullptr),
	_codec(codec),
	_action(action)
{
    Q_ASSERT(codec);
    Q_ASSERT(action);

    moveToThread(_codec->thread());
    setParent(_codec);

    connect(_codec, &QObject::destroyed, this, [this]() {
#ifdef CODEC_ACTION_BINDING_VERBOSE
        qDebug() << __FUNCTION__ << "- BlobCodec destroyed, scheduling CodecSettingsAction for deletion";
#endif

        if (_action)
            _action->deleteLater();
    });
}

CodecActionBinding::~CodecActionBinding()
{
#ifdef CODEC_ACTION_BINDING_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}

}
