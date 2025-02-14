// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QObject>
#include <QPalette>

namespace mv::util
{
	
/**
 * Theme watcher utility class
 *
 * Helper class for responding to application palette (theme) changes.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ThemeWatcher : public QObject
{
    Q_OBJECT

public: // Construction/destruction

    /**
     * Construct with pointer to \p parent object
     * @param parent Parent object (maybe nullptr)
     */
    ThemeWatcher(QObject* parent = nullptr);

    /**
     * Get whether the theme is dark
     * @return Boolean indicating whether the theme is dark
     */
    bool isDark() const;

    /**
     * Get whether the theme is light
     * @return Boolean indicating whether the theme is light
     */
    bool isLight() const;

signals:

    /**
     * Signal emitted when the application theme changed
     * @param dark Boolean indicating whether the theme is dark
     */
	void themeChanged(bool dark);

    /** Signal emitted when the application theme changed to light */
    void themeChangedToLight();

    /** Signal emitted when the application theme changed to dark */
    void themeChangedToDark();
};

}