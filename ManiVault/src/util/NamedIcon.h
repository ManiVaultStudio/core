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

protected:

    /** Only allow named icon construction */
    using QIcon::QIcon;

    /** Trigger repaint of owning widget */
    void updateIcon();

private:
    const QString   _iconName;          /** Name of the icon */
    const QString   _iconFontName;      /** Name of the icon font */
    const Version   _iconFontVersion;   /** Version of the icon font */
    ThemeWatcher    _themeWatcher;      /** Use our own theme watcher (which does not emit paletteChanged(...) needlessly) */
};


}