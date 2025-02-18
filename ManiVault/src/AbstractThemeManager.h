// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"

#include <QObject>
#include <QPalette>

namespace mv::gui
{
    class TriggerAction;
    class ToggleAction;
    class OptionAction;
}

namespace mv {

/**
 * Abstract theme manager class
 *
 * This class provides an interface for the theme manager
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractThemeManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractThemeManager(QObject* parent) :
        AbstractManager(parent, "Theme")
    {
    }

    /**
     * Get whether the current theme is light
     * @return Boolean indicating whether the current theme is light
     */
    virtual bool isLight() const = 0;

    /**
     * Get whether the current theme is dark
     * @return Boolean indicating whether the current theme is dark
     */
    virtual bool isDark() const = 0;

    /** Set theme to light */
    virtual void setLight() = 0;

    /** Set theme to dark */
    virtual void setDark() = 0;

    /**
     * Add theme with \p themeName and \p themePalette
     * @param themeName Theme name
     * @param themePalette Theme palette
     */
    virtual void addCustomTheme(const QString& themeName, const QPalette& themePalette) = 0;

signals:

    /**
     * Signals that the application palette change to \p palette
     * @param palette Current palette
     */
    void applicationPaletteChanged(const QPalette& palette);

    /**
     * Signal emitted when the application theme changed
     * @param dark Boolean indicating whether the theme is dark
     */
    void themeChanged(bool dark);

    /** Signal emitted when the application theme changed to light */
    void themeChangedToLight();

    /** Signal emitted when the application theme changed to dark */
    void themeChangedToDark();

public: // Action getters

    virtual gui::ToggleAction& getUseSystemThemeAction() = 0;
    virtual gui::ToggleAction& getLightThemeAction() = 0;
    virtual gui::ToggleAction& getDarkThemeAction() = 0;
    virtual gui::OptionAction& getCustomThemeAction() = 0;
};

}
