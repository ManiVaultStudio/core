// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "PassthroughBlobCodecFactory.h"
#include "PassthroughBlobCodec.h"
#include "PassthroughCodecSettingsAction.h"

#ifdef _DEBUG
	#define PASSTHROUGH_CODEC_FACTORY_VERBOSE
#endif

PassthroughBlobCodecFactory::PassthroughBlobCodecFactory(QObject* parent /*= nullptr*/) :
    BlobCodecFactory(parent),
    _defaultSettingsAction(nullptr, "Settings")
{
#ifdef 	PASSTHROUGH_CODEC_FACTORY_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}

PassthroughBlobCodecFactory::~PassthroughBlobCodecFactory()
{
#ifdef 	PASSTHROUGH_CODEC_FACTORY_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}

mv::util::BlobCodec::Type PassthroughBlobCodecFactory::type() const
{
    return mv::util::BlobCodec::Type::None;
}

QString PassthroughBlobCodecFactory::key() const
{
    return "none";
}

QString PassthroughBlobCodecFactory::displayName() const
{
    return "No compression";
}

mv::gui::CodecSettingsAction* PassthroughBlobCodecFactory::createSettingsFromVariantMap(const QVariantMap& map, QObject* parent) const
{
#ifdef 	PASSTHROUGH_CODEC_FACTORY_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    auto* settings = new PassthroughCodecSettingsAction(nullptr, "Settings");
    settings->fromVariantMap(map);
    return settings;
}

std::shared_ptr<mv::util::BlobCodec> PassthroughBlobCodecFactory::createCodec(mv::gui::CodecSettingsAction* codecSettingsAction /*= nullptr*/) const
{
#ifdef 	PASSTHROUGH_CODEC_FACTORY_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    return std::make_shared<PassthroughBlobCodec>(parent(), codecSettingsAction);
}

const mv::gui::CodecSettingsAction* PassthroughBlobCodecFactory::getDefaultCodecSettingsAction() const
{
#ifdef 	PASSTHROUGH_CODEC_FACTORY_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    return &_defaultSettingsAction;
}
