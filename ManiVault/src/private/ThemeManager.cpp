// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ThemeManager.h"

#include <QStyleFactory>

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
    _numberOfCommits(0)
{
    _colorSchemeModeAction.setSettingsPrefix(getSettingsPrefix() + "ColorSchemeMode");
    _systemLightColorSchemeAction.setSettingsPrefix(getSettingsPrefix() + "Light");
    _systemDarkColorSchemeAction.setSettingsPrefix(getSettingsPrefix() + "Dark");
    _customColorSchemeAction.setSettingsPrefix(getSettingsPrefix() + "Custom");

    _customColorSchemeAction.setStretch(1);
    _customColorSchemeAction.setPlaceHolderString("Pick custom theme...");

    _requestChangesTimer.setSingleShot(true);
    _requestChangesTimer.setInterval(1000);

    connect(&_colorSchemeModeAction, &OptionAction::currentIndexChanged, this, [this](std::int32_t currentIndex) -> void {
        emit colorSchemeModeChanged(static_cast<ColorSchemeMode>(currentIndex));
    });

    connect(&_requestChangesTimer, &QTimer::timeout, this, [this]() -> void {
        commitChanges();
	});

    /*_detectColorSchemeChangeTimer.setSingleShot(true);
    _detectColorSchemeChangeTimer.setInterval(1000);

    connect(&_detectColorSchemeChangeTimer, &QTimer::timeout, this, [this]() -> void {
        requestChanges();
	});*/
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
//#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
//        connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, this, [this](Qt::ColorScheme scheme) -> void {
//#ifdef THEME_MANAGER_VERBOSE
//            qDebug() << "QStyleHints::colorSchemeChanged()" << scheme;
//#endif
//
//            emit applicationPaletteChanged(qApp->palette());
//
//            requestChanges();
//        });
//#endif

        connect(&_colorSchemeModeAction, &OptionAction::currentIndexChanged, this, [this](std::int32_t currentIndex) -> void {
#ifdef THEME_MANAGER_VERBOSE
            qDebug() << "mv::ThemeManager::_colorSchemeModeAction::currentIndexChanged()" << currentIndex;
#endif

            if (!isSystemLightColorSchemeActive() && !isSystemDarkColorSchemeActive())
                activateLightSystemColorScheme();

            requestChanges();
		});

        connect(&_systemLightColorSchemeAction, &ToggleAction::toggled, this, [this](bool toggled) -> void {
#ifdef THEME_MANAGER_VERBOSE
            qDebug() << "mv::ThemeManager::_systemLightColorSchemeAction::toggled()" << toggled;
#endif

            //if (toggled) {
            //    QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);

            //    requestChanges();
            //}
        });

        connect(&_systemDarkColorSchemeAction, &ToggleAction::toggled, this, [this](bool toggled) -> void {
#ifdef THEME_MANAGER_VERBOSE
            qDebug() << "mv::ThemeManager::_systemDarkColorSchemeAction::toggled()" << toggled;
#endif

            //if (toggled) {
            //    QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Dark);

            //    requestChanges();
            //}
        });

        connect(&_customColorSchemeAction, &OptionAction::currentTextChanged, this, [this](const QString& currentTheme) -> void {
#ifdef THEME_MANAGER_VERBOSE
            qDebug() << "mv::ThemeManager::_customColorSchemeAction::currentTextChanged()" << currentTheme;
#endif

            if (_customThemes.contains(currentTheme)) {
	            qApp->setPalette(_customThemes[currentTheme]);
                qApp->setStyle(QStyleFactory::create("Fusion"));

                emit applicationPaletteChanged(qApp->palette());

                requestChanges();
            }
        });

        addDefaultCustomThemes();
        requestChanges();
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
    if (isSystemColorSchemeModeActive()) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Light;
#else
        return QApplication::palette().color(QPalette::Window).lightness() >= 128;
#endif
    }

    return _systemLightColorSchemeAction.isChecked();
}

bool ThemeManager::isSystemDarkColorSchemeActive() const
{
    if (isSystemColorSchemeModeActive()) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
#else
        return QApplication::palette().color(QPalette::Window).lightness() < 128;
#endif
    }

    return _systemDarkColorSchemeAction.isChecked();
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
    _systemLightColorSchemeAction.setChecked(true);
}

void ThemeManager::activateDarkSystemColorScheme()
{
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
    return _customThemes.keys();
}

void ThemeManager::addCustomTheme(const QString& themeName, const QPalette& themePalette)
{
#ifdef THEME_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << themeName << themePalette;
#endif

    _customThemes[themeName] = themePalette;

    _customColorSchemeAction.setOptions(_customThemes.keys());
}

bool ThemeManager::event(QEvent* event)
{
    if (event->type() == QEvent::PaletteChange) {
#ifdef THEME_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << "QEvent::PaletteChange";
#endif

        auto currentPalette = QGuiApplication::palette();

        if (currentPalette != _currentPalette) {

            _currentPalette = currentPalette;

            emit applicationPaletteChanged(_currentPalette);

            restyleAllWidgets();
        }
    }

    if (event->type() == QEvent::ThemeChange) {
#ifdef THEME_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << "QEvent::ThemeChange";
#endif

        emit applicationPaletteChanged(_currentPalette);

        restyleAllWidgets();
    }

	return AbstractThemeManager::event(event);
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

	_customColorSchemeAction.setOptions(_customThemes.keys());
}

void ThemeManager::restyleAllWidgets() const
{
#ifdef THEME_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QTimer::singleShot(25, [this]() -> void
    {
        for (auto widget : QApplication::topLevelWidgets()) {
            widget->style()->unpolish(widget);
            widget->style()->polish(widget);
            widget->update();
            widget->repaint();
        }
    });
}

void ThemeManager::requestChanges()
{
#ifdef THEME_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _systemLightColorSchemeAction.setEnabled(isSystemLightDarkColorSchemeModeActive());
    _systemDarkColorSchemeAction.setEnabled(isSystemLightDarkColorSchemeModeActive());
    _customColorSchemeAction.setEnabled(isCustomColorSchemeModeActive());

    if (isSystemLightDarkColorSchemeModeActive()) {
        _systemLightColorSchemeAction.setChecked(isSystemLightColorSchemeActive());
        _systemDarkColorSchemeAction.setChecked(isSystemDarkColorSchemeActive());
    }

    _requestChangesTimer.start();
}

void ThemeManager::commitChanges()
{
#ifdef THEME_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    restyleAllWidgets();

    if (_numberOfCommits++ == 0)
        return;
    
    //if (_useSystemColorSchemeAction.isChecked())
    //    mv::help().addNotification("Theme update", QString("<b>%1</b> system theme has been activated.").arg(isSystemLightColorSchemeActive() ? "Light" : "Dark"), StyledIcon("palette"));
    //else
    //    mv::help().addNotification("Theme update", QString("<b>%1</b> custom application theme has been activated.").arg(_customColorSchemeAction.getCurrentText()), StyledIcon("palette"));
}

}
