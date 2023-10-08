// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectCompressionAction.h"

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps {

ProjectCompressionAction::ProjectCompressionAction(QObject* parent /*= nullptr*/) :
    GroupAction(parent, "ProjectCompression"),
    _enabledAction(this, "Compression", DEFAULT_ENABLE_COMPRESSION),
    _levelAction(this, "Compression level", 1, 9, DEFAULT_COMPRESSION_LEVEL)
{
    addAction(&_enabledAction);
    addAction(&_levelAction);

    _levelAction.setPrefix("Level: ");

    const auto updateCompressionLevelReadOnly = [this]() -> void {
        _levelAction.setEnabled(_enabledAction.isChecked());
    };

    connect(&_enabledAction, &ToggleAction::toggled, this, updateCompressionLevelReadOnly);

    updateCompressionLevelReadOnly();
}

void ProjectCompressionAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _enabledAction.fromParentVariantMap(variantMap);
    _levelAction.fromParentVariantMap(variantMap);
}

QVariantMap ProjectCompressionAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    _enabledAction.insertIntoVariantMap(variantMap);
    _levelAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
