// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginType.h"
#include "Application.h"

namespace hdps::plugin
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
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    static const QMap<Type, QIcon> typeNames{
        { Type::ANALYSIS, fontAwesome.getIcon("square-root-alt") },
        { Type::DATA, fontAwesome.getIcon("database") },
        { Type::LOADER, fontAwesome.getIcon("file-import") },
        { Type::WRITER, fontAwesome.getIcon("file-export") },
        { Type::TRANSFORMATION, fontAwesome.getIcon("random") },
        { Type::VIEW, fontAwesome.getIcon("eye") }
    };

    return typeNames[type];
}

}