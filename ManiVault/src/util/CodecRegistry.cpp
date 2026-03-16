// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "CodecRegistry.h"

#include "actions/CodecSettingsAction.h"

using namespace mv::gui;

namespace mv::util {

CodecRegistry& CodecRegistry::instance()
{
	static CodecRegistry registry;

	return registry;
}

void CodecRegistry::registerFactory(std::unique_ptr<BlobCodecFactory> factory)
{
	const auto type = factory->type();

	_factoriesByType[type] = std::move(factory);
}

bool CodecRegistry::isRegistered(BlobCodec::Type type) const
{
	return _factoriesByType.contains(type);
}

bool CodecRegistry::isRegistered(const QString& key) const
{
	return _factoriesByType.contains(BlobCodec::typeFromString(key));
}

const BlobCodecFactory& CodecRegistry::factory(BlobCodec::Type type) const
{
	const auto it = _factoriesByType.find(type);

	if (it == _factoriesByType.end())
		throw std::runtime_error("No codec factory registered for requested type");

	return *it->second;
}

CodecSettingsAction* CodecRegistry::createDefaultSettings(BlobCodec::Type type, QObject* parent) const
{
	return factory(type).createDefaultSettings(parent);
}

CodecSettingsAction* CodecRegistry::createSettingsFromVariantMap(BlobCodec::Type type, const QVariantMap& map, QObject* parent) const
{
	return factory(type).createSettingsFromVariantMap(map, parent);
}

std::unique_ptr<BlobCodec> CodecRegistry::createCodec(const CodecSettingsAction& codecSettingsAction) const
{
    const auto codecType = BlobCodec::typeFromString(codecSettingsAction.getTypeAction().getString());

	return factory(codecType).createCodec(codecSettingsAction);
}

}
