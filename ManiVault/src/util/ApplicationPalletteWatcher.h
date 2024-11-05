// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QObject>

namespace mv::util {

/**
 * Application pallette watcher
 *
 * Simple utility class which executes a lambda function when the application palette changes
 *
 * Create an instance of this class and set the watch function with applicationPaletteWatcher.setWatchFunction(...)
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ApplicationPaletteWatcher
{

public: // Construction/destruction

    /** (Default) constructor
     * @param name Name of the color map
     * @param resourcePath Resource path of the color map image
     * @param name Color map type
     * @param image Color map image
     */
    ColorMap(const QString& name = "", const QString& resourcePath = "", const Type& type = Type::OneDimensional, const QImage& image = QImage());

public: // Getters

    /** Returns the color map name */
    QString getName() const;

    /** Returns the resource path */
    QString getResourcePath() const;

    /** Returns the color map type */
    Type getType() const;

    /** Returns the color map image */
    QImage getImage() const;

    /** Returns the number of dimensions */
    std::uint32_t getNoDimensions() const;

private:
    QString     _name;              /** Name in the user interface */
    QString     _resourcePath;      /** Resource path of the color image */
    Type        _type;              /** Color map type */
    QImage      _image;             /** Color map image */
};

}
