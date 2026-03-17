// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "ZstdBlobCodecFactory.h"
#include "ZstdBlobCodec.h"
#include "ZstdCodecSettingsAction.h"

ZstdBlobCodecFactory::ZstdBlobCodecFactory(QObject* parent /*= nullptr*/) :
    BlobCodecFactory(parent),
    _defaultSettingsAction(nullptr, "Settings")
{
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

mv::gui::CodecSettingsAction* ZstdBlobCodecFactory::createSettingsFromVariantMap(const QVariantMap& map, QObject* parent) const
{
	auto* settings = new ZstdCodecSettingsAction(nullptr, "Settings");
	settings->fromVariantMap(map);
	return settings;
}

std::unique_ptr<mv::util::BlobCodec> ZstdBlobCodecFactory::createCodec(mv::gui::CodecSettingsAction* codecSettingsAction /*= nullptr*/) const
{
	return std::make_unique<ZstdBlobCodec>(const_cast<ZstdBlobCodecFactory*>(this), codecSettingsAction);
}

const mv::gui::CodecSettingsAction* ZstdBlobCodecFactory::getDefaultCodecSettingsAction() const
{
	return &_defaultSettingsAction;
}
