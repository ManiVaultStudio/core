// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "CodecRegistry.h"
#include "Application.h"

#include "actions/CodecSettingsAction.h"

#ifdef _DEBUG
	#define CODEC_REGISTRY_VERBOSE
#endif

using namespace mv::gui;

namespace mv::util {

CodecRegistry::CodecRegistry() = default;

CodecRegistry& codecRegistry()
{
    return Application::current()->getCodecRegistry();
}

void CodecRegistry::registerFactory(std::unique_ptr<BlobCodecFactory> factory)
{
#ifdef CODEC_REGISTRY_VERBOSE
    qDebug() << __FUNCTION__;
#endif

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

SharedCodec CodecRegistry::createCodec(QObject* parent, BlobCodec::Type type) const
{
#ifdef CODEC_REGISTRY_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    return factory(type).createCodec(parent, nullptr);
}

SharedCodec CodecRegistry::createCodec(QObject* parent, mv::gui::CodecSettingsAction* codecSettingsAction /*= nullptr*/) const
{
#ifdef CODEC_REGISTRY_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    //if (!codecSettingsAction)
    //    throw std::runtime_error("Codec settings action is null");

	return factory(codecSettingsAction->getTypeAction().getString()).createCodec(parent, codecSettingsAction);
}

SharedCodec CodecRegistry::createCodec(QObject* parent, const QString& typeName) const
{
#ifdef CODEC_REGISTRY_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (typeName == "")
        throw std::runtime_error("Blob codec type name is empty");

    return createCodec(parent, BlobCodec::typeFromString(typeName));
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

QStringList CodecRegistry::availableTypeDisplayNames() const
{
    QStringList typeDisplayNames;

    for (const auto& availableType : availableTypes())
        typeDisplayNames << factory(availableType).displayName();

    return typeDisplayNames;
}

BlobCodec::Type CodecRegistry::typeFromDisplayName(const QString& displayName) const
{
    const auto it = std::find_if(_factoriesOwned.begin(), _factoriesOwned.end(), [&displayName](const std::unique_ptr<BlobCodecFactory>& factory) {
        return factory->displayName() == displayName;
    });

    if (it == _factoriesOwned.end())
        throw std::runtime_error("Blob codec not registered");

    return (*it)->type();

}

}
