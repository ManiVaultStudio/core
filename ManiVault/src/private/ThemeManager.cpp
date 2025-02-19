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
    #define THEME_MANAGER_VERBOSE
#endif

namespace mv
{

ThemeManager::ThemeManager(QObject* parent) :
    AbstractThemeManager(parent),
    _colorSchemeModeAction(this, "Color scheme", { "System", "System light/dark", "Custom" }),
    _systemLightColorSchemeAction(this, "Light"),
    _systemDarkColorSchemeAction(this, "Dark"),
    _customColorSchemeAction(this, "Custom theme"),
    _numberOfCommits(0),
    _systemColorScheme(Qt::ColorScheme::Unknown),
    _applicationColorScheme(Qt::ColorScheme::Unknown),
    _disableSystemLightColorSchemeSlot(false),
    _disableSystemDarkColorSchemeSlot(false)
{
    _colorSchemeModeAction.setSettingsPrefix(getSettingsPrefix() + "ColorSchemeMode");
    //_systemLightColorSchemeAction.setSettingsPrefix(getSettingsPrefix() + "Light");
    //_systemDarkColorSchemeAction.setSettingsPrefix(getSettingsPrefix() + "Dark");
    //_customColorSchemeAction.setSettingsPrefix(getSettingsPrefix() + "Custom");

    //_systemLightColorSchemeAction.setDefaultWidgetFlags(ToggleAction::WidgetFlag::PushButton);
    //_systemDarkColorSchemeAction.setDefaultWidgetFlags(ToggleAction::WidgetFlag::PushButton);

    _customColorSchemeAction.setStretch(1);
    _customColorSchemeAction.setPlaceHolderString("Pick custom theme...");
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

    beginInitialization();
    {
        connect(&_colorSchemeModeAction, &OptionAction::currentIndexChanged, this, [this](const std::int32_t& currentIndex) -> void {
            switch (static_cast<ColorSchemeMode>(currentIndex)) {
				case ColorSchemeMode::System:
				{
                    _systemColorScheme = Qt::ColorScheme::Unknown;
				   privateActivateSystemColorScheme();
				   break;
				}

				case ColorSchemeMode::SystemLightDark:
				{
					privateActivateSystemColorSchemeLightDark();
				   break;
				}

                case ColorSchemeMode::Custom:
				{
					privateActivateCustomColorScheme();
				   break;
				}
            }
		});

        connect(&_systemLightColorSchemeAction, &ToggleAction::toggled, this, [this](bool toggled) -> void {
            if (!toggled || _disableSystemLightColorSchemeSlot || isSystemLightColorSchemeActive())
                return;

            _disableSystemDarkColorSchemeSlot = true;
            {
                privateActivateLightSystemColorScheme();
                _systemDarkColorSchemeAction.setChecked(false);
            }
            _disableSystemDarkColorSchemeSlot = false;
        });

        connect(&_systemDarkColorSchemeAction, &ToggleAction::toggled, this, [this](bool toggled) -> void {
            if (!toggled || _disableSystemDarkColorSchemeSlot || isSystemDarkColorSchemeActive())
                return;

            _disableSystemLightColorSchemeSlot = true;
            {
                privateActivateDarkSystemColorScheme();
                _systemLightColorSchemeAction.setChecked(false);
            }
            _disableSystemLightColorSchemeSlot = false;
        });

    	connect(&_customColorSchemeAction, &OptionAction::currentTextChanged, this, [this]() -> void {
            if (!isCustomColorSchemeModeActive())
                return;

            privateActivateCustomColorScheme();
		});

        _detectSystemColorSchemeChangesTimer.setInterval(1000);

        connect(&_detectSystemColorSchemeChangesTimer, &QTimer::timeout, this, [this]() -> void {
            if (!isSystemColorSchemeModeActive())
                return;

#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 2)

#ifdef Q_OS_WIN
            const auto settings = QSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
            const auto currentSystemColorScheme = settings.value("AppsUseLightTheme", 1).toInt() == 0 ? Qt::ColorScheme::Dark : Qt::ColorScheme::Light;
#else
            const auto currentSystemColorScheme = QGuiApplication::styleHints()->colorScheme();
#endif
            //qDebug() << "System color scheme changed" << _systemColorScheme << currentSystemColorScheme;

            if (currentSystemColorScheme != _systemColorScheme) {
                privateActivateSystemColorScheme();

                _systemColorScheme = QGuiApplication::styleHints()->colorScheme();
            }
#endif
        });

        _detectSystemColorSchemeChangesTimer.start();

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

bool ThemeManager::isSystemColorSchemeModeActive() const
{
    return _colorSchemeModeAction.getCurrentText() == "System";
}

bool ThemeManager::isSystemLightDarkColorSchemeModeActive() const
{
    return _colorSchemeModeAction.getCurrentText() == "System light/dark";
}

bool ThemeManager::isSystemLightColorSchemeActive() const
{
    if (isSystemColorSchemeModeActive() || isSystemLightDarkColorSchemeModeActive()) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Light;
#else
        return QApplication::palette().color(QPalette::Window).lightness() >= 128;
#endif
    }

    return false;
}

bool ThemeManager::isSystemDarkColorSchemeActive() const
{
    if (isSystemColorSchemeModeActive() || isSystemLightDarkColorSchemeModeActive()) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
#else
        return QApplication::palette().color(QPalette::Window).lightness() < 128;
#endif
    }

    return false;
}

bool ThemeManager::isCustomColorSchemeModeActive() const
{
    return _colorSchemeModeAction.getCurrentText() == "Custom";
}

void ThemeManager::activateSystemColorScheme()
{
    _colorSchemeModeAction.setCurrentText("System");
}

void ThemeManager::activateSystemColorSchemeLightDark()
{
    _colorSchemeModeAction.setCurrentText("System light/dark");
}

void ThemeManager::activateLightSystemColorScheme()
{
    _colorSchemeModeAction.setCurrentText("System light/dark");
    _systemLightColorSchemeAction.setChecked(true);
}

void ThemeManager::activateDarkSystemColorScheme()
{
    _colorSchemeModeAction.setCurrentText("System light/dark");
    _systemDarkColorSchemeAction.setChecked(true);
}

void ThemeManager::activateCustomColorScheme()
{
    _colorSchemeModeAction.setCurrentText("Custom");
}

void ThemeManager::activateCustomColorScheme(const QString& customColorSchemeName)
{
    activateCustomColorScheme();

    _customColorSchemeAction.setCurrentText(customColorSchemeName);
}

QStringList ThemeManager::getCustomThemeNames() const
{
    return _customPalettes.keys();
}

void ThemeManager::addCustomTheme(const QString& themeName, const QPalette& themePalette)
{
#ifdef THEME_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << themeName;
#endif

    _customPalettes[themeName] = themePalette;

    _customColorSchemeAction.setOptions(_customPalettes.keys());
}

void ThemeManager::addDefaultCustomThemes()
{
#ifdef THEME_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const auto addTheme = [this](QString name, QColor window, QColor windowText, QColor base, QColor alternateBase, QColor tooltipBase, QColor tooltipText, QColor text, QColor button, QColor buttonText, QColor brightText) -> void {
        auto palette = QPalette();

        palette.setColor(QPalette::Window, window);
        palette.setColor(QPalette::WindowText, windowText);
        palette.setColor(QPalette::Base, base);
        palette.setColor(QPalette::AlternateBase, alternateBase);
        palette.setColor(QPalette::ToolTipBase, tooltipBase);
        palette.setColor(QPalette::ToolTipText, tooltipText);
        palette.setColor(QPalette::Text, tooltipText);
        palette.setColor(QPalette::Button, button);
        palette.setColor(QPalette::ButtonText, buttonText);
        palette.setColor(QPalette::BrightText, brightText);

        addCustomTheme(name, palette);
    };

    addTheme("Dark", QColor(45, 45, 45), Qt::white, QColor(30, 30, 30), QColor(45, 45, 45), Qt::white, Qt::white, Qt::white, QColor(60, 60, 60), Qt::white, Qt::red);
    addTheme("Light", Qt::white, Qt::black, Qt::white, QColor(240, 240, 240), Qt::white, Qt::black, Qt::black, QColor(220, 220, 220), Qt::black, Qt::red);
    addTheme("Fusion (dark)", QColor(45, 45, 45), Qt::white, QColor(30, 30, 30), QColor(45, 45, 45), Qt::white, Qt::white, Qt::white, QColor(60, 60, 60), Qt::white, Qt::red);
    addTheme("Fusion (light)", Qt::white, Qt::black, Qt::white, QColor(240, 240, 240), Qt::white, Qt::black, Qt::black, QColor(220, 220, 220), Qt::black, Qt::red);
    addTheme("Google Material Design (dark)", QColor(33, 33, 33), QColor(220, 220, 220), QColor(50, 50, 50), QColor(40, 40, 40), QColor(50, 50, 50), QColor(220, 220, 220), QColor(220, 220, 220), QColor(50, 50, 50), QColor(220, 220, 220), QColor(220, 220, 220));
    addTheme("Google Material Design (light)", QColor(245, 245, 245), QColor(0, 0, 0), QColor(255, 255, 255), QColor(240, 240, 240), QColor(255, 255, 255), QColor(0, 0, 0), QColor(0, 0, 0), QColor(230, 230, 230), QColor(0, 0, 0), QColor(0, 0, 0));
    addTheme("Dracula", QColor(40, 42, 54), QColor(248, 248, 242), QColor(68, 71, 90), QColor(50, 50, 70), QColor(68, 71, 90), QColor(248, 248, 242), QColor(248, 248, 242), QColor(68, 71, 90), QColor(248, 248, 242), QColor(248, 248, 242));
    addTheme("Nord (Cold Minimalistic)", QColor(46, 52, 64), QColor(216, 222, 233), QColor(59, 66, 82), QColor(76, 86, 106), QColor(59, 66, 82), QColor(216, 222, 233), QColor(216, 222, 233), QColor(59, 66, 82), QColor(216, 222, 233), QColor(216, 222, 233));
    addTheme("Solarized (dark)", QColor(0, 43, 54), QColor(131, 148, 150), QColor(7, 54, 66), QColor(0, 43, 54), QColor(7, 54, 66), QColor(131, 148, 150), QColor(147, 161, 161), QColor(88, 110, 117), QColor(147, 161, 161), QColor(147, 161, 161));
    addTheme("Solarized (light)", QColor(253, 246, 227), QColor(101, 123, 131), QColor(238, 232, 213), QColor(253, 246, 227), QColor(238, 232, 213), QColor(101, 123, 131), QColor(88, 110, 117), QColor(147, 161, 161), QColor(88, 110, 117), QColor(88, 110, 117));
    addTheme("Monokai", QColor(39, 40, 34), QColor(248, 248, 242), QColor(50, 50, 50), QColor(60, 60, 60), QColor(50, 50, 50), QColor(248, 248, 242), QColor(248, 248, 242), QColor(70, 70, 70), QColor(248, 248, 242), QColor(248, 248, 242));
    addTheme("High contrast", Qt::black, Qt::white, Qt::black, Qt::white, Qt::black, Qt::white, Qt::white, Qt::black, Qt::white, Qt::red);

	_customColorSchemeAction.setOptions(_customPalettes.keys());
}

ThemeManager::ThemeSettings* ThemeManager::getRequestThemeSettings()
{
	if (_requestThemeSettings.isNull())
        _requestThemeSettings = new ThemeSettings();

	return _requestThemeSettings;
}

void ThemeManager::privateActivateSystemColorScheme(bool forceRedraw /*= false*/)
{
    getRequestThemeSettings()->setColorSchemeMode(ColorSchemeMode::System);
    getRequestThemeSettings()->setColorScheme(QGuiApplication::styleHints()->colorScheme());
}

void ThemeManager::privateActivateSystemColorSchemeLightDark(bool forceRedraw /*= false*/)
{
    if (_systemDarkColorSchemeAction.isChecked())
        privateActivateDarkSystemColorScheme();

    if (_systemLightColorSchemeAction.isChecked())
        privateActivateLightSystemColorScheme();
}

void ThemeManager::privateActivateLightSystemColorScheme(bool forceRedraw /*= false*/)
{
    if (isSystemLightDarkColorSchemeModeActive()) {
        getRequestThemeSettings()->setColorSchemeMode(ColorSchemeMode::SystemLightDark);
        getRequestThemeSettings()->setColorScheme(Qt::ColorScheme::Light);
    }
}

void ThemeManager::privateActivateDarkSystemColorScheme(bool forceRedraw /*= false*/)
{
    if (isSystemLightDarkColorSchemeModeActive()) {
        getRequestThemeSettings()->setColorSchemeMode(ColorSchemeMode::SystemLightDark);
        getRequestThemeSettings()->setColorScheme(Qt::ColorScheme::Dark);
    }
}

void ThemeManager::privateActivateCustomColorScheme(bool forceRedraw /*= false*/)
{
    const auto currentCustomPaletteName = _customColorSchemeAction.getCurrentText();

    if (isCustomColorSchemeModeActive() && _customPalettes.contains(currentCustomPaletteName)) {
        getRequestThemeSettings()->setColorSchemeMode(ColorSchemeMode::Custom);
        getRequestThemeSettings()->setPalette(_customPalettes[currentCustomPaletteName]);
    }
}

void ThemeManager::setSystemLightColorSchemeActionCheckedSilent(bool checked)
{
    _disableSystemLightColorSchemeSlot = true;
    {
        _systemLightColorSchemeAction.setChecked(checked);
    }
    _disableSystemLightColorSchemeSlot = false;
}

void ThemeManager::setSystemDarkColorSchemeActionCheckedSilent(bool checked)
{
    _disableSystemDarkColorSchemeSlot = true;
    {
        _systemDarkColorSchemeAction.setChecked(checked);
    }
    _disableSystemDarkColorSchemeSlot = false;
}

}
