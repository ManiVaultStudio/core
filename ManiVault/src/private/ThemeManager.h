// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <AbstractThemeManager.h>

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
     * Get whether the current theme is light
     * @return Boolean indicating whether the current theme is light
     */
    bool isLight() const override;

    /**
     * Get whether the current theme is dark
     * @return Boolean indicating whether the current theme is dark
     */
    bool isDark() const override;

    /** Set theme to light */
    void setLight() override;

    /** Set theme to dark */
    void setDark() override;

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
    void restyleAllWidgets();

public: // Action getters

    gui::ToggleAction& getUseSystemThemeAction() override { return _useSystemThemeAction; }
    gui::ToggleAction& getLightThemeAction() override { return _lightThemeAction; }
    gui::ToggleAction& getDarkThemeAction() override { return _darkThemeAction; }
    gui::OptionAction& getCustomThemeAction() override { return _customThemeAction; }

private:
    gui::ToggleAction           _useSystemThemeAction;      /** Toggle action for toggling the learning center */
    gui::ToggleAction           _lightThemeAction;          /** Set to light theme when triggered */
    gui::ToggleAction           _darkThemeAction;           /** Set to dark theme when triggered */
    gui::OptionAction           _customThemeAction;         /** Set to dark theme when triggered */
    QMap<QString, QPalette>     _customThemes;              /** Custom themes */
    QPalette                    _currentPalette;            /** Current application palette */
};

}
