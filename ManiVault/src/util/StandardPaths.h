// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QMap>
#include <QString>

namespace mv::util {

/**
 * Standard paths class
 *
 * Provides methods to retrieve standard paths used in the application
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT StandardPaths
{
public:

    enum StandardLocation {
        Plugins,
        Downloads,
        Customization,
        Projects,
        Logs
    };

    /**
     * Get standard path
     *
     * @param location Standard location to retrieve
     * @return Standard path as string
     */
    static QString get(StandardLocation location);

public: // Specific getters for convenience

    static QString getPluginsDirectory();
    static QString getDownloadsDirectory();
    static QString getCustomizationDirectory();
    static QString getProjectsDirectory();
    static QString getLogsDirectory();

private:

    /**
     * Determine if the OS is macOS
     * @return True if the OS is macOS
     */
    static bool isMacOS();
};

}
