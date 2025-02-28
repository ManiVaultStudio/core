// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>
#include <QPalette>

namespace mv::util {

/**
 * Color scheme class
 *
 * Contains color scheme information
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ColorScheme
{
public:

    /** Color scheme modes */
    enum class Mode {
        BuiltIn,    /** Built-in color scheme */
        UserAdded   /** Color scheme added by the user */
    };

    /** Maps color scheme mode to name */
    static const QMap<Mode, QString> modeNames;

    /** Vector of color scheme modes */
    using Modes = std::vector<Mode>;

public:

    /**
     * Construct color scheme with \p mode, \p name, \p description and \p palette
     * @param mode Color scheme mode
     * @param name Color scheme name
     * @param description Color scheme description
     * @param palette Color scheme palette
     */
    explicit ColorScheme(const Mode& mode = Mode::BuiltIn, const QString& name = "", const QString& description = "", const QPalette& palette = QPalette());

    /**
     * Get color scheme mode
     * @return Color scheme mode
     */
    Mode getMode() const;

    /**
     * Set color scheme mode to \p mode
     * @param mode Color scheme mode
     */
    void setMode(const Mode& mode);

    /**
     * Get color scheme name
     * @return Color scheme name
     */
    QString getName() const;

    /**
     * Set color scheme name to \p name
     * @param name Color scheme name
     */
    void setName(const QString& name);

    /**
     * Get color scheme description
     * @return Color scheme description
     */
    QString getDescription() const;

    /**
     * Set color scheme description to \p description
     * @param description Color scheme description
     */
    void setDescription(const QString& description);

    /**
     * Get color scheme palette
     * @return Color scheme palette
     */
    QPalette getPalette() const;

    /**
     * Set color scheme palette to \p palette
     * @param palette Color scheme palette
     */
    void setPalette(const QPalette& palette);

private:
    Mode        _mode;          /** Color scheme mode */
    QString     _name;          /** Title */
    QString     _description;   /** Description */
    QPalette    _palette;       /** Palette */
};

/** Vector of color schemes */
using ColorSchemes = QVector<ColorScheme>;

/** Map of color schemes */
using ColorSchemesMap = QMap<QString, ColorScheme>;

}
