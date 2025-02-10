// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "Version.h"
#include "ThemeWatcher.h"

#include <QIcon>
#include <QObject>

namespace mv::util
{

/**
 * Named icon class
 *
 * Icon which responds to theme changes (from mv::util::ThemeWatcher).
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT NamedIcon : public QIcon, public QObject
{
public: // Construction/destruction

    /**
     * Construct from \p iconName, \p iconFontName and \p iconFontVersion
     * @param iconName Name of the icon
     * @param iconFontName Name of the icon font
     * @param iconFontVersion Version of the icon font
     */
    explicit NamedIcon(const QString& iconName, const QString& iconFontName, const Version& iconFontVersion);

    /**
     * Create icon with \p iconName from \p version of Font Awesome
     * @param iconName Name of the icon
     * @param version Font Awesome version
     * @return Named icon
     */
    static NamedIcon fromFontAwesome(const QString& iconName, const Version& version = { 6, 5 });

    /**
     * Create icon with \p iconName from \p version of Font Awesome Brands
     * @param iconName Name of the icon
     * @param version Font Awesome version
     * @return Named icon
     */
    static NamedIcon fromFontAwesomeBrands(const QString& iconName, const Version& version = { 6, 5 });

    /**
     * Initialize \p version of \p iconFontName
     * @param iconFontName Name of the icon font
     * @param iconFontVersion Font Awesome version
     */
    static void initializeIconFont(const QString& iconFontName, const Version& iconFontVersion);

    /**
     * Converts human readable \p icon name to icon character (unicode)
     * @param iconName Name of the icon
     * @param iconFontName Name of the icon font
     * @param iconFontVersion Version of the icon font
     * @return Unicode icon character (empty if not found) 
     */
    static QString getIconCharacter(const QString& iconName, const QString& iconFontName, const Version& iconFontVersion);

    /**
     * Get icon font resource name for \p iconFontName at \p version
     * @param iconFontName Name of the icon font
     * @param version Font Awesome version
     * @return Icon font resource name
     */
    static QString getIconFontResourceName(const QString& iconFontName, const Version& version);

    /**
     * Get icon with \p iconName, \p iconFontName and \p iconFontVersion
     * @param iconName Icon name
     * @param iconFontName Font name
     * @param iconFontVersion Font version
     * @param foregroundColor Foreground color
     * @param backgroundColor Background color
     * @return 
     */
    static QIcon getIcon(const QString& iconName, const QString& iconFontName, const Version& iconFontVersion, const QColor& foregroundColor/*= QColor(0, 0, 0, 0)*/, const QColor& backgroundColor/*= Qt::transparent*/);

    /**
     * Get icon font version string for \p version
     * @return Icon font version string (e.g. 2.4)
     */
    static QString getIconFontVersionString(const Version& version);

protected:

    /** Only allow named icon construction */
    using QIcon::QIcon;

    /** Trigger repaint of owning widget */
    void updateIcon();

private:
    const QString                   _iconName;          /** Name of the icon */
    const QString                   _iconFontName;      /** Name of the icon font */
    const Version                   _iconFontVersion;   /** Version of the icon font */
    ThemeWatcher                    _themeWatcher;      /** Use our own theme watcher (which does not emit paletteChanged(...) needlessly) */

protected:
	static QMap<QString, QVariant>  fontMetadata;   /** Font-specific metadata */
    static QMap<QString, QFont>     fonts;          /** Icon fonts */
};


}