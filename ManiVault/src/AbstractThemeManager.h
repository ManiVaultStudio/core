// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"

#include "AppearanceSettingsAction.h"

#include "actions/ColorSchemeAction.h"

#include "util/ColorScheme.h"

#include "models/ColorSchemesListModel.h"

#include <QObject>
#include <QApplication>

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

    

    /** Maps custom color scheme name to palette */
    using CustomColorSchemesMap = QMap<QString, QPalette>;

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractThemeManager(QObject* parent) :
        AbstractManager(parent, "Theme")
    {
    }

    /** Perform event manager startup initialization */
    void initialize() override
    {
        AbstractManager::initialize();

        getCustomColorSchemeListModel().addBuiltInColorSchemes();
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

    /** Set color scheme to system light (this will activate the system light/dark color scheme mode) */
    virtual void activateLightSystemColorScheme() = 0;

    /** Set theme to system dark (this will activate the system light/dark color scheme mode) */
    virtual void activateDarkSystemColorScheme() = 0;

    /** Activates the currently selected custom color scheme */
    virtual void activateCustomColorScheme() = 0;

    /**
     * Activate custom color scheme with \p customThemeName (this will override the system color scheme)
     * @param customColorSchemeName Custom color scheme name
     */
    virtual void activateCustomColorScheme(const QString& customColorSchemeName) = 0;

    /**
     * Get custom color scheme mode names for \p customColorSchemeModes
     * @return List of color scheme mode names for the custom color scheme modes
     */
    virtual QStringList getCustomColorSchemeNames(const util::ColorScheme::Modes& customColorSchemeModes = { util::ColorScheme::Mode::BuiltIn, util::ColorScheme::Mode::UserAdded }) const = 0;

    /**
     * Get custom color schemes for \p customColorSchemeModes
     * @param customColorSchemeModes Custom color scheme modes
     * @return Color schemes map
     */
    virtual util::ColorSchemesMap getCustomColorSchemes(const util::ColorScheme::Modes& customColorSchemeModes = { util::ColorScheme::Mode::BuiltIn, util::ColorScheme::Mode::UserAdded }) const = 0;

    /**
     * Add custom \p colorScheme
     * @param colorScheme Color scheme
     */
    virtual void addCustomColorScheme(const util::ColorScheme& colorScheme) = 0;

    /**
     * Get custom color schemes list model
     * @return Reference to custom color schemes list model
     */
    virtual const ColorSchemesListModel& getCustomColorSchemeListModel() const = 0;

    /** Restyles all widgets to implement a color scheme change */
    static void restyleAllWidgets()
    {
        QList<QWidget*> allWidgets;

        for (QWidget* window : QApplication::topLevelWidgets()) {
            allWidgets.append(window);
            allWidgets.append(window->findChildren<QWidget*>());
        }

        for (auto widget : allWidgets) {
            widget->style()->unpolish(widget);
            widget->style()->polish(widget);
            widget->update();
            widget->repaint();
        }
    }

protected:

    /**
     * Get custom color schemes list model
     * @return Reference to custom color schemes list model
     */
    virtual ColorSchemesListModel& getCustomColorSchemeListModel() = 0;

signals:

    /** Signals that the color scheme changed */
    void colorSchemeChanged();

    /**
     * Signals that the color scheme mode changed to \p colorSchemeMode
     * @param colorSchemeMode Color scheme mode
     */
    void colorSchemeModeChanged(const ColorSchemeMode& colorSchemeMode);

protected: // Action getters

    virtual gui::OptionAction& getColorSchemeModeAction() = 0;
    virtual gui::ToggleAction& getSystemLightColorSchemeAction() = 0;
    virtual gui::ToggleAction& getSystemDarkColorSchemeAction() = 0;
    virtual gui::ColorSchemeAction& getCustomColorSchemeAction() = 0;

public: // Action getters

    virtual const gui::OptionAction& getColorSchemeModeAction() const = 0;
    virtual const gui::ToggleAction& getSystemLightColorSchemeAction() const = 0;
    virtual const gui::ToggleAction& getSystemDarkColorSchemeAction() const = 0;
    virtual const gui::ColorSchemeAction& getCustomColorSchemeAction() const = 0;

    friend class gui::AppearanceSettingsAction;
};

}
