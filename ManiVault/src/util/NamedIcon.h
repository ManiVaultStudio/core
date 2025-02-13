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
class CORE_EXPORT NamedIcon : public QObject, public QIcon
{
    Q_OBJECT

public:

    /**
     * Construct from \p iconName, \p iconFontName and \p iconFontVersion
     * @param iconName Name of the icon
     * @param iconFontName Name of the icon font
     * @param iconFontVersion Version of the icon font
     * @param parent Pointer to parent object (maybe nullptr)
     */
    explicit NamedIcon(const QString& iconName = "", const QString& iconFontName = defaultIconFontName, const Version& iconFontVersion = defaultIconFontVersion, QWidget* parent = nullptr);

    /**
     * Copy construct from \p other named icon
     * @param other Other named icon to copy from
     */
    explicit NamedIcon(const NamedIcon& other);

    /**
     * Construct from \p icon
     * @param icon Icon to initialize from
     */
    explicit NamedIcon(const QIcon& icon);

    /**
     * Assign \p other named icon
     * @param other Other named icon to assign from
     * @return Copied result
     */
    NamedIcon& operator=(const NamedIcon& other) {
        _iconName           = other._iconName;
        _iconFontName       = other._iconFontName;
        _iconFontVersion    = other._iconFontVersion;
        _sha                = other._sha;

        return *this;
    }

    /**
     * Assign \p other QIcon
     * @param other Other named icon to assign from
     * @return Copied result
     */
    NamedIcon& operator=(const QIcon& other) {
        *this = NamedIcon(other);

        return *this;
    }
    
    /**
     * Configure the named icon by \p iconName and possibly override the default \p iconFontName and \p iconFontVersion
     * @param iconName Name of the icon
     * @param iconFontName Name of the icon font
     * @param iconFontVersion Version of the icon font
     */
    void set(const QString& iconName, const QString& iconFontName = defaultIconFontName, const util::Version& iconFontVersion = defaultIconFontVersion);

    /**
     * Get icon pixmap
     * @param foregroundColor Foreground color
     * @return Pointer to pixmap (maybe nullptr)
     */
    QPixmap* getIconPixmap(const QColor& foregroundColor = QColor(0, 0, 0, 0)) const;

    /**
     * Create icon with \p iconName from \p version of Font Awesome regular
     * @param iconName Name of the icon
     * @param version Font Awesome version
     * @return Named icon
     */
    static NamedIcon fromFontAwesomeRegular(const QString& iconName, const Version& version = { 6, 5 });

    /**
     * Create icon with \p iconName from \p version of Font Awesome Brands
     * @param iconName Name of the icon
     * @param version Font Awesome version
     * @return Named icon
     */
    static NamedIcon fromFontAwesomeBrandsRegular(const QString& iconName, const Version& version = { 6, 5 });

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
     * @param iconFontVersion Icon font version
     * @return Icon font resource name
     */
    static QString getIconFontResourceName(const QString& iconFontName, const Version& iconFontVersion);

    /**
     * Get icon font resource path for \p iconFontName at \p version
     * @param iconFontName Name of the icon font
     * @param iconFontVersion Icon font version
     * @return Icon font resource path
     */
    static QString getIconFontResourcePath(const QString& iconFontName, const Version& iconFontVersion);

    /**
     * Get icon font metadata resource path for \p iconFontName at \p version
     * @param iconFontName Name of the icon font
     * @param iconFontVersion Icon font version
     * @return Icon font metadata resource path
     */
    static QString getIconFontMetadataResourcePath(const QString& iconFontName, const Version& iconFontVersion);

    /**
     * Create icon pixmap from \p iconName, \p iconFontName and \p iconFontVersion
     * @param iconName Icon name
     * @param iconFontName Font name
     * @param iconFontVersion Font version
     * @param foregroundColor Foreground color
     * @param backgroundColor Background color
     * @return Icon pixmap
     */
    static QPixmap createIconPixmap(const QString& iconName, const QString& iconFontName, const Version& iconFontVersion, const QColor& foregroundColor= QColor(0, 0, 0, 0), const QColor& backgroundColor = Qt::transparent);

    /**
     * Get icon font version string for \p version
     * @return Icon font version string (e.g. 2.4)
     */
    static QString getIconFontVersionString(const Version& version);

private:

    /** Only allow named icon construction */
    using QIcon::QIcon;

    /** Trigger repaint of owning widget */
    void updateIcon() const;

    /**
     * Create cryptographic key 
     * @param iconName Icon name
     * @param iconFontName Font name
     * @param iconFontVersion Font version
     * @return SHA
     */
    static QString generateSha(const QString& iconName, const QString& iconFontName, const Version& iconFontVersion);

signals:

    /**
     * Signals that the icon name changed from \p previousIconName to \p currentIconName
     * @param previousIconName Previous icon name
     * @param currentIconName Current icon name
     */
    void iconNameChanged(const QString& previousIconName, const QString& currentIconName);

    /**
     * Signals that the icon font name changed from \p previousFontName to \p currentFontName
     * @param previousIconFontName Previous icon font name
     * @param currentIconFontName Current icon font name
     */
    void iconFontNameChanged(const QString& previousIconFontName, const QString& currentIconFontName);

    /**
     * Signals that the icon font version changed from \p previousFontVersion to \p currentFontVersion
     * @param previousIconFontVersion Previous icon font version
     * @param currentIconFontVersion Current icon font version
     */
    void iconFontVersionChanged(const Version& previousIconFontVersion, const Version& currentIconFontVersion);

    /** Signals that the NamedIcon::_iconName and/or NamedIcon::_iconFontName and/or NamedIcon::_iconFontVersion changed */
    void changed();

private:
    QString         _iconName;          /** Name of the icon */
    QString         _iconFontName;      /** Name of the icon font */
    Version         _iconFontVersion;   /** Version of the icon font */
    QString         _sha;               /** NamedIcons::icons key */
    ThemeWatcher    _themeWatcher;      /** Use our own theme watcher (which does not emit paletteChanged(...) needlessly) */

protected:
	static QMap<QString, QVariantMap>   fontMetadata;               /** Font-specific metadata */
    static QMap<QString, QFont>         fonts;                      /** Icon fonts */
    static QString                      defaultIconFontName;        /** Default icon font name */
    static Version                      defaultIconFontVersion;     /** Default icon font version */
    static QMap<QString, QPixmap>       pixmaps;                    /** All pixmaps for icons */
};


}
