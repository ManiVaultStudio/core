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

    /** Color scheme modes */
    enum class ColorSchemeMode {
        System,             /** Color scheme is determined by the system color scheme */
        SystemLightDark,    /** Color scheme is determined by the light/dark system color scheme */
        Custom              /** Color scheme is determined by a custom color scheme */
    };

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
     * Get the current color scheme
     * @return Color scheme mode
     */
    ColorSchemeMode getColorSchemeMode() const { return static_cast<ColorSchemeMode>(getColorSchemeModeAction().getCurrentIndex()); }

    /**
     * Set the color scheme mode to \p colorSchemeMode
     * @param colorSchemeMode Color scheme mode
     */
    void setColorSchemeMode(const ColorSchemeMode& colorSchemeMode)
    {
        if (colorSchemeMode == getColorSchemeMode())
            return;

	    getColorSchemeModeAction().setCurrentIndex(static_cast<int>(colorSchemeMode));
    }

    /**
     * Get whether the system color scheme mode is active
     * @return Boolean indicating whether the system color scheme mode is active
     */
    virtual bool isSystemColorSchemeModeActive() const = 0;

    /**
     * Get whether the system light/dark color scheme mode is active
     * @return Boolean indicating whether the system light/dark color scheme mode is active
     */
    virtual bool isSystemLightDarkColorSchemeModeActive() const = 0;

    /**
     * Get whether the light system color scheme is active
     * @return Boolean indicating whether the light system color scheme is active (also false when the system theme is not active)
     */
    virtual bool isSystemLightColorSchemeActive() const = 0;

    /**
     * Get whether the dark system color scheme is active
     * @return Boolean indicating whether the dark system color scheme is active (also false when the system theme is not active)
     */
    virtual bool isSystemDarkColorSchemeActive() const = 0;

    /**
     * Get whether a custom color scheme mode is active
     * @return Boolean indicating whether a custom color scheme mode is active
     */
    virtual bool isCustomColorSchemeModeActive() const = 0;

    /** Set color scheme mode to system */
    virtual void activateSystemColorScheme() = 0;

    /** Set color scheme mode to system light/dark */
    virtual void activateSystemColorSchemeLightDark() = 0;

    /** Set color scheme to system light */
    virtual void activateLightSystemColorScheme() = 0;

    /** Set theme to system dark */
    virtual void activateDarkSystemColorScheme() = 0;

    /** Activates the currently selected custom color scheme */
    virtual void activateCustomColorScheme() = 0;

    /**
     * Activate custom color scheme with \p customThemeName (this will override the system color scheme)
     * @param customColorSchemeName Custom color scheme name
     */
    virtual void activateCustomColorScheme(const QString& customColorSchemeName) = 0;

    /**
     * Get custom theme names
     * @return List of non-system theme names (built-in and added)
     */
    virtual QStringList getCustomThemeNames() const = 0;

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

    /**
     * Signals that the color scheme mode changed to \p colorSchemeMode
     * @param colorSchemeMode Color scheme mode
     */
    void colorSchemeModeChanged(const ColorSchemeMode& colorSchemeMode);

protected: // Action getters

	virtual gui::OptionAction& getColorSchemeModeAction() = 0;
    virtual gui::ToggleAction& getSystemLightColorSchemeAction() = 0;
    virtual gui::ToggleAction& getSystemDarkColorSchemeAction() = 0;
    virtual gui::OptionAction& getCustomColorSchemeAction() = 0;

public: // Action getters

    virtual const gui::OptionAction& getColorSchemeModeAction() const = 0;
    virtual const gui::ToggleAction& getSystemLightColorSchemeAction() const = 0;
    virtual const gui::ToggleAction& getSystemDarkColorSchemeAction() const = 0;
    virtual const gui::OptionAction& getCustomColorSchemeAction() const = 0;

    friend class mv::gui::ApplicationSettingsAction;
};

}
