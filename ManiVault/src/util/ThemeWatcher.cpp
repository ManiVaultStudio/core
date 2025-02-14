// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ThemeWatcher.h"
#include "Application.h"

#include <QDebug>
#include <QGuiApplication>
#include <QStyleHints>

namespace mv::util
{

ThemeWatcher::ThemeWatcher(QObject* parent /*= nullptr*/) :
    QObject(parent)
{
    if (!Application::current()) {
        qWarning() << "ThemeWatcher: Application is not available";
        return;
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, this, [this](Qt::ColorScheme scheme) -> void {
        const auto isDark = scheme == Qt::ColorScheme::Dark;

        emit themeChanged(isDark);

    	if (isDark)
            emit themeChangedToDark();
        else
            emit themeChangedToLight();
	});
#else
    connect(Application::current(), &Application::paletteChanged, this, [this](const QPalette& palette) -> void {
        const auto isDark = ;

        emit themeChanged(isDark);

        if (isDark)
            emit themeChangedToDark();
        else
            emit themeChangedToLight();
    });
#endif
}

bool ThemeWatcher::isDark() const
{
	return QApplication::palette().color(QPalette::Window).lightness() < 128;
}

bool ThemeWatcher::isLight() const
{
	return !isDark();
}

}
