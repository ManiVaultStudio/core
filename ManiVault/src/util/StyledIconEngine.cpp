// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StyledIconEngine.h"
#include "StyledIcon.h"

#include <QDebug>
#include <QPainter>
#include <QStyleHints>
#include <windows.h>

#include "actions/ColorAction.h"

namespace mv::util
{

StyledIconEngine::StyledIconEngine(StyledIcon& styledIcon) :
    _styledIcon(styledIcon),
    _colorGroupLightTheme(QPalette::ColorGroup::Normal),
    _colorGroupDarkTheme(QPalette::ColorGroup::Normal),
	_colorRoleLightTheme(QPalette::ColorRole::Text),
	_colorRoleDarkTheme(QPalette::ColorRole::Text)
{
    _styledIcon._iconEngine = this;
}

StyledIconEngine::StyledIconEngine(StyledIcon& styledIcon, const QString& sha, const QPalette::ColorGroup& colorGroupLightTheme, const QPalette::ColorGroup& colorGroupDarkTheme, const QPalette::ColorRole& colorRoleLightTheme, const QPalette::ColorRole& colorRoleDarkTheme) :
    _styledIcon(styledIcon),
    _sha(sha),
    _colorGroupLightTheme(colorGroupLightTheme),
    _colorGroupDarkTheme(colorGroupDarkTheme),
    _colorRoleLightTheme(colorRoleLightTheme),
    _colorRoleDarkTheme(colorRoleDarkTheme)
{
}

StyledIconEngine::StyledIconEngine(const StyledIconEngine& other) :
    StyledIconEngine(other._styledIcon, other._sha, other._colorGroupLightTheme, other._colorGroupDarkTheme, other._colorRoleLightTheme, other._colorRoleDarkTheme)
{
}

StyledIconEngine::~StyledIconEngine()
{
    _styledIcon._iconEngine = nullptr;
}

void StyledIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
}

QPixmap StyledIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
    if (StyledIcon::pixmaps.contains(_sha)) {
	    const auto pixmap = StyledIcon::pixmaps[_sha];

    	if (!pixmap.isNull()) {
            const auto recolorColor     = qApp->palette().color(getColorGroupForCurrentTheme(), getColorRoleForCurrentTheme());
    		const auto recoloredPixmap  = recolorPixmap(pixmap, recolorColor);
    		const auto recoloredIcon    = QIcon(recoloredPixmap);

            //if (_styledIcon.getBadge().getEnabled()) {
            //    std::int32_t iconMargin = 0;

            //    const auto iconPixmapSize       = size + QSize(2 * iconMargin, 2 * iconMargin);
            //    const auto iconPixmapRectangle  = QRectF(QPoint(0, 0), iconPixmapSize);
            //    const auto badgePixmap          = _styledIcon.getBadge().getPixmap();
            //    const auto badgeRectangle       = QRectF(QPoint(0, 0), badgePixmap.size());

            //    QPixmap iconPixmap(iconPixmapSize);

            //    iconPixmap.fill(Qt::transparent);

            //    QPainter iconPixmapPainter(&iconPixmap);

            //    iconPixmapPainter.setRenderHint(QPainter::Antialiasing);
            //    iconPixmapPainter.drawPixmap(QRectF(QPoint(iconMargin, iconMargin), iconPixmapSize), pixmap, iconPixmapRectangle);

            //    if (_styledIcon.getBadge().getEnabled()) {
            //        const auto badgeScale           = _styledIcon.getBadge().getScale();
            //        const auto scaledIconPixmapSize = badgeScale * iconPixmapSize;
            //        const auto badgeAlignment       = _styledIcon.getBadge().getAlignment();

            //        QPoint origin;

            //        if (badgeAlignment & Qt::AlignTop)
            //            origin.setY(0);

            //        if (badgeAlignment & Qt::AlignVCenter)
            //            origin.setY((0.5 * iconPixmapSize.height()) - ((0.5 * badgeScale) * iconPixmapSize.height()));

            //        if (badgeAlignment & Qt::AlignBottom)
            //            origin.setY((1.0f - badgeScale) * iconPixmapSize.height());

            //        if (badgeAlignment & Qt::AlignLeft)
            //            origin.setX(0);

            //        if (badgeAlignment & Qt::AlignHCenter)
            //            origin.setX((0.5 * iconPixmapSize.width()) - ((0.5 * badgeScale) * iconPixmapSize.width()));

            //        if (badgeAlignment & Qt::AlignRight)
            //            origin.setX((1.0f - badgeScale) * iconPixmapSize.width());

            //        iconPixmapPainter.drawPixmap(QRectF(origin, scaledIconPixmapSize), badgePixmap, badgeRectangle);
            //    }
            //}

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
    return mv::theme().isSystemDarkColorSchemeActive() ? _colorGroupDarkTheme : _colorGroupLightTheme;
}

QPalette::ColorRole StyledIconEngine::getColorRoleForCurrentTheme() const
{
    return mv::theme().isSystemDarkColorSchemeActive() ? _colorRoleDarkTheme : _colorRoleLightTheme;
}

}
