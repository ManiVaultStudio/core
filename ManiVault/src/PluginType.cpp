// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginType.h"
#include "Application.h"

using namespace mv::util;

namespace mv::plugin
{

QString getPluginTypeName(const Type& type)
{
    static const QMap<Type, QString> typeNames{
        { Type::ANALYSIS, "Analysis" },
        { Type::DATA, "Data" },
        { Type::LOADER, "Loader" },
        { Type::WRITER, "Writer" },
        { Type::TRANSFORMATION, "Transformation" },
        { Type::VIEW, "View" }
    };

    return typeNames[type];
}

QIcon getPluginTypeIcon(const Type& type)
{
    static const QMap<Type, QIcon> typeNames{
        { Type::ANALYSIS, StyledIcon("square-root-variable") },
        { Type::DATA, StyledIcon("database") },
        { Type::LOADER, StyledIcon("file-import") },
        { Type::WRITER, StyledIcon("file-export") },
        { Type::TRANSFORMATION, StyledIcon("shuffle") },
        { Type::VIEW, StyledIcon("eye") }
    };

    return typeNames[type];
}

}