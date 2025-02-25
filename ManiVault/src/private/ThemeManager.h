// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <AbstractThemeManager.h>

#include <QStyleFactory>
#include <QStyleHints>
#include <QTimer>

namespace mv
{

class ThemeManager : public mv::AbstractThemeManager
{
    Q_OBJECT

public:

    class ThemeSettings : public QObject
    {
    public:

        /**
         * Construct theme settings with pointer to \p parent object
         * @param parent Pointer to parent theme manager
         */
        ThemeSettings(QObject* parent = nullptr);

        /**
         * Set color scheme mode to \p colorSchemeMode
         * @param colorSchemeMode Color scheme mode
         */
        void setColorSchemeMode(const ColorSchemeMode& colorSchemeMode);

        /**
         * Set color scheme to \p colorScheme
         * @param colorScheme Color scheme
         */
        void setColorScheme(const Qt::ColorScheme& colorScheme);

        /**
         * Set palette to \p palette
         * @param palette Palette
         * @param paletteName Palette name
         */
        void setPalette(const QPalette& palette, const QString& paletteName);

        /** Update theme, restyle all widgets and self-destruct */
        void updateTheme();

        /** Re-styles all widgets so they are properly styled */
        static void restyleAllWidgets();

    private:
        ColorSchemeMode     _colorSchemeMode;       /** Type of color scheme mode*/
        Qt::ColorScheme     _colorScheme;           /** Color scheme */
        QPalette            _palette;               /** Custom application palette */
        QString             _paletteName;           /** Custom palette name */
        QTimer              _updateThemeTimer;      /** Apply theme on timeout and re-start when settings change */
    };

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

    /** Set color scheme to system light (this will activate the system light/dark color scheme mode) */
    void activateLightSystemColorScheme() override;

    /** Set theme to system dark (this will activate the system light/dark color scheme mode) */
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

    /** Add default custom themes */
    void addDefaultCustomThemes();

    /**
     * Set color scheme mode to system and \p forceRedraw 
     */
    void privateActivateSystemColorScheme();

    /**
     * Set color scheme mode to system light/dark and \p forceRedraw
     */
    void privateActivateSystemColorSchemeLightDark();

    /**
     * Set color scheme to system light and \p forceRedraw
     */
    void privateActivateLightSystemColorScheme();

    /**
     * Set color scheme to system dark and \p forceRedraw
     */
    void privateActivateDarkSystemColorScheme();

    /**
     * Set color scheme to custom and \p forceRedraw
     */
    void privateActivateCustomColorScheme() ;

    /**
     * Set the system light color scheme action to \p checked without emitting signals
     * @param checked Checked state
     */
    void setSystemLightColorSchemeActionCheckedSilent(bool checked);

    /**
     * Set the system dark color scheme action to \p checked without emitting signals
     * @param checked Checked state
     */
    void setSystemDarkColorSchemeActionCheckedSilent(bool checked);

    /**
     * Get the requested theme settings for the next timeout of the ThemeManager::_updateThemeTimer
     * @return Requested theme settings
     */
    ThemeSettings* getRequestThemeSettings();

    /** Processes the current color scheme */
    void updateColorSchemeMode();

#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    static Qt::ColorScheme getCurrentSystemColorScheme();
#endif

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
    QMap<QString, QPalette>     _customPalettes;                        /** Custom color schemes */
    QTimer                      _detectSystemColorSchemeChangesTimer;   /** QStyleHints::colorSchemeChanged is sometimes unreliable: check periodically for system color scheme changes */
    Qt::ColorScheme             _systemColorScheme;                     /** System color scheme */
    Qt::ColorScheme             _applicationColorScheme;                /** Application color scheme */
    bool                        _disableSystemLightColorSchemeSlot;     /** Disable system light slot */
    bool                        _disableSystemDarkColorSchemeSlot;      /** Disable system dark slot */
    QPointer<ThemeSettings>     _requestThemeSettings;                  /** Requested theme settings for the next timeout of the ThemeManager::_updateThemeTimer */
};

}
