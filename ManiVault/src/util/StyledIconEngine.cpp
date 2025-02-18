// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StyledIconEngine.h"
#include "StyledIcon.h"

#include <QDebug>
#include <QPainter>
#include <QStyleHints>

#include "actions/ColorAction.h"

namespace mv::util
{

StyledIconEngine::StyledIconEngine(StyledIcon& styledIcon) :
    _colorGroupLightTheme(QPalette::ColorGroup::Normal),
    _colorGroupDarkTheme(QPalette::ColorGroup::Normal),
	_colorRoleLightTheme(QPalette::ColorRole::Text),
	_colorRoleDarkTheme(QPalette::ColorRole::Text)
{
    styledIcon._iconEngine = this;
}

StyledIconEngine::StyledIconEngine(const QString& sha, const QPalette::ColorGroup& colorGroupLightTheme, const QPalette::ColorGroup& colorGroupDarkTheme, const QPalette::ColorRole& colorRoleLightTheme, const QPalette::ColorRole& colorRoleDarkTheme) :
    _sha(sha),
    _colorGroupLightTheme(colorGroupLightTheme),
    _colorGroupDarkTheme(colorGroupDarkTheme),
    _colorRoleLightTheme(colorRoleLightTheme),
    _colorRoleDarkTheme(colorRoleDarkTheme)
{
}

StyledIconEngine::StyledIconEngine(const StyledIconEngine& other) :
    StyledIconEngine(other._sha, other._colorGroupLightTheme, other._colorGroupDarkTheme, other._colorRoleLightTheme, other._colorRoleDarkTheme)
{
}

void StyledIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
}

QPixmap StyledIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
    if (StyledIcon::pixmaps.contains(_sha)) {
	    const auto pixmap = StyledIcon::pixmaps[_sha];

    	if (!pixmap.isNull()) {
    		const auto recoloredPixmap  = recolorPixmap(pixmap, qApp->palette().color(getColorGroupForCurrentTheme(), getColorRoleForCurrentTheme()));
    		const auto recoloredIcon    = QIcon(recoloredPixmap);

    		return recoloredIcon.pixmap(size, mode, state);
    	}
    }

	return {};
}

QIconEngine* StyledIconEngine::clone() const
{
    return new StyledIconEngine(*this);
}

QPixmap StyledIconEngine::recolorPixmap(const QPixmap& pixmap, const QColor& color)
{
    auto image = pixmap.toImage();

    QPainter painter(&image);

    painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    painter.fillRect(image.rect(), color);
    painter.end();

    return { QPixmap::fromImage(image) };
}

QPalette::ColorGroup StyledIconEngine::getColorGroupForCurrentTheme() const
{
    return mv::theme().isDarkColorSchemeActive() ? _colorGroupDarkTheme : _colorGroupLightTheme;
}

QPalette::ColorRole StyledIconEngine::getColorRoleForCurrentTheme() const
{
    return mv::theme().isDarkColorSchemeActive() ? _colorRoleDarkTheme : _colorRoleLightTheme;
}

}
