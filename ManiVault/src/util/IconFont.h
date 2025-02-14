// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>
#include <QFont>
#include <QIcon>
#include <QMap>
#include <QDebug>

namespace mv::util {

/**
 * Icon font utility class
 *
 * Helper class for extracting an icon (character) from an icon font
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT IconFont
{
public: // Construction/destruction

    /**
     * Constructor
     * @param name Name of the icon font
     * @param majorVersion Major version number
     * @param minorVersion Minor version number
     * @param fontResourceNames Names of the font resources
     * @param defaultFont Boolean determining whether this font is default when no version is specified
     */
    IconFont(const QString& name, const std::uint32_t& majorVersion, const std::uint32_t& minorVersion, const QStringList& fontResourceNames, bool defaultFont = false);

public: // Getters

    /** Get icon font name */
    QString getName() const;

    /** Get icon font full name (includes major and minor version) */
    QString getFullName() const;

    /** Get icon font major version */
    std::uint32_t getMajorVersion() const;

    /** Get icon font minor version */
    std::uint32_t getMinorVersion() const;

    /** Get icon font version string*/
    QString getVersionString() const;

    /**
     * Get icon font search version string
     * @param majorVersion Major version number
     * @param minorVersion Minor version number
     */
    static QString getSearchVersionString(const std::int32_t& majorVersion = -1, const std::int32_t& minorVersion = -1);

    /**
     * Get font
     * @param pointSize Font point size
     * @return Qt font
     */
    QFont getFont(const int& pointSize = -1) const;

    /**
     * Get icon by \p name and \p color
     * @param name Name of the icon
     * @param foregroundColor Foreground color of the icon
     * @param backgroundColor Background color of the icon
     */
    QIcon getIcon(const QString& name, const QColor& foregroundColor = QColor(0, 0, 0, 0), const QColor& backgroundColor = Qt::transparent) const;

    /**
     * Determine if this is the default font
     * @return Boolean determining whether this font is default when no version is specified
     */
    bool isDefaultFont() const;

    /**
     * Set default font to \p defaultFont
     * @param defaultFont Boolean determining whether this font is default when no version is specified
     */
    void setDefaultFont(bool defaultFont);

public: // Initialization

    /**
     * Initializes the icon font
     * A typical implementation builds a mapping from icon name to string-based hexadecimal icon unicode character
     */
    virtual void initialize();

public: // Conversion

    /**
     * Converts human readable \p icon name to icon character (unicode)
     * @param iconName Name of the icon
     * @param return Icon character (unicode)
     */
    virtual QString getIconCharacter(const QString& iconName) const = 0;

protected:
    QString                     _name;                  /** Name of the icon font */
    std::uint32_t               _majorVersion;          /** Major version of the icon font */
    std::uint32_t               _minorVersion;          /** Minor version of the icon font */
    QString                     _fontFamily;            /** Loaded font family */
    QMap<QString, QString>      _characters;            /** Maps icon name to icon character (unicode) */
    bool                        _defaultFont;           /** Whether this font is default when no version is specified */
};

/**
 * Print the contents of \p iconFont to the console using \p debug
 * @param debug Qt output stream for debugging information
 * @param iconFont Reference to icon font class
 */
CORE_EXPORT inline QDebug operator << (QDebug debug, const IconFont& iconFont) {
    debug << iconFont.getFullName();

    return debug;
}

}
