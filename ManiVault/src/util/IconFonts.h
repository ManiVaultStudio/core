// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <map>
#include <stdexcept>

#include <QString>
#include <QVector>
#include <QSharedPointer>

#include "IconFont.h"

namespace mv::util {

/**
 * Icon fonts utility class
 *
 * Helper class for keeping track of icon fonts
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT IconFonts
{
    using Fonts = QVector<QSharedPointer<IconFont>>;

public: // Exceptions

    /**
     * Icon not found exception class
     *
     * @author Thomas Kroes
     */
    class CORE_EXPORT IconFontNotFoundException : public std::runtime_error
    {
    public:
        /**
         * Constructor
         * @param iconFontName Name of the icon font that was not found
         * @param majorVersion Major version of the icon font that was not found
         * @param minorVersion Minor version of the icon font that was not found
         */
        IconFontNotFoundException(const QString& iconFontName, const std::int32_t& majorVersion, const std::int32_t& minorVersion) :
            std::runtime_error(QString("%1-%2 not found").arg(iconFontName, IconFont::getSearchVersionString(majorVersion, minorVersion)).toLatin1())
        {
        }
    };

public: // Getters

    /**
     * Get icon font by \p name \p majorVersion and \p minorVersion
     * @param name Name of the icon font
     * @param majorVersion Major version number (latest if not specified)
     * @param minorVersion Minor version number (latest if not specified)
     */
    const IconFont& getIconFont(const QString& name, const std::int32_t& majorVersion = -1, const std::int32_t& minorVersion = -1) const;

    /**
     * Get icon by \p fontName, \p iconName, \p iconForegroundGolor and \p iconBackgroundColor
     * @param fontName Name of the icon font
     * @param iconName Name of the icon
     * @param iconForegroundGolor Foreground color of the icon
     * @param iconBackgroundColor Background color of the icon
    */
    QIcon getIcon(const QString& fontName, const QString& iconName, const QColor& iconForegroundGolor = QColor(0, 0, 0, 255), const QColor& iconBackgroundColor = Qt::transparent) const;

    /** Returns a list of the installed icon font names */
    QStringList getFontNames() const;

    /**
     * Get all icon fonts (versions) for the provided \p fontName
     * @param fontName Name of the font
     */
    Fonts getIconFonts(const QString& fontName) const;

public:

    /**
     * Adds an icon font to the database
     * @param iconFont The icon font to add
     */
    void add(const QSharedPointer<IconFont>& iconFont);

private:
    QMap<QString, Fonts>    _iconFonts;  /** Icon fonts */
};

/**
 * Print the contents of \p iconFonts to the console using \p debug
 * @param debug Qt output stream for debugging information
 * @param iconFonts Reference to icon fonts class
 */
CORE_EXPORT inline QDebug operator << (QDebug debug, const IconFonts& iconFonts) {
    for (const auto& fontName : iconFonts.getFontNames()) {
        debug.noquote();

        QStringList versions;

        for (const auto& iconFont : iconFonts.getIconFonts(fontName)) {
            versions << iconFont->getVersionString();
        }

        debug << QString("%1 (%2)").arg(fontName, versions.join(", "));
    }
    
    return debug;
}

}