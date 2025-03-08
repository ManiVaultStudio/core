// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ThemeManager.h"

#ifdef Q_OS_MACX
	#include "util/MacThemeHelper.h"
#endif

#include <models/ColorSchemesFilterModel.h>

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
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
			qApp->styleHints()->setColorScheme(Qt::ColorScheme::Unknown);
			qApp->setPalette(QApplication::style()->standardPalette());
#endif

#ifdef Q_OS_MACOS
            macSetToAutoTheme();
#endif
			break;
		}

		case ColorSchemeMode::SystemLightDark:
		{
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
			qApp->styleHints()->setColorScheme(_colorScheme);
			qApp->setPalette(QApplication::style()->standardPalette());
#endif

#ifdef Q_OS_MACOS
            switch (_colorScheme) {
	            case Qt::ColorScheme::Light:
	                macSetToLightTheme();
	                break;

	            case Qt::ColorScheme::Dark:
	                macSetToDarkTheme();
	                break;

                case Qt::ColorScheme::Unknown:
                    break;
            }
#endif

			mv::help().addNotification("Theme update", QString("<b>%1</b> system theme has been activated.").arg(_colorScheme == Qt::ColorScheme::Light ? "Light" : "Dark"), util::StyledIcon("palette"));
			break;
		}

		case ColorSchemeMode::Custom:
		{
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
			qApp->setPalette(_palette);
			mv::help().addNotification("Theme update", QString("Custom <b>%1</b> theme has been activated.").arg(_paletteName), util::StyledIcon("palette"));
#endif
			break;
		}
	}

	AbstractThemeManager::restyleAllWidgets();
	deleteLater();
}

ThemeManager::ThemeManager(QObject* parent) :
    AbstractThemeManager(parent),
    _colorSchemeModeAction(this, "Theme"),
    _systemLightColorSchemeAction(this, "Light", false),
    _systemDarkColorSchemeAction(this, "Dark", true),
    _colorSchemeAction(this, "Custom"),
    _systemColorScheme(Qt::ColorScheme::Unknown),
    _applicationColorScheme(Qt::ColorScheme::Unknown),
    _disableSystemLightColorSchemeSlot(false),
    _disableSystemDarkColorSchemeSlot(false)
{
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    _colorSchemeModeAction.setOptions({ "System", "Light/dark", "Custom" });
#endif

#ifdef Q_OS_MACOS
    _colorSchemeModeAction.setOptions({ "System", "Light/dark" });
#endif
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

    	connect(&_colorSchemeAction.getCurrentColorSchemeAction(), &OptionAction::currentTextChanged, this, [this]() -> void {
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

        _colorSchemeAction.initialize();

        _systemLightColorSchemeAction.setSettingsPrefix(getSettingsPrefix() + "ColorScheme/LightDark/Light");
        _systemDarkColorSchemeAction.setSettingsPrefix(getSettingsPrefix() + "ColorScheme/LightDark/Dark");
        _colorSchemeAction.getCurrentColorSchemeAction().setSettingsPrefix(getSettingsPrefix() + "ColorScheme/Custom/Current");
        _colorSchemeModeAction.setSettingsPrefix(getSettingsPrefix() + "ColorScheme/Mode");

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
        return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Light;
    }

    return false;
}

bool ThemeManager::isSystemDarkColorSchemeActive() const
{
    if (isSystemColorSchemeModeActive() || isSystemLightDarkColorSchemeModeActive()) {
        return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
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

    _colorSchemeAction.getCurrentColorSchemeAction().setCurrentText(customColorSchemeName);
}

QStringList ThemeManager::getCustomColorSchemeNames(const util::ColorScheme::Modes& customColorSchemeModes) const
{
    ColorSchemesFilterModel colorSchemesFilterModel;

    colorSchemesFilterModel.setSourceModel(const_cast<ColorSchemesListModel*>(&_customColorSchemeListModel));

    QStringList customColorSchemeModeNames;

    for (const auto& customColorSchemeMode : customColorSchemeModes) {
        customColorSchemeModeNames << ColorScheme::modeNames[customColorSchemeMode];
    }

	colorSchemesFilterModel.getModeFilterAction().setSelectedOptions(customColorSchemeModeNames);

    QStringList customColorSchemeNames;

    for (int rowIndex = 0; rowIndex < colorSchemesFilterModel.rowCount(); ++rowIndex) {
        const auto sourceNameIndex  = colorSchemesFilterModel.mapToSource(colorSchemesFilterModel.index(rowIndex, static_cast<int>(ColorSchemesListModel::Column::Name)));
        const auto colorSchemeName  = getCustomColorSchemeListModel().data(sourceNameIndex).toString();

        customColorSchemeNames << colorSchemeName;
    }

    return customColorSchemeNames;
}

ColorSchemesMap ThemeManager::getCustomColorSchemes(const util::ColorScheme::Modes& customColorSchemeModes) const
{
    ColorSchemesFilterModel colorSchemesFilterModel;

    colorSchemesFilterModel.setSourceModel(const_cast<ColorSchemesListModel*>(&_customColorSchemeListModel));

    QStringList customColorSchemeModeNames;

    for (const auto& customColorSchemeMode : customColorSchemeModes) {
        customColorSchemeModeNames << ColorScheme::modeNames[customColorSchemeMode];
    }

    colorSchemesFilterModel.getModeFilterAction().setSelectedOptions(customColorSchemeModeNames);

    ColorSchemesMap colorSchemes;

    for (int rowIndex = 0; rowIndex < colorSchemesFilterModel.rowCount(); ++rowIndex) {
        const auto sourceNameIndex  = colorSchemesFilterModel.mapToSource(colorSchemesFilterModel.index(rowIndex, static_cast<int>(ColorSchemesListModel::Column::Name)));

        if (const auto colorSchemeItem = dynamic_cast<AbstractColorSchemesModel::Item*>(getCustomColorSchemeListModel().itemFromIndex(sourceNameIndex)))
			colorSchemes[colorSchemeItem->getColorScheme().getName()] = colorSchemeItem->getColorScheme();
    }

    return colorSchemes;
}

void ThemeManager::addCustomColorScheme(const util::ColorScheme& colorScheme)
{
}

const ColorSchemesListModel& ThemeManager::getCustomColorSchemeListModel() const
{
    return _customColorSchemeListModel;
}

ColorSchemesListModel& ThemeManager::getCustomColorSchemeListModel()
{
    return _customColorSchemeListModel;
}

/*
QStringList ThemeManager::getCustomColorSchemeNames(const CustomColorSchemeModes& customColorSchemeModes ) const
{
    QStringList customColorSchemeNames;

    for (const auto& customColorSchemeMode : customColorSchemeModes)
        customColorSchemeNames << _customColorSchemes[customColorSchemeMode].keys();

    return customColorSchemeNames;
}

AbstractThemeManager::CustomColorSchemesMap ThemeManager::getCustomColorSchemes(const CustomColorSchemeModes& customColorSchemeModes) const
{
    CustomColorSchemesMap customColorSchemesMap;

    for (const auto& customColorSchemeMode : customColorSchemeModes) {
        for (const auto& customColorSchemeName : _customColorSchemes[customColorSchemeMode].keys())
			customColorSchemesMap[customColorSchemeName] = _customColorSchemes[customColorSchemeMode][customColorSchemeName];
    }

    return customColorSchemesMap;
}

void ThemeManager::addCustomColorScheme(const CustomColorSchemeMode& mode, const QString& name, const QPalette& palette)
{
#ifdef THEME_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << mode << name << palette;
#endif

    _customColorSchemes[mode][name] = palette;

    _colorSchemeAction.getCurrentColorSchemeAction().setOptions(getCustomColorSchemeNames());
}
*/



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
    _colorSchemeAction.setEnabled(currentColorSchemeMode == ColorSchemeMode::Custom);

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

    const auto currentCustomColorSchemeName = _colorSchemeAction.getCurrentColorSchemeAction().getCurrentText();
    const auto customColorSchemes           = getCustomColorSchemes();

    if (isCustomColorSchemeModeActive() && customColorSchemes.keys().contains(currentCustomColorSchemeName)) {
        getRequestThemeSettings()->setColorSchemeMode(ColorSchemeMode::Custom);
        getRequestThemeSettings()->setPalette(customColorSchemes[currentCustomColorSchemeName].getPalette(), currentCustomColorSchemeName);
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
