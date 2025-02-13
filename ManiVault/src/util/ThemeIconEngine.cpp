// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ThemeIconEngine.h"
#include "NamedIcon.h"

#include <QDebug>
#include <QPainter>

namespace mv::util
{

ThemeIconEngine::ThemeIconEngine(NamedIcon& namedIcon) :
	_colorRoleLightTheme(QPalette::Text),
	_colorRoleDarkTheme(QPalette::Text)
{
    namedIcon._iconEngine = this;
}

ThemeIconEngine::ThemeIconEngine(const QString& sha, const QPalette::ColorRole& colorRoleLightTheme, const QPalette::ColorRole& colorRoleDarkTheme) :
    _sha(sha),
    _colorRoleLightTheme(colorRoleLightTheme),
    _colorRoleDarkTheme(colorRoleDarkTheme)
{
}

ThemeIconEngine::ThemeIconEngine(const ThemeIconEngine& other) :
    ThemeIconEngine(other._sha, other._colorRoleLightTheme, other._colorRoleDarkTheme)
{
}

void ThemeIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
}

QPixmap ThemeIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
    if (NamedIcon::pixmaps.contains(_sha)) {
        const auto pixmap = NamedIcon::pixmaps[_sha];

        if (!pixmap.isNull()) {
            const auto recoloredPixmap = recolorPixmap(pixmap, qApp->palette().color(QPalette::ColorGroup::Normal, getColorRoleForCurrentTheme()));
            const auto recoloredIcon = QIcon(recoloredPixmap);

            return recoloredIcon.pixmap(size, mode, state);
        }
    }

	return {};
}

QIconEngine* ThemeIconEngine::clone() const
{
    return new ThemeIconEngine(*this);
}

QPixmap ThemeIconEngine::recolorPixmap(const QPixmap& pixmap, const QColor& color)
{
    auto image = pixmap.toImage();

    QPainter painter(&image);

    painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    painter.fillRect(image.rect(), color);
    painter.end();

    return { QPixmap::fromImage(image) };
}

bool ThemeIconEngine::isDarkTheme()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    return QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
#else
    return QApplication::palette().color(QPalette::Window).lightness() < 128;
#endif
}

QPalette::ColorRole ThemeIconEngine::getColorRoleForCurrentTheme() const
{
    return isDarkTheme() ? _colorRoleDarkTheme : _colorRoleLightTheme;
}

}
