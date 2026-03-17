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
    /*
    addAction(&_levelAction);

    _levelAction.setPrefix("Level: ");

    const auto updateCompressionLevelReadOnly = [this]() -> void {
        _levelAction.setEnabled(_enabledAction.isChecked());
    };
    */
    //connect(&_enabledAction, &ToggleAction::toggled, this, updateCompressionLevelReadOnly);

    //updateCompressionLevelReadOnly();

    QMap<QString, QString> typeNameForDisplayName {
        { "None", "none" },
        { "Qt Compress", "QtCompress" },
        { "Zstandard", "Zstdandard" } 
    };

    _codecTypeAction.initialize(codecRegistry().availableTypeDisplayNames(), "Zstandard");
}

std::unique_ptr<BlobCodec> ProjectCompressionAction::createCodec() const
{
    try {
        if (!codecRegistry().isRegistered( codecRegistry().typeFromDisplayName(_codecTypeAction.getCurrentText())))
            throw std::runtime_error("Codec not registered");

        if (_codecSettingsAction)
            throw std::runtime_error("Codec settings action is not available");

        return codecRegistry().createCodec();
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
