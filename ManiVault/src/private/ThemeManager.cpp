// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ThemeManager.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
	#include <QStyleHints>
#endif

using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
    //#define THEME_MANAGER_VERBOSE
#endif

namespace mv
{

ThemeManager::ThemeManager(QObject* parent) :
    AbstractThemeManager(parent),
    _useSystemThemeAction(this, "System"),
    _lightThemeAction(this, "Light"),
    _darkThemeAction(this, "Dark"),
    _customThemeAction(this, "Custom theme")
{
    _useSystemThemeAction.setSettingsPrefix(getSettingsPrefix() + "System");
    _lightThemeAction.setSettingsPrefix(getSettingsPrefix() + "Light");
    _darkThemeAction.setSettingsPrefix(getSettingsPrefix() + "Dark");
    _customThemeAction.setSettingsPrefix(getSettingsPrefix() + "Custom");

    _customThemeAction.setStretch(1);
    _customThemeAction.setPlaceHolderString("Pick custom theme...");
}

ThemeManager::~ThemeManager()
{
    reset();
}

void ThemeManager::initialize()
{
#ifdef HELP_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractThemeManager::initialize();

    if (isInitialized())
        return;

    //connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, this, [this](Qt::ColorScheme scheme) -> void {
    //    qDebug() << "Color scheme changed";
    //    });

    beginInitialization();
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, this, [this](Qt::ColorScheme scheme) -> void {
            const auto isDark = scheme == Qt::ColorScheme::Dark;

            emit themeChanged(isDark);

            if (isDark)
                emit themeChangedToDark();
            else
                emit themeChangedToLight();
        });
#endif

        connect(this, &AbstractThemeManager::themeChanged, this, [this](bool dark) -> void {
            for (auto widget : QApplication::allWidgets()) {
                widget->style()->unpolish(widget);
                widget->style()->polish(widget);

                widget->repaint();
            }
		});

        connect(&_useSystemThemeAction, &ToggleAction::toggled, this, [this](bool toggled) -> void {
            _lightThemeAction.setChecked(!toggled);
            _darkThemeAction.setChecked(!toggled);
		});

        connect(&_lightThemeAction, &ToggleAction::toggled, this, [this](bool toggled) -> void {
            if (!toggled)
                return;

            _useSystemThemeAction.setChecked(false);
            _darkThemeAction.setChecked(false);

            _customThemeAction.setCurrentText("Light");

            QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);
        });

        connect(&_darkThemeAction, &ToggleAction::toggled, this, [this](bool toggled) -> void {
            if (!toggled)
                return;

            _useSystemThemeAction.setChecked(false);
            _lightThemeAction.setChecked(false);

            _customThemeAction.setCurrentText("Dark");

            QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Dark);
        });

        connect(&_customThemeAction, &OptionAction::currentTextChanged, this, [this](const QString& currentTheme) -> void {
            _useSystemThemeAction.setChecked(false);
            _lightThemeAction.setChecked(false);
            _darkThemeAction.setChecked(false);

            if (_customThemes.contains(currentTheme)) {
	            qApp->setPalette(_customThemes[currentTheme]);

				for (auto widget : QApplication::allWidgets()) {
					widget->style()->unpolish(widget);
					widget->style()->polish(widget);

					widget->update();
					widget->repaint();
				}
            }
        });

        addDefaultCustomThemes();
    }
    endInitialization();
}

void ThemeManager::reset()
{
#ifdef HELP_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
    }
    endReset();
}

bool ThemeManager::isDark() const
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
#else
    return QApplication::palette().color(QPalette::Window).lightness() < 128;
#endif
}

void ThemeManager::setLight()
{
    _useSystemThemeAction.setChecked(false);
    _lightThemeAction.setChecked(true);
    _darkThemeAction.setChecked(false);
}

void ThemeManager::setDark()
{
    _useSystemThemeAction.setChecked(false);
    _lightThemeAction.setChecked(false);
    _darkThemeAction.setChecked(true);
}

bool ThemeManager::isLight() const
{
    return !isDark();
}

void ThemeManager::addCustomTheme(const QString& themeName, const QPalette& themePalette)
{
    _customThemes[themeName] = themePalette;

    _customThemeAction.setOptions(_customThemes.keys());
}

bool ThemeManager::event(QEvent* event)
{
    if (event->type() == QEvent::PaletteChange) {
        emit themeChanged(isDark());
        if (isDark())
            emit themeChangedToDark();
        else
            emit themeChangedToLight();
    }
	return AbstractThemeManager::event(event);
}

void ThemeManager::addDefaultCustomThemes()
{
    auto& palette = _customThemes["Dark"];

    palette.setColor(QPalette::Window, QColor(45, 45, 45));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(30, 30, 30));
    palette.setColor(QPalette::AlternateBase, QColor(45, 45, 45));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(60, 60, 60));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);

    palette = _customThemes["Light"];

    palette.setColor(QPalette::Window, Qt::white);
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, QColor(240, 240, 240));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, QColor(220, 220, 220));
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::BrightText, Qt::red);

    palette = _customThemes["Fusion light"];

    palette.setColor(QPalette::Window, Qt::white);
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, QColor(240, 240, 240));
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, QColor(220, 220, 220));
    palette.setColor(QPalette::ButtonText, Qt::black);

    palette = _customThemes["Fusion (dark)"];

    palette.setColor(QPalette::Window, QColor(45, 45, 45));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(30, 30, 30));
    palette.setColor(QPalette::AlternateBase, QColor(45, 45, 45));
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(60, 60, 60));
    palette.setColor(QPalette::ButtonText, Qt::white);

    palette = _customThemes["Google Material Design (dark)"];

    palette.setColor(QPalette::Window, QColor(33, 33, 33));
    palette.setColor(QPalette::WindowText, QColor(220, 220, 220));
    palette.setColor(QPalette::Base, QColor(50, 50, 50));
    palette.setColor(QPalette::AlternateBase, QColor(40, 40, 40));
    palette.setColor(QPalette::Text, QColor(220, 220, 220));
    palette.setColor(QPalette::Button, QColor(50, 50, 50));
    palette.setColor(QPalette::ButtonText, QColor(220, 220, 220));

    palette = _customThemes["Google Material Design (light)"];

    palette.setColor(QPalette::Window, QColor(245, 245, 245));
    palette.setColor(QPalette::WindowText, QColor(0, 0, 0));
    palette.setColor(QPalette::Base, QColor(255, 255, 255));
    palette.setColor(QPalette::AlternateBase, QColor(240, 240, 240));
    palette.setColor(QPalette::Text, QColor(0, 0, 0));
    palette.setColor(QPalette::Button, QColor(230, 230, 230));
    palette.setColor(QPalette::ButtonText, QColor(0, 0, 0));

    palette = _customThemes["Dracula"];

    palette.setColor(QPalette::Window, QColor(40, 42, 54));
    palette.setColor(QPalette::WindowText, QColor(248, 248, 242));
    palette.setColor(QPalette::Base, QColor(68, 71, 90));
    palette.setColor(QPalette::AlternateBase, QColor(50, 50, 70));
    palette.setColor(QPalette::Text, QColor(248, 248, 242));
    palette.setColor(QPalette::Button, QColor(68, 71, 90));
    palette.setColor(QPalette::ButtonText, QColor(248, 248, 242));

    palette = _customThemes["Nord (Cold Minimalistic)"];

    palette.setColor(QPalette::Window, QColor(46, 52, 64));
    palette.setColor(QPalette::WindowText, QColor(216, 222, 233));
    palette.setColor(QPalette::Base, QColor(59, 66, 82));
    palette.setColor(QPalette::AlternateBase, QColor(76, 86, 106));
    palette.setColor(QPalette::Text, QColor(216, 222, 233));
    palette.setColor(QPalette::Button, QColor(76, 86, 106));
    palette.setColor(QPalette::ButtonText, QColor(216, 222, 233));

    palette = _customThemes["Solarized (dark)"];

    palette.setColor(QPalette::Window, QColor(0, 43, 54));
    palette.setColor(QPalette::WindowText, QColor(131, 148, 150));
    palette.setColor(QPalette::Base, QColor(7, 54, 66));
    palette.setColor(QPalette::AlternateBase, QColor(0, 43, 54));
    palette.setColor(QPalette::Text, QColor(147, 161, 161));
    palette.setColor(QPalette::Button, QColor(88, 110, 117));
    palette.setColor(QPalette::ButtonText, QColor(147, 161, 161));

    palette = _customThemes["Solarized (light)"];

    palette.setColor(QPalette::Window, QColor(253, 246, 227));
    palette.setColor(QPalette::WindowText, QColor(101, 123, 131));
    palette.setColor(QPalette::Base, QColor(238, 232, 213));
    palette.setColor(QPalette::AlternateBase, QColor(253, 246, 227));
    palette.setColor(QPalette::Text, QColor(88, 110, 117));
    palette.setColor(QPalette::Button, QColor(147, 161, 161));
    palette.setColor(QPalette::ButtonText, QColor(88, 110, 117));

    palette = _customThemes["Monokai"];

    palette.setColor(QPalette::Window, QColor(39, 40, 34));
    palette.setColor(QPalette::WindowText, QColor(248, 248, 242));
    palette.setColor(QPalette::Base, QColor(50, 50, 50));
    palette.setColor(QPalette::AlternateBase, QColor(60, 60, 60));
    palette.setColor(QPalette::Text, QColor(248, 248, 242));
    palette.setColor(QPalette::Button, QColor(70, 70, 70));
    palette.setColor(QPalette::ButtonText, QColor(248, 248, 242));

    palette = _customThemes["High contrast"];

    palette.setColor(QPalette::Window, Qt::black);
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, Qt::black);
    palette.setColor(QPalette::ButtonText, Qt::white);

    _customThemeAction.setOptions(_customThemes.keys());
}

}
