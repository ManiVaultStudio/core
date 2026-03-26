// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QVariantMap>
#include <QString>

/** Context for workspace loading */
struct WorkspaceLoadContext
{
    QString         _jsonFilePath;          /** Path to the workspace JSON file */
    QVariantMap     _workspaceVariantMap;   /** Variant map containing the workspace data */
    QString         _error;                 /** Error message, if any error occurs during the loading process */
};
