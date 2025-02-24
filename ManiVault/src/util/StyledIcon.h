// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "Version.h"
#include "StyledIconCommon.h"
#include "Badge.h"

#include <QIcon>
#include <QObject>
#include <QPalette>

#include "StyledIconEngine.h"

namespace mv::util
{

class StyledIconEngine;

/**
 * Styled icon class
 *
 * StyledIcon class is a wrapper around QIcon that allows for easy creation of icons from icon fonts.
 * - Support icon fonts (FontAwesome, Material Design Icons, etc.)
 * - Supports custom icons
 * - Theme-aware (due to StyledIconEngine)
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT StyledIcon : public QIcon
{
public:

    /**
     * Construct from \p iconName, \p iconFontName and \p iconFontVersion
     * @param iconName Name of the icon
     * @param iconFontName Name of the icon font
     * @param iconFontVersion Version of the icon font
     * @param parent Pointer to parent object (maybe nullptr)
     */
    explicit StyledIcon(const QString& iconName = "", const QString& iconFontName = defaultIconFontName, const Version& iconFontVersion = defaultIconFontVersion, QWidget* parent = nullptr);

    /**
     * Copy construct from \p other styled icon
     * @param other Other styled icon to copy from
     */
    StyledIcon(const StyledIcon& other);

protected:

    /**
     * Construct from \p icon
     * @param icon Icon to initialize from
     */
    explicit StyledIcon(const QIcon& icon);

public:

    /**
     * Assign \p other styled icon
     * @param other Other styled icon to assign from
     * @return Copied result
     */
    StyledIcon& operator=(const StyledIcon& other) {
        QIcon::operator=(other);

        _iconEngine      = dynamic_cast<StyledIconEngine*>(other._iconEngine->clone());
        _iconName        = other._iconName;
        _iconFontName    = other._iconFontName;
        _iconFontVersion = other._iconFontVersion;
        _fixColor        = other._fixColor;
        _color           = other._color;
        //_badge              = other._badge;

        return *this;
    }

    /**
     * Assign \p other QIcon
     * @param other Other styled icon to assign from
     * @return Copied result
     */
    //StyledIcon& operator=(const QIcon& other) {
    //    QIcon::operator=(other);

    //    *this = StyledIcon(other);

    //    return *this;
    //}
    
    /**
     * Configure the styled icon by \p iconName and possibly override the default \p iconFontName and \p iconFontVersion
     * @param iconName Name of the icon
     * @param iconFontName Name of the icon font
     * @param iconFontVersion Version of the icon font
     */
    void set(const QString& iconName, const QString& iconFontName = defaultIconFontName, const util::Version& iconFontVersion = defaultIconFontVersion);

    /**
     * Create icon with \p iconName from \p version of Font Awesome regular
     * @param iconName Name of the icon
     * @param version Font Awesome version
     * @return Named icon
     */
    static StyledIcon fromFontAwesomeRegular(const QString& iconName, const Version& version = defaultIconFontVersion);

    /**
     * Create icon with \p iconName from \p version of Font Awesome Brands
     * @param iconName Name of the icon
     * @param version Font Awesome version
     * @return Named icon
     */
    static StyledIcon fromFontAwesomeBrandsRegular(const QString& iconName, const Version& version = defaultIconFontVersion);

    /**
     * Create a styled icon from a Qt \p icon with \p mode
     * @param icon Icon to create the styled icon from
     * @param mode The mode of the styled icon
     * @return Styled icon
     */
    static StyledIcon fromQIcon(const QIcon& icon, const StyledIconMode& mode = StyledIconMode::ThemeAware);

    /**
     * Return styled icon with changed color groups \p colorGroupLightTheme and \p colorGroupDarkTheme
     * @param colorGroupLightTheme Color group for light theme
     * @param colorGroupDarkTheme Color group for dark theme
     * @return Reference to changed styled icon
     */
    StyledIcon& withColorGroups(const QPalette::ColorGroup& colorGroupLightTheme, const QPalette::ColorGroup& colorGroupDarkTheme);

    /**
     * Return styled icon with changed color roles \p colorRoleLightTheme and \p colorRoleDarkTheme
     * @param colorRoleLightTheme Color role for light theme
     * @param colorRoleDarkTheme Color role for dark theme
     * @return Reference to changed styled icon
     */
    StyledIcon& withColorRoles(const QPalette::ColorRole& colorRoleLightTheme, const QPalette::ColorRole& colorRoleDarkTheme);

    /**
     * Return styled icon with \p badge
     * @param badge Badge
     * @return Reference to changed styled icon
     */
    StyledIcon& withBadge(const util::Badge& badge);

    /**
     * Return styled icon with changed \p color
     * @param color Color
     * @return Reference to changed styled icon
     */
    StyledIcon& withColor(const QColor& color);

    /**
     * Return styled icon with \p mode
     * @param mode The mode of the styled icon
     * @return Reference to changed styled icon
     */
    StyledIcon& withMode(const StyledIconMode& mode);

    /**
     * Get icon font for \p iconFontName at \p iconFontVersion
     * @param fontPointSize Point size of the font
     * @param iconFontName Name of the icon font
     * @param iconFontVersion Version of the icon font
     * @return Icon font
     */
    static QFont getIconFont(std::int32_t fontPointSize = -1, const QString& iconFontName = defaultIconFontName, const Version& iconFontVersion = defaultIconFontVersion);

    /**
     * Converts human readable \p icon name to icon character (unicode)
     * @param iconName Name of the icon
     * @param iconFontName Name of the icon font
     * @param iconFontVersion Version of the icon font
     * @return Unicode icon character (empty if not found)
     */
    static QString getIconCharacter(const QString& iconName, const QString& iconFontName = defaultIconFontName, const Version& iconFontVersion = defaultIconFontVersion);

protected:

    /**
     * Initialize \p version of \p iconFontName
     * @param iconFontName Name of the icon font
     * @param iconFontVersion Font Awesome version
     */
    static void initializeIconFont(const QString& iconFontName, const Version& iconFontVersion);

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

public: // Color roles

    /**
     * Get color role for light theme
     * @return Color role for light theme
     */
    QPalette::ColorRole getColorRoleLightTheme() const;

    /**
     * Set color role for light theme to \p colorRoleLightTheme
     * @param colorRoleLightTheme Color role for light theme
     */
    void setColorRoleLightTheme(const QPalette::ColorRole& colorRoleLightTheme) const;

    /**
     * Get color role for dark theme
     * @return Color role for dark theme
     */
    QPalette::ColorRole getColorRoleDarkTheme() const;

    /**
     * Set color role for dark theme to \p colorRoleDarkTheme
     * @param colorRoleDarkTheme Color role for dark theme
     */
    void setColorRoleDarkTheme(const QPalette::ColorRole& colorRoleDarkTheme) const;

    /**
     * Get color group for light theme
     * @return Color group for light theme
     */
    QPalette::ColorGroup getColorGroupLightTheme() const;

    /**
     * Set color group for light theme to \p colorGroupLightTheme
     * @param colorGroupLightTheme Color group for light theme
     */
    void setColorGroupLightTheme(const QPalette::ColorGroup& colorGroupLightTheme) const;

    /**
     * Get color group for dark theme
     * @return Color group for dark theme
     */
    QPalette::ColorGroup getColorGroupDarkTheme() const;

    /**
     * Set color group for dark theme to \p colorGroupDarkTheme
     * @param colorGroupDarkTheme Color group for dark theme
     */
    void setColorGroupDarkTheme(const QPalette::ColorGroup& colorGroupDarkTheme) const;

public: // Badge

    /**
     * Get badge
     * @return Badge
     */
    //Badge& getBadge();

    /**
     * Set badge enabled to \p badgeEnabled
     * @param badgeEnabled Badge enabled
     */
    //void setBadgeEnabled(bool badgeEnabled);

    /**
     * Get badge enabled
     * @return Badge enabled
     */
    //bool isBadgeEnabled() const;

private:

    /** Only allow styled icon constructors, not the base */
    using QIcon::QIcon;

    /** Update the pixmap representation of the icon */
    void updateIconPixmap() const;

    /**
     * Create cryptographic key 
     * @param iconName Icon name
     * @param iconFontName Font name
     * @param iconFontVersion Font version
     * @return SHA
     */
    static QString generateSha(const QString& iconName, const QString& iconFontName, const Version& iconFontVersion);

private:
    StyledIconEngine*   _iconEngine;        /** Icon engine */
	QString             _iconName;          /** Name of the icon */
    QString             _iconFontName;      /** Name of the icon font */
    Version             _iconFontVersion;   /** Version of the icon font */
    bool                _fixColor;          /** Whether to fix the color */
    QColor              _color;             /** Color of the icon */
    //Badge               _badge;             /** Badge */

protected:
	static QMap<QString, QVariantMap>   fontMetadata;               /** Font-specific metadata */
    static QMap<QString, QFont>         fonts;                      /** Icon fonts */
    static QString                      defaultIconFontName;        /** Default icon font name */
    static Version                      defaultIconFontVersion;     /** Default icon font version */
    static QMap<QString, QPixmap>       pixmaps;                    /** All pixmaps for icons */
    static QVector<QStringList>         iconFontPreferenceGroups;   /** The order in each usb-vector determines fetching precedence */

    friend class StyledIconEngine;
};


}
