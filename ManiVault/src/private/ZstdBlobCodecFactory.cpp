// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "ZstdBlobCodecFactory.h"
#include "ZstdBlobCodec.h"
#include "ZstdCodecSettingsAction.h"

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

mv::gui::CodecSettingsAction* ZstdBlobCodecFactory::createDefaultSettings(QObject* parent) const
{
	return new ZstdCodecSettingsAction(nullptr, "Settings");
}

mv::gui::CodecSettingsAction* ZstdBlobCodecFactory::createSettingsFromVariantMap(const QVariantMap& map, QObject* parent) const
{
	auto* settings = new ZstdCodecSettingsAction(nullptr, "Settings");
	settings->fromVariantMap(map);
	return settings;
}

std::unique_ptr<mv::util::BlobCodec> ZstdBlobCodecFactory::createCodec(const mv::gui::CodecSettingsAction& codecSettingsAction) const
{
	return std::make_unique<ZstdBlobCodec>();
}
