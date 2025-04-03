// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StyledIconEngine.h"

#include <QDebug>
#include <QPainter>
#include <QStyleHints>
#include <QGraphicsPixmapItem>

namespace mv::util
{

StyledIconEngine::StyledIconEngine(const StyledIconSettings& styledIconSettings) :
    _iconSettings(styledIconSettings)
{
}

void StyledIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
    painter->drawPixmap(rect, pixmap(rect.size(), mode, state));
}

QPixmap StyledIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
    if (StyledIcon::pixmaps.contains(_iconSettings._sha)) {
	    auto iconPixmap = StyledIcon::pixmaps[_iconSettings._sha].copy();

    	if (!iconPixmap.isNull()) {
            QPixmap result;

		    switch (_iconSettings._mode) {
                case StyledIconMode::ThemeAware:
                {
                    mode = std::clamp(mode, QIcon::Normal, QIcon::Active);

                    const auto recolorColor     = qApp->palette().color(static_cast<QPalette::ColorGroup>(mode), _iconSettings.getColorRoleForCurrentTheme());
                    const auto scaledPixmap     = iconPixmap.scaled(size, Qt::AspectRatioMode::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation);
                    const auto recoloredPixmap  = recolorPixmap(scaledPixmap, size, recolorColor);
                    
					result = recoloredPixmap;

                    break;
                }

                case StyledIconMode::FixedColor:
                {
                    const auto scaledPixmap = iconPixmap.scaled(size, Qt::AspectRatioMode::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation);
                    
                    result = recolorPixmap(scaledPixmap, size, _iconSettings._fixedColor);

                    break;
                }
		    }

            if (!_iconSettings._modifierSha.isEmpty()) {
                const auto recolorColor         = qApp->palette().color(static_cast<QPalette::ColorGroup>(mode), _iconSettings.getColorRoleForCurrentTheme());
                const auto modifierIconPixmap   = recolorPixmap(StyledIcon::pixmaps[_iconSettings._modifierSha], size / 2, recolorColor);

                QPainter modifierIconPixmapPainter(&result);

                modifierIconPixmapPainter.setRenderHint(QPainter::Antialiasing);
                modifierIconPixmapPainter.setRenderHint(QPainter::SmoothPixmapTransform, true);
                modifierIconPixmapPainter.setRenderHint(QPainter::LosslessImageRendering, true);
                modifierIconPixmapPainter.drawPixmap(QPoint(size.width() / 2, size.height() / 2), modifierIconPixmap);
            }
            
            auto& badgeParameters = _iconSettings._badgeParameters;

            if (badgeParameters._enabled) {
                std::int32_t iconMargin = 0;

                const auto iconPixmapSize       = size + QSize(2 * iconMargin, 2 * iconMargin);
                const auto iconPixmapRectangle  = QRectF(QPoint(0, 0), iconPixmapSize);
                const auto badgePixmap          = badgeParameters._customPixmap.isNull() ? gui::createNumberBadgeOverlayPixmap(badgeParameters._number, badgeParameters._backgroundColor, badgeParameters._foregroundColor) : badgeParameters._customPixmap;
                const auto badgeRectangle       = QRectF(QPoint(0, 0), badgePixmap.size());

                QPixmap badgeIconPixmap(iconPixmapSize);

                badgeIconPixmap.fill(Qt::transparent);

                QPainter badgeIconPixmapPainter(&badgeIconPixmap);

                badgeIconPixmapPainter.setRenderHint(QPainter::Antialiasing);
                badgeIconPixmapPainter.setRenderHint(QPainter::SmoothPixmapTransform, true);
                badgeIconPixmapPainter.setRenderHint(QPainter::LosslessImageRendering, true);
            	badgeIconPixmapPainter.drawPixmap(QRectF(QPoint(iconMargin, iconMargin), iconPixmapSize), result, iconPixmapRectangle);

                const auto badgeScale           = badgeParameters._scale;
                const auto scaledIconPixmapSize = badgeScale * iconPixmapSize;
                const auto badgeAlignment       = badgeParameters._alignment;

                QPoint origin;

                if (badgeAlignment & Qt::AlignTop)
                    origin.setY(0);

                if (badgeAlignment & Qt::AlignVCenter)
                    origin.setY((0.5 * iconPixmapSize.height()) - ((0.5 * badgeScale) * iconPixmapSize.height()));

                if (badgeAlignment & Qt::AlignBottom)
                    origin.setY((1.0f - badgeScale) * iconPixmapSize.height());

                if (badgeAlignment & Qt::AlignLeft)
                    origin.setX(0);

                if (badgeAlignment & Qt::AlignHCenter)
                    origin.setX((0.5 * iconPixmapSize.width()) - ((0.5 * badgeScale) * iconPixmapSize.width()));

                if (badgeAlignment & Qt::AlignRight)
                    origin.setX((1.0f - badgeScale) * iconPixmapSize.width());

                badgeIconPixmapPainter.drawPixmap(QRectF(origin, scaledIconPixmapSize), badgePixmap, badgeRectangle);

                return badgeIconPixmap;
            }

            return result;
    	}
        else {
            qWarning() << "StyledIconEngine::pixmap() - pixmap is null" << _iconSettings._sha;
        }
    }

	return {};
}

QIconEngine* StyledIconEngine::clone() const
{
    return new StyledIconEngine(*this);
}

QPixmap StyledIconEngine::recolorPixmap(const QPixmap& pixmap, const QSize& size, const QColor& color)
{
    QPixmap coloredPixmap(size);
    coloredPixmap.fill(Qt::transparent);

    QPainter painter(&coloredPixmap);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::LosslessImageRendering, true);

    painter.drawPixmap(0, 0, size.width(), size.height(), pixmap);

    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(QRect(QPoint(0, 0), size), color);

    painter.end();

    return coloredPixmap;
}

}
