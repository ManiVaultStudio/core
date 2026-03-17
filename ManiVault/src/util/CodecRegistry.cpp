// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "CodecRegistry.h"
#include "Application.h"

#include "actions/CodecSettingsAction.h"

using namespace mv::gui;

namespace mv::util {

CodecRegistry::CodecRegistry() = default;

CodecRegistry& codecRegistry()
{
    return Application::current()->getCodecRegistry();
}

void CodecRegistry::registerFactory(std::unique_ptr<BlobCodecFactory> factory)
{
	const auto type = factory->type();
	const auto key  = factory->key();

	_factoriesByType[type] = factory.get();
	_factoriesByKey[key]   = factory.get();

	_factoriesOwned.push_back(std::move(factory));
}

bool CodecRegistry::isRegistered(BlobCodec::Type type) const
{
	return _factoriesByType.contains(type);
}

bool CodecRegistry::isRegistered(const QString& key) const
{
	return _factoriesByKey.contains(key);
}

const BlobCodecFactory& CodecRegistry::factory(BlobCodec::Type type) const
{
	const auto it = _factoriesByType.find(type);

	if (it == _factoriesByType.end())
		throw std::runtime_error("Blob codec not registered");

	return *it->second;
}

const BlobCodecFactory& CodecRegistry::factory(const QString& key) const
{
	const auto it = _factoriesByKey.find(key);

	if (it == _factoriesByKey.end())
		throw std::runtime_error("Blob codec not registered");

	return *it.value();
}

gui::CodecSettingsAction* CodecRegistry::createSettingsFromVariantMap(BlobCodec::Type type, const QVariantMap& map, QObject* parent) const
{
	return factory(type).createSettingsFromVariantMap(map, parent);
}

std::unique_ptr<BlobCodec> CodecRegistry::createCodec(const mv::gui::CodecSettingsAction* codecSettingsAction /*= nullptr*/) const
{
    Q_ASSERT(codecSettingsAction);

	return factory(codecSettingsAction->getTypeAction().getString()).createCodec(codecSettingsAction);
}

std::unique_ptr<BlobCodec> CodecRegistry::createCodec(BlobCodec::Type type) const
{
    return factory(type).createCodec();
}

std::unique_ptr<BlobCodec> CodecRegistry::createCodec(const QString& typeName) const
{
    return createCodec(BlobCodec::typeFromString(typeName));
}

std::vector<BlobCodec::Type> CodecRegistry::availableTypes() const
{
	std::vector<BlobCodec::Type> types;

	types.reserve(_factoriesByType.size());

	for (const auto& [type, factory] : _factoriesByType)
		types.push_back(type);

	return types;
}

QStringList CodecRegistry::availableTypeNames() const
{
    QStringList typeNames;

    for (const auto& availableType : availableTypes())
        typeNames.append(BlobCodec::typeToString(availableType));

    return typeNames;
}

}
