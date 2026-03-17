// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectCompressionAction.h"

#include "util/CodecRegistry.h"

using namespace mv::gui;
using namespace mv::util;

namespace mv {

ProjectCompressionAction::ProjectCompressionAction(QObject* parent /*= nullptr*/) :
    GroupAction(parent, "ProjectCompression"),
    _enabledAction(this, "Compression", DEFAULT_ENABLE_COMPRESSION),
    _codecTypeAction(this, "Blob Codec")
{
    addAction(&_enabledAction);

    QMap<QString, QString> typeNameForDisplayName;

    auto addCodec = [&](const BlobCodecFactory* factory) {
        Q_ASSERT(factory);

        typeNameForDisplayName[factory->displayName()] = factory->key();

        _codecInstanceMap[factory->displayName()] = factory->createCodec();
    };

    addCodec(&codecRegistry().factory(BlobCodec::Type::None));
    addCodec(&codecRegistry().factory(BlobCodec::Type::Zstd));

    _codecTypeAction.initialize(typeNameForDisplayName.keys(), codecRegistry().factory(BlobCodec::Type::Zstd).displayName());
}

std::shared_ptr<BlobCodec> ProjectCompressionAction::getCodec() const
{
    try {
        const auto codecType = codecRegistry().typeFromDisplayName(_codecTypeAction.getCurrentText());

        if (!codecRegistry().isRegistered(codecType))
            throw std::runtime_error("Codec not registered");

        return _codecInstanceMap.at(_codecTypeAction.getCurrentText());
    }
    catch (const std::exception& e) {
        qWarning() << "Failed to create blob codec:" << e.what();

        return nullptr;
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
