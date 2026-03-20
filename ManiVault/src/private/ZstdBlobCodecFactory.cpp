// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "ZstdBlobCodecFactory.h"
#include "ZstdBlobCodec.h"
#include "ZstdCodecSettingsAction.h"

#ifdef _DEBUG
	#define ZSTD_CODEC_FACTORY_VERBOSE
#endif

ZstdBlobCodecFactory::ZstdBlobCodecFactory(QObject* parent /*= nullptr*/) :
    BlobCodecFactory(parent),
    _defaultSettingsAction(nullptr, "Default Zstandard settings")
{
#ifdef ZSTD_CODEC_FACTORY_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}

ZstdBlobCodecFactory::~ZstdBlobCodecFactory()
{
#ifdef ZSTD_CODEC_FACTORY_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}

mv::util::BlobCodec::Type ZstdBlobCodecFactory::type() const
{
	return mv::util::BlobCodec::Type::Zstd;
}

QString ZstdBlobCodecFactory::key() const
{
	return "zstd";
}

QString ZstdBlobCodecFactory::displayName() const
{
	return "Zstandard";
}

std::shared_ptr<mv::util::BlobCodec> ZstdBlobCodecFactory::createCodec(QObject* parent, mv::gui::CodecSettingsAction* codecSettingsAction /*= nullptr*/) const
{
#ifdef ZSTD_CODEC_FACTORY_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (!codecSettingsAction) {
        codecSettingsAction = createCodecSettingsAction(const_cast<ZstdBlobCodecFactory*>(this));
    }

    return std::make_shared<ZstdBlobCodec>(parent, codecSettingsAction);
}

const mv::gui::CodecSettingsAction* ZstdBlobCodecFactory::getDefaultCodecSettingsAction() const
{
#ifdef ZSTD_CODEC_FACTORY_VERBOSE
    qDebug() << __FUNCTION__;
#endif

	return &_defaultSettingsAction;
}

mv::gui::CodecSettingsAction* ZstdBlobCodecFactory::createCodecSettingsAction(QObject* parent) const
{
    return new ZstdCodecSettingsAction(parent, "Z-standard codec settings action");
}
