// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectCompressionAction.h"

#include "util/CodecRegistry.h"

using namespace mv::gui;
using namespace mv::util;

namespace mv {

ProjectCompressionAction::ProjectCompressionAction(QObject* parent /*= nullptr*/) :
    HorizontalGroupAction(parent, "Compression"),
    _enabledAction(this, "Compression", DEFAULT_ENABLE_COMPRESSION),
    _codecTypeAction(this, "Codec")
{
    setShowLabels(false);
    addAction(&_codecTypeAction);

    QMap<QString, QString> typeNameForDisplayName;

    auto addCodec = [&](const BlobCodecFactory* factory) {
        Q_ASSERT(factory);

        typeNameForDisplayName[factory->displayName()] = factory->key();

        auto codecSettingsAction = factory->createCodecSettingsAction(this);

        _codecSettingsActionMap.emplace(factory->displayName(), codecSettingsAction);

        addAction(codecSettingsAction);
    };

    addCodec(&codecRegistry().factory(BlobCodec::Type::None));
    addCodec(&codecRegistry().factory(BlobCodec::Type::Zstd));

    _codecTypeAction.initialize(typeNameForDisplayName.keys(), codecRegistry().factory(BlobCodec::Type::Zstd).displayName());

    auto codecTypeChanged = [this]() -> void {
		for (auto& [displayName, codecSettingsAction] : _codecSettingsActionMap) {
            codecSettingsAction->setVisible(displayName == _codecTypeAction.getCurrentText());
        }
	};

    connect(&_codecTypeAction, &OptionAction::currentIndexChanged, this, codecTypeChanged);

    codecTypeChanged();
}

CodecSettingsAction* ProjectCompressionAction::getCodecSettingsAction() const
{
    try {
        return _codecSettingsActionMap.at(_codecTypeAction.getCurrentText());
    }
    catch (const std::exception& e) {
        qWarning() << "Failed to create blob codec:" << e.what();

        return {};
    }
}

SharedCodec ProjectCompressionAction::createCodec(QObject* parent) const
{
    try {
        return codecRegistry().createCodec(parent, getCodecSettingsAction());
    }
    catch (const std::exception& e) {
        qWarning() << "Failed to create blob codec:" << e.what();

        return {};
    }
}

void ProjectCompressionAction::fromVariantMap(const QVariantMap& variantMap)
{
    GroupAction::fromVariantMap(variantMap);

    _enabledAction.fromParentVariantMap(variantMap);
    //_levelAction.fromParentVariantMap(variantMap);
}

QVariantMap ProjectCompressionAction::toVariantMap() const
{
    auto variantMap = GroupAction::toVariantMap();

    _enabledAction.insertIntoVariantMap(variantMap);
    //_levelAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
