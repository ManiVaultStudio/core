// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <AbstractThemeManager.h>

#include <QTimer>

namespace mv
{

class ThemeManager : public mv::AbstractThemeManager
{
    Q_OBJECT

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    ThemeManager(QObject* parent);

    /** Reset when destructed */
    ~ThemeManager() override;

    /** Perform event manager startup initialization */
    void initialize() override;

    /** Resets the contents of the event manager */
    void reset() override;

    /**
     * Get whether the system color scheme mode is active
     * @return Boolean indicating whether the system color scheme mode is active
     */
    bool isSystemColorSchemeModeActive() const override;

    /**
     * Get whether the system light/dark color scheme mode is active
     * @return Boolean indicating whether the system light/dark color scheme mode is active
     */
    bool isSystemLightDarkColorSchemeModeActive() const override;

    /**
    * Get whether the light system color scheme is active
    * @return Boolean indicating whether the light system color scheme is active (also false when the system theme is not active)
    */
    bool isSystemLightColorSchemeActive() const override;

    /**
     * Get whether the dark system color scheme is active
     * @return Boolean indicating whether the dark system color scheme is active (also false when the system theme is not active)
     */
    bool isSystemDarkColorSchemeActive() const override;

    /**
     * Get whether a custom color scheme mode is active
     * @return Boolean indicating whether a custom color scheme mode is active
     */
    bool isCustomColorSchemeModeActive() const override;

    /** Set color scheme mode to system */
    void activateSystemColorScheme() override;

    /** Set color scheme mode to system light/dark */
    void activateSystemColorSchemeLightDark() override;

    /** Set color scheme to system light */
    void activateLightSystemColorScheme() override;

    /** Set theme to system dark */
    void activateDarkSystemColorScheme() override;

    /** Activates the currently selected custom color scheme */
    void activateCustomColorScheme() override;

    /**
     * Activate custom color scheme with \p customThemeName (this will override the system color scheme)
     * @param customColorSchemeName Custom color scheme name
     */
    void activateCustomColorScheme(const QString& customColorSchemeName) override;

    /**
     * Get custom theme names
     * @return List of non-system theme names (built-in and added)
     */
    QStringList getCustomThemeNames() const override;

    /**
     * Add theme with \p themeName and \p themePalette
     * @param themeName Theme name
     * @param themePalette Theme palette
     */
    void addCustomTheme(const QString& themeName, const QPalette& themePalette) override;

private:

    /**
     * Override to handle palette and theme changes
     * @param event Pointer to event
     * @return Boolean indicating whether the event was handled
     */
    bool event(QEvent* event) override;

    /** Add default custom themes */
    void addDefaultCustomThemes();

    /** Restyles all widgets in the application */
    void restyleAllWidgets() const;

    /** Synchronizes the states of all actions */
    void requestChanges();

    /** Updates the styling of all widgets and notifies the user of the change */
    void commitChanges();

    /**
     * Set the application palette to \p palette
     * @param palette Application palette
     */
    void setPalette(const QPalette& palette);

protected: // Action getters

    gui::OptionAction& getColorSchemeModeAction() override { return _colorSchemeModeAction; }
    gui::ToggleAction& getSystemLightColorSchemeAction() override { return _systemLightColorSchemeAction; }
    gui::ToggleAction& getSystemDarkColorSchemeAction() override { return _systemDarkColorSchemeAction; }
    gui::OptionAction& getCustomColorSchemeAction() override { return _customColorSchemeAction; }

public: // Action getters

    const gui::OptionAction& getColorSchemeModeAction() const override { return _colorSchemeModeAction; }
    const gui::ToggleAction& getSystemLightColorSchemeAction() const override { return _systemLightColorSchemeAction; }
    const gui::ToggleAction& getSystemDarkColorSchemeAction() const override { return _systemDarkColorSchemeAction; }
    const gui::OptionAction& getCustomColorSchemeAction() const override { return _customColorSchemeAction; }

private:
    gui::OptionAction           _colorSchemeModeAction;                 /** For selecting the color scheme mode */
    gui::ToggleAction           _systemLightColorSchemeAction;          /** Set to light system color scheme when triggered */
    gui::ToggleAction           _systemDarkColorSchemeAction;           /** Set to dark system color scheme when triggered */
    gui::OptionAction           _customColorSchemeAction;               /** Custom color scheme action  */
    QMap<QString, QPalette>     _customThemes;                          /** Custom themes */
    QPalette                    _currentPalette;                        /** Current application palette */
    QTimer                      _requestChangesTimer;                   /** Timer for processing requested changes */
    QTimer                      _systemColorSchemeChangesTimer;         /** QStyleHints::colorSchemeChanged is sometimes unreliable: check periodically for system color scheme changes */
    std::int32_t                _numberOfCommits;                       /** Number of commits */
    Qt::ColorScheme             _currentColorScheme;                    /** Current color scheme (valid in system color scheme mode) */
};

}
