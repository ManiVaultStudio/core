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

protected:

	using PaletteGetter = std::function<QPalette()>;

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
     * Get whether system theming is active
     * @return Boolean determining whether system theming is active
     */
    bool isSystemThemingActive() const override;

    /** Activates system theming */
    void activateSystemTheming() override;

    /**
     * Activates system theming with \p colorScheme
     * @param colorScheme Color scheme
     */
    void activateSystemTheming(const Qt::ColorScheme& colorScheme) override;

    /** De-activates the system theme (switches the application palette) */
    void deactivateSystemTheme() override;

    /**
     * Get whether the light system color scheme is active
     * @return Boolean indicating whether the light system color scheme is active (also false when the system theme is not active)
     */
    bool isLightColorSchemeActive() const override;

    /**
     * Get whether the dark system color scheme is active
     * @return Boolean indicating whether the dark system color scheme is active (also false when the system theme is not active)
     */
    bool isDarkColorSchemeActive() const override;

    /** Set theme to system light */
    void activateLightSystemTheme() override;

    /** Set theme to system dark */
    void activateDarkSystemTheme() override;

    /**
     * Set custom theme to theme with \p customThemeName (this will override the system theme)
     * @param customThemeName Custom theme name
     */
    void activateCustomTheme(const QString& customThemeName) override;

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
     * Handle \p event
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

public: // Action getters

    gui::ToggleAction& getUseSystemThemeAction() override { return _useSystemThemeAction; }
    gui::ToggleAction& getLightThemeAction() override { return _lightSystemThemeAction; }
    gui::ToggleAction& getDarkThemeAction() override { return _darkSystemThemeAction; }
    gui::OptionAction& getCustomThemeAction() override { return _customThemeAction; }

private:
    gui::ToggleAction           _useSystemThemeAction;              /** Toggle action for toggling the learning center */
    gui::ToggleAction           _lightSystemThemeAction;            /** Set to light system theme when triggered */
    gui::ToggleAction           _darkSystemThemeAction;             /** Set to dark theme when triggered */
    gui::OptionAction           _customThemeAction;                 /** Set to dark theme when triggered */
    QMap<QString, QPalette>     _customThemes;                      /** Custom themes */
    QPalette                    _currentPalette;                    /** Current application palette */
    QTimer                      _requestChangesTimer;               /** Timer for processing requested changes */
    QTimer                      _detectColorSchemeChangeTimer;      /** QStyleHints::colorSchemeChanged is sometimes unreliable: check periodically for changes */
    std::int32_t                _numberOfCommits;                   /** Number of commits */
};

}
