// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#ifndef HDPS_PLUGINTYPE_H
#define HDPS_PLUGINTYPE_H

#include <QVector>
#include <QMap>
#include <QIcon>

#include <cstddef>

/**
 * Provides all possible types of plugins.
 *
 * Analysis       - A plugin to perform complex computation on a dataset.
 * Data           - A plugin that defines the structure of raw data.
 * Loader         - A plugin that loads a specific type of data into a dataset.
 * Writer         - A plugin that writes a specific type of data to disk.
 * Transformation - A plugin to transform data in a minimal way.
 * View           - A plugin to display a dataset on screen.
 */

namespace hdps
{
namespace plugin
{
    enum class Type { ANALYSIS, DATA, LOADER, WRITER, TRANSFORMATION, VIEW };

    struct TypeHash
    {
        std::size_t operator()(Type type) const
        {
            return static_cast<std::size_t>(type);
        }
    };
    
    using Types = QVector<Type>;

    QString getPluginTypeName(const Type& type);
    QIcon getPluginTypeIcon(const Type& type);

} // namespace plugin

} // namespace hdps

#endif // HDPS_PLUGINTYPE_H
