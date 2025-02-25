// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ThemeManager.h"


#include <QStyleHints>

using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
    #define THEME_MANAGER_VERBOSE
#endif

namespace mv
{
ThemeManager::ThemeSettings::ThemeSettings(QObject* parent):
	QObject(parent),
	_colorSchemeMode(ColorSchemeMode::System),
	_colorScheme(Qt::ColorScheme::Unknown)
{
	_updateThemeTimer.setSingleShot(true);
	_updateThemeTimer.setInterval(1000);
	_updateThemeTimer.start();

	connect(&_updateThemeTimer, &QTimer::timeout, this, &ThemeSettings::updateTheme);
}

void ThemeManager::ThemeSettings::setColorSchemeMode(const ColorSchemeMode& colorSchemeMode)
{
	if (colorSchemeMode == _colorSchemeMode)
		return;

	_colorSchemeMode = colorSchemeMode;

	_updateThemeTimer.start();
}

void ThemeManager::ThemeSettings::setColorScheme(const Qt::ColorScheme& colorScheme)
{
	if (colorScheme == _colorScheme)
		return;

	_colorScheme = colorScheme;

	_updateThemeTimer.start();
}

void ThemeManager::ThemeSettings::setPalette(const QPalette& palette, const QString& paletteName)
{
	if (palette == _palette)
		return;

	_palette     = palette;
	_paletteName = paletteName;

	_updateThemeTimer.start();
}

void ThemeManager::ThemeSettings::updateTheme()
{
	switch (_colorSchemeMode) {
		case ColorSchemeMode::System:
		{
			qApp->styleHints()->setColorScheme(Qt::ColorScheme::Unknown);
			qApp->setPalette(QApplication::style()->standardPalette());
			break;
		}

		case ColorSchemeMode::SystemLightDark:
		{

			qApp->styleHints()->setColorScheme(_colorScheme);
			qApp->setPalette(QApplication::style()->standardPalette());

			mv::help().addNotification("Theme update", QString("<b>%1</b> system theme has been activated.").arg(_colorScheme == Qt::ColorScheme::Light ? "Light" : "Dark"), util::StyledIcon("palette"));
			break;
		}

		case ColorSchemeMode::Custom:
		{
			qApp->setPalette(_palette);
			mv::help().addNotification("Theme update", QString("Custom <b>%1</b> theme has been activated.").arg(_paletteName), util::StyledIcon("palette"));
			break;
		}
	}

	restyleAllWidgets();
	deleteLater();
}

void ThemeManager::ThemeSettings::restyleAllWidgets()
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

ThemeManager::ThemeManager(QObject* parent) :
    AbstractThemeManager(parent),
    _colorSchemeModeAction(this, "Theme", { "System", "Light/dark", "Custom" }),
    _systemLightColorSchemeAction(this, "Light", false),
    _systemDarkColorSchemeAction(this, "Dark", true),
    _customColorSchemeAction(this, "Custom"),
    _numberOfCommits(0),
    _systemColorScheme(Qt::ColorScheme::Unknown),
    _applicationColorScheme(Qt::ColorScheme::Unknown),
    _disableSystemLightColorSchemeSlot(false),
    _disableSystemDarkColorSchemeSlot(false)
{
}

ThemeManager::~ThemeManager()
{
    reset();
}

void ThemeManager::initialize()
{
#ifdef THEME_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractThemeManager::initialize();

    if (isInitialized())
        return;

    beginInitialization();
    {
        connect(&_colorSchemeModeAction, &OptionAction::currentIndexChanged, this, &ThemeManager::updateColorSchemeMode);

        connect(&_colorSchemeModeAction, &OptionAction::currentIndexChanged, this, [](const std::int32_t& currentIndex) -> void {
            if (currentIndex == static_cast<std::int32_t>(ColorSchemeMode::System))
				mv::help().addNotification("Theme update", "ManiVault Studio theme will synchronize with the current system theme.", util::StyledIcon("palette"));
        });

        connect(&_systemLightColorSchemeAction, &ToggleAction::toggled, this, [this](bool toggled) -> void {
            if (!toggled || _disableSystemLightColorSchemeSlot)
                return;

            privateActivateLightSystemColorScheme();
            setSystemDarkColorSchemeActionCheckedSilent(false);
        });

        connect(&_systemDarkColorSchemeAction, &ToggleAction::toggled, this, [this](bool toggled) -> void {
            if (!toggled || _disableSystemDarkColorSchemeSlot)
                return;

            privateActivateDarkSystemColorScheme();
            setSystemLightColorSchemeActionCheckedSilent(false);
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

            const auto currentSystemColorScheme = getCurrentSystemColorScheme();

            if (currentSystemColorScheme != _systemColorScheme) {
                privateActivateSystemColorScheme();

            	_systemColorScheme = currentSystemColorScheme;
            }
        });

        _detectSystemColorSchemeChangesTimer.start();

        addDefaultCustomThemes();

        _colorSchemeModeAction.setSettingsPrefix(getSettingsPrefix() + "ColorSchemeMode");
        _systemLightColorSchemeAction.setSettingsPrefix(getSettingsPrefix() + "Light");
        _systemDarkColorSchemeAction.setSettingsPrefix(getSettingsPrefix() + "Dark");
        _customColorSchemeAction.setSettingsPrefix(getSettingsPrefix() + "Custom");

        _customColorSchemeAction.setStretch(1);
        _customColorSchemeAction.setPlaceHolderString("Pick custom theme...");

        updateColorSchemeMode();
    }
    endInitialization();
}

void ThemeManager::reset()
{
#ifdef THEME_MANAGER_VERBOSE
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
    return _colorSchemeModeAction.getCurrentText() == "Light/dark";
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
    _colorSchemeModeAction.setCurrentText("Light/dark");
}

void ThemeManager::activateLightSystemColorScheme()
{
    _colorSchemeModeAction.setCurrentText("Light/dark");
    _systemLightColorSchemeAction.setChecked(true);
}

void ThemeManager::activateDarkSystemColorScheme()
{
    _colorSchemeModeAction.setCurrentText("Light/dark");
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

    connect(_requestThemeSettings, &QObject::destroyed, this, &AbstractThemeManager::colorSchemeChanged);

	return _requestThemeSettings;
}

void ThemeManager::updateColorSchemeMode()
{
    const auto currentColorSchemeMode = static_cast<ColorSchemeMode>(_colorSchemeModeAction.getCurrentIndex());

    _systemLightColorSchemeAction.setEnabled(currentColorSchemeMode == ColorSchemeMode::SystemLightDark);
    _systemDarkColorSchemeAction.setEnabled(currentColorSchemeMode == ColorSchemeMode::SystemLightDark);
    _customColorSchemeAction.setEnabled(currentColorSchemeMode == ColorSchemeMode::Custom);

    switch (currentColorSchemeMode) {
	    case ColorSchemeMode::System:
	    {
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
}

Qt::ColorScheme ThemeManager::getCurrentSystemColorScheme()
{
#ifdef Q_OS_WIN
    const auto settings = QSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
    return settings.value("AppsUseLightTheme", 1).toInt() == 0 ? Qt::ColorScheme::Dark : Qt::ColorScheme::Light;
#else
    return QGuiApplication::styleHints()->colorScheme();
#endif
}

void ThemeManager::privateActivateSystemColorScheme()
{
#ifdef THEME_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    getRequestThemeSettings()->setColorSchemeMode(ColorSchemeMode::System);
    getRequestThemeSettings()->setColorScheme(getCurrentSystemColorScheme());
}

void ThemeManager::privateActivateSystemColorSchemeLightDark()
{
#ifdef THEME_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (_systemDarkColorSchemeAction.isChecked())
        privateActivateDarkSystemColorScheme();

    if (_systemLightColorSchemeAction.isChecked())
        privateActivateLightSystemColorScheme();
}

void ThemeManager::privateActivateLightSystemColorScheme()
{
#ifdef THEME_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (isSystemColorSchemeModeActive() || isSystemLightDarkColorSchemeModeActive()) {
        getRequestThemeSettings()->setColorSchemeMode(ColorSchemeMode::SystemLightDark);
        getRequestThemeSettings()->setColorScheme(Qt::ColorScheme::Light);
    }
}

void ThemeManager::privateActivateDarkSystemColorScheme()
{
#ifdef THEME_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (isSystemColorSchemeModeActive() || isSystemLightDarkColorSchemeModeActive()) {
        getRequestThemeSettings()->setColorSchemeMode(ColorSchemeMode::SystemLightDark);
        getRequestThemeSettings()->setColorScheme(Qt::ColorScheme::Dark);
    }
}

void ThemeManager::privateActivateCustomColorScheme()
{
#ifdef THEME_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const auto currentCustomPaletteName = _customColorSchemeAction.getCurrentText();

    if (isCustomColorSchemeModeActive() && _customPalettes.contains(currentCustomPaletteName)) {
        getRequestThemeSettings()->setColorSchemeMode(ColorSchemeMode::Custom);
        getRequestThemeSettings()->setPalette(_customPalettes[currentCustomPaletteName], currentCustomPaletteName);
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
